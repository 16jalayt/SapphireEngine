#pragma once

#include <SDL2/SDL_audio.h>
#include "AudioPacket.h"
#include <memory>

extern "C"
{
#include <libavcodec/avcodec.h>
	//#include <libavdevice/avdevice.h>
#include <libavfilter/avfilter.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavutil/avutil.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>
}

class FFAudio
{
public:
	FFAudio(AVCodecContext*);
	~FFAudio();

	struct SwrContext* swrCtx = NULL;
	AVFrame wanted_frame;

	AudioPacket audioq;

	void open();

	static int getAudioPacket(AudioPacket*, AVPacket*, int);
	int audio_decode_frame(AVCodecContext*, uint8_t*, int);
	int put_audio_packet(AVPacket*);

private:
	static FFAudio* instance;
	SDL_AudioSpec wantedSpec = { 0 }, audioSpec = { 0 };
};

using FFAudio_ptr = std::shared_ptr<FFAudio>;