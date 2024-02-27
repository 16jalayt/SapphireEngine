#include "FFPlayer.h"
#include "../graphics.h"
#include "defs.h"
#include "../audio.h"
#include "../globals.h"
#include "../GUI.h"
#include "../Config.h"
#include <loguru/loguru.hpp>

//TODO: expose startPaused somewhere
FFPlayer::FFPlayer(std::string filename, int x, int y, bool looped, bool startPaused)
{
	_paused = startPaused;
	_looped = looped;
	//TODO: bik playback on switch is blue
	// [swscaler @ 0x162cfdd010] No accelerated colorspace conversion found from yuv420p to rgb24.
	//LOG_F(ERROR, "Before open stream");
	this->OpenStream(filename);
	//LOG_F(ERROR, "Before malloc");
	this->malloc();
}

FFPlayer::~FFPlayer()
{
	// close context info
	avformat_close_input(&pFormatCtx);
	avcodec_free_context(&pCodecCtx);

	// free buffers
	av_free(buffer);
	av_free(pFrameRGB);

	// Free the YUV frame
	av_free(pFrame);

	// Close the codecs
	avcodec_close(pCodecCtx);

	// Close the video file
	avformat_close_input(&pFormatCtx);
}

void FFPlayer::OpenStream(std::string filename)
{
	audioStream = -1;

	// open video
	int res = avformat_open_input(&pFormatCtx, filename.c_str(), NULL, NULL);

	// check video
	if (res != 0)
	{
		char error_buf[ERROR_SIZE];
		av_strerror(res, error_buf, ERROR_SIZE);
		printf(error_buf);
		return;
	}

	// get video info
	res = avformat_find_stream_info(pFormatCtx, NULL);
	if (res < 0)
	{
		char error_buf[ERROR_SIZE];
		av_strerror(res, error_buf, ERROR_SIZE);
		printf(error_buf);
		return;
	}

	// get video stream
	videoStream = get_video_stream();
	if (videoStream == -1)
	{
		printf("FFPlayer: Error opening your video using AVCodecParameters, probably doesnt have codecpar_type type AVMEDIA_TYPE_VIDEO\n");
		return;
	}

	// open
	read_audio_video_codec();
}

/*
Acquires video stream
*/
int FFPlayer::get_video_stream(void)
{
	int videoStream = -1;

	for (unsigned int i = 0; i < pFormatCtx->nb_streams; i++) {
		if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) videoStream = i;
		if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) audioStream = i;
	}

	if (videoStream == -1)
	{
		printf("FFPlayer: Couldnt find stream\n");
		return NULL;
	}

	pCodecParameters = pFormatCtx->streams[videoStream]->codecpar;
	if (audioStream != -1) pCodecAudioParameters = pFormatCtx->streams[audioStream]->codecpar;

	return videoStream;
}

/*
Reads audio and video codec
*/
int FFPlayer::read_audio_video_codec(void)
{
	pCodec = avcodec_find_decoder(pCodecParameters->codec_id);
	pAudioCodec = avcodec_find_decoder(pCodecAudioParameters->codec_id);

	if (pCodec == NULL)
	{
		printf("FFPlayer: Video decoder not found\n");
		return 0;
	}

	if (pAudioCodec == NULL)
	{
		printf("FFPlayer: Audio decoder not found\n");
		return 0;
	}

	pCodecCtx = avcodec_alloc_context3(pCodec);

	if (pCodecCtx == NULL)
	{
		printf("FFPlayer: Failed to allocate video context decoder\n");
		return 0;
	}

	pCodecAudioCtx = avcodec_alloc_context3(pAudioCodec);

	if (pCodecAudioCtx == NULL)
	{
		printf("FFPlayer: Failed to allocate audio context decoder\n");
		return 0;
	}

	int res = avcodec_parameters_to_context(pCodecCtx, pCodecParameters);

	if (res < 0)
	{
		printf("FFPlayer: Failed to transfer video parameters to context\n");
		return 0;
	}

	res = avcodec_parameters_to_context(pCodecAudioCtx, pCodecAudioParameters);

	if (res < 0)
	{
		printf("FFPlayer: Failed to transfer audio parameters to context\n");
		return 0;
	}

	res = avcodec_open2(pCodecCtx, pCodec, NULL);

	if (res < 0)
	{
		printf("Failed to open video codec\n");
		return 0;
	}

	res = avcodec_open2(pCodecAudioCtx, pAudioCodec, NULL);

	if (res < 0)
	{
		printf("FFPlayer: Failed to open auvio codec\n");
		return 0;
	}

	return 1;
}

