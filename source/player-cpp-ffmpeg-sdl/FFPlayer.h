#pragma once
//SDL #define _USE_MATH_DEFINES, FFMPEG defines it's own version
#pragma warning(disable: 4005)

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavfilter/avfilter.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavutil/avutil.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
}

#include <iostream>
#include <SDL2/SDL_render.h>
#include "AudioPacket.h"
#include "FFAudio.h"

class FFPlayer
{
public:
	FFPlayer(std::string filename);
	~FFPlayer();

	AVCodecParameters* pCodecAudioParameters = NULL;
	void Draw();

	//TODO:implement
	bool playing = true;
	bool looped = false;

private:
	void OpenStream(std::string filename);

	int malloc(void);

	int get_video_stream(void);

	int read_audio_video_codec(void);

	int videoStream = 0;
	int audioStream = 0;

	AVFormatContext* pFormatCtx = NULL;

	AVCodecParameters* pCodecParameters = NULL;

	AVCodecContext* pCodecCtx = NULL;
	AVCodecContext* pCodecAudioCtx = NULL;

	const AVCodec* pCodec = NULL;
	const AVCodec* pAudioCodec = NULL;
	AVFrame* pFrame = NULL;
	AVFrame* pFrameRGB = NULL;

	uint8_t* buffer = NULL;

	SDL_Texture* bmp = NULL;

	struct SwsContext* sws_ctx = NULL;

	FFAudio_ptr audio;
};

using Player_ptr = std::shared_ptr<FFPlayer>;