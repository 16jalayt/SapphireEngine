#pragma once

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

#include <SDL2/SDL_audio.h>
#include "AudioPacket.h"

class FFAudio
{
public:
	static FFAudio* get_instance();

	struct SwrContext* swrCtx = NULL;
	AVFrame wanted_frame;

	AudioPacket audioq;

	void open();
	void malloc(AVCodecContext*);
	void init_audio_packet(AudioPacket*);
	int audio_decode_frame(AVCodecContext*, uint8_t*, int);
	int put_audio_packet(AVPacket*);

private:
	FFAudio() {}
	static FFAudio* instance;

	SDL_AudioSpec wantedSpec = { 0 }, audioSpec = { 0 };
};