/*
Alloc memory for the display
*/
int FFPlayer::malloc(void)
{
	//TODO: error handling for audio not initing
	if (!Config::debugNoSound && !binkAudioLock)
	{
		audio = std::make_shared<FFAudio>(pCodecAudioCtx);
		audio->open();
	}

	pFrame = av_frame_alloc();
	if (pFrame == NULL)
	{
		printf("FFPlayer: Couldnt allocate frame memory\n");
		return 0;
	}

	pFrameRGB = av_frame_alloc();
	if (pFrameRGB == NULL)
	{
		printf("FFPlayer: Couldnt allocate rgb frame memory\n");
		return 0;
	}

	int numBytes = av_image_get_buffer_size(VIDEO_FORMAT, pCodecCtx->width, pCodecCtx->height, 1);

	buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));

	int res = av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, buffer, VIDEO_FORMAT, pCodecCtx->width, pCodecCtx->height, 1);
	if (res < 0)
	{
		char error_buf[ERROR_SIZE];
		av_strerror(res, error_buf, ERROR_SIZE);
		printf(error_buf);
		return 0;
	}

	bmp = SDL_CreateTexture(Graphics::renderer.get(), SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STATIC, pCodecCtx->width, pCodecCtx->height);

	//video context
	sws_ctx = sws_getContext(pCodecCtx->width,
		pCodecCtx->height,
		pCodecCtx->pix_fmt,
		pCodecCtx->width,
		pCodecCtx->height,
		VIDEO_FORMAT,
		SWS_BILINEAR,
		NULL,
		NULL,
		NULL
	);

	return 1;
}

//TODO: thread?
void FFPlayer::parsePacket()
{
	AVPacket packet;

	if (av_read_frame(pFormatCtx, &packet))
	{
		_ended = true;
		av_packet_unref(&packet);
		return;
	}

	if (packet.stream_index == audioStream) {
		if (!Config::debugNoSound)
			audio->put_audio_packet(&packet);

		//Need to get the accompanying video packet this frame
		//parsePacket();
	}
	else if (packet.stream_index == videoStream)
	{
		videoFrameThisFrame = true;

		int res = avcodec_send_packet(pCodecCtx, &packet);
		if (res < 0)
		{
			char error_buf[ERROR_SIZE];
			av_strerror(res, error_buf, ERROR_SIZE);
			printf(error_buf);
			av_packet_unref(&packet);
			return;
		}

		res = avcodec_receive_frame(pCodecCtx, pFrame);

		SDL_UpdateYUVTexture(bmp, NULL,
			pFrame->data[0], pFrame->linesize[0],
			pFrame->data[1], pFrame->linesize[1],
			pFrame->data[2], pFrame->linesize[2]);
	}
	av_packet_unref(&packet);

	if (!videoFrameThisFrame)
		parsePacket();
}

void FFPlayer::Draw()
{
	if (_ended)
	{
		if (_looped)
		{
			//To seek to timebase not frames
			//frmseekPos = av_rescale(seekPos, ic->streams[videoStream]->time_base.den, ic->streams[videoStream]->time_base.num);
			//frmseekPos /= 1000;
			av_seek_frame(pFormatCtx, videoStream, 0, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
			_ended = false;
		}
		else
			return;
	}

	if (!_paused)
	{
		//TODO: kind of stuttery, needs seperate thread?
		//TODO: change in avf player too
		auto now = std::chrono::high_resolution_clock::now();
		//int32_t timeSinceLastFrame = (int32_t)(std::chrono::duration_cast<std::chrono::milliseconds>(now - prevFrame).count());
		int32_t timeSinceLastFrame = (int32_t)(std::chrono::duration_cast<std::chrono::milliseconds>(now - prevFrame).count());
		//printf("%d\n", timeSinceLastFrame);
		if (frameTime < timeSinceLastFrame) {
			videoFrameThisFrame = false;
			parsePacket();
			prevFrame = now;
		}
	}
	//videoFrameThisFrame = false;
	//parsePacket();

	SDL_SetRenderTarget(Graphics::renderer.get(), GUI::canvas.get());
	SDL_RenderCopy(Graphics::renderer.get(), bmp, NULL, NULL);
	SDL_SetRenderTarget(Graphics::renderer.get(), NULL);
}

SDL_Texture_ptr FFPlayer::GetFrame()
{
	videoFrameThisFrame = false;
	parsePacket();

	return SDL_Texture_ptr(bmp);
}