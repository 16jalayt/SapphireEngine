#include "Engine/player-cpp-ffmpeg-sdl/FFAudio.h"
#include <assert.h>
#include "Engine/player-cpp-ffmpeg-sdl/defs.h"
#include "Engine/player-cpp-ffmpeg-sdl/AudioCallback.h"
#include "Engine/Audio.h"

FFAudio::FFAudio(AVCodecContext* pCodecAudioCtx)
{
	AudioCallback::set_audio_instance(this);

	swrCtx = swr_alloc();
	if (swrCtx == NULL)
	{
		printf("FFAudio: Failed to load audio\n");
		return;
	}

	av_opt_set_channel_layout(swrCtx, "in_channel_layout", pCodecAudioCtx->channel_layout, 0);
	av_opt_set_channel_layout(swrCtx, "out_channel_layout", pCodecAudioCtx->channel_layout, 0);
	av_opt_set_int(swrCtx, "in_sample_rate", pCodecAudioCtx->sample_rate, 0);
	av_opt_set_int(swrCtx, "out_sample_rate", pCodecAudioCtx->sample_rate, 0);
	av_opt_set_sample_fmt(swrCtx, "in_sample_fmt", pCodecAudioCtx->sample_fmt, 0);
	av_opt_set_sample_fmt(swrCtx, "out_sample_fmt", AV_SAMPLE_FMT_FLT, 0);

	int res = swr_init(swrCtx);

	if (res != 0)
	{
		printf("FFAudio: Failed to initialize audio\n");
		return;
	}

	memset(&wantedSpec, 0, sizeof(wantedSpec));

	wantedSpec.channels = pCodecAudioCtx->channels;
	wantedSpec.freq = pCodecAudioCtx->sample_rate;
	wantedSpec.format = AUDIO_S16SYS;
	wantedSpec.silence = 0;
	wantedSpec.samples = SDL_AUDIO_BUFFER_SIZE;
	wantedSpec.userdata = pCodecAudioCtx;
	wantedSpec.callback = AudioCallback::audio_callback;
}
FFAudio::~FFAudio()
{
	stopaudio = true;
	SDL_PauseAudio(true);
	SDL_CloseAudio();
	binkAudioLock = false;
}

void FFAudio::open()
{
	//SDL_AudioInit("winmm");
	SDL_AudioInit("directsound");
	if (SDL_OpenAudio(&wantedSpec, &audioSpec) < 0)
	{
		printf("FFAudio: Failed to open audio\n");
		return;
	}
	//Currently due to callback, only one bink can play audio;
	binkAudioLock = true;

	wanted_frame.format = AV_SAMPLE_FMT_S16;
	wanted_frame.sample_rate = audioSpec.freq;
	wanted_frame.channel_layout = av_get_default_channel_layout(audioSpec.channels);
	wanted_frame.channels = audioSpec.channels;

	audioq.nb_packets = 0;
	audioq.size = 0;
	audioq.last = NULL;
	audioq.first = NULL;
	audioq.mutex = SDL_CreateMutex();
	audioq.cond = SDL_CreateCond();

	SDL_PauseAudio(0);
}

int FFAudio::audio_decode_frame(AVCodecContext* aCodecCtx, uint8_t* audio_buf, int buf_size) {
	static AVPacket pkt;
	static uint8_t* audio_pkt_data = NULL;
	static int audio_pkt_size = 0;
	static AVFrame frame;

	int len1;
	int data_size = 0;

	SwrContext* swr_ctx = NULL;

	while (1 && !stopaudio)
	{
		while (audio_pkt_size > 0)
		{
			avcodec_send_packet(aCodecCtx, &pkt);
			avcodec_receive_frame(aCodecCtx, &frame);

			len1 = frame.pkt_size;
			if (len1 < 0)
			{
				audio_pkt_size = 0;
				break;
			}

			audio_pkt_data += len1;
			audio_pkt_size -= len1;

			data_size = 0;

			/*if (frame.channels > 0 && frame.channel_layout == 0)
				frame.channel_layout = av_get_default_channel_layout(frame.channels);
			else if (frame.channels == 0 && frame.channel_layout > 0)
				frame.channels = av_get_channel_layout_nb_channels(frame.channel_layout);*/

				//needed?
			if (swr_ctx)
			{
				swr_free(&swr_ctx);
				swr_ctx = NULL;
			}

			swr_ctx = swr_alloc_set_opts(NULL, wanted_frame.channel_layout, (AVSampleFormat)wanted_frame.format, wanted_frame.sample_rate,
				frame.channel_layout, (AVSampleFormat)frame.format, frame.sample_rate, 0, NULL);

			if (!swr_ctx || swr_init(swr_ctx) < 0)
			{
				printf("FFAudio: swr_init failed\n");
				return 0;
			}

			int dst_nb_samples = (int)av_rescale_rnd(swr_get_delay(swr_ctx, frame.sample_rate) + frame.nb_samples,
				wanted_frame.sample_rate, wanted_frame.format, AV_ROUND_INF);

			int len2 = swr_convert(swr_ctx, &audio_buf, dst_nb_samples,
				(const uint8_t**)frame.data, frame.nb_samples);
			if (len2 < 0)
			{
				printf("FFAudio: swr_convert failed\n");
				return 0;
			}

			av_packet_unref(&pkt);

			if (swr_ctx)
			{
				swr_free(&swr_ctx);
				swr_ctx = NULL;
			}

			return wanted_frame.channels * len2 * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
		}

		if (getAudioPacket(&audioq, &pkt, 1) < 0)
			return -1;

		audio_pkt_data = pkt.data;
		audio_pkt_size = pkt.size;
	}
	return -1;
}

int FFAudio::put_audio_packet(AVPacket* packet)
{
	AVPacketList* pktl;
	AVPacket* newPkt;
	newPkt = (AVPacket*)av_calloc(1, sizeof(AVPacket));
	//newPkt = (AVPacket*)av_mallocz_array(1, sizeof(AVPacket));
	if (av_packet_ref(newPkt, packet) < 0)
		return -1;

	pktl = (AVPacketList*)av_malloc(sizeof(AVPacketList));
	if (!pktl)
		return -1;

	pktl->pkt = *newPkt;
	pktl->next = NULL;

	SDL_LockMutex(audioq.mutex);

	if (!audioq.last)
		audioq.first = pktl;
	else
		audioq.last->next = pktl;

	audioq.last = pktl;

	audioq.nb_packets++;
	audioq.size += newPkt->size;

	SDL_CondSignal(audioq.cond);
	SDL_UnlockMutex(audioq.mutex);

	return 0;
}

int FFAudio::getAudioPacket(AudioPacket* q, AVPacket* pkt, int block) {
	AVPacketList* pktl;
	int ret = 0;

	SDL_LockMutex(q->mutex);

	while (1 && !stopaudio)
	{
		pktl = q->first;
		if (pktl)
		{
			q->first = pktl->next;
			if (!q->first)
				q->last = NULL;

			q->nb_packets--;
			q->size -= pktl->pkt.size;

			*pkt = pktl->pkt;
			av_free(pktl);
			ret = 1;
			break;
		}
		else if (!block)
		{
			ret = 0;
			break;
		}
		else
		{
			SDL_CondWait(q->cond, q->mutex);
		}
	}

	SDL_UnlockMutex(q->mutex);

	return ret;
}