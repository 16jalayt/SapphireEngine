#pragma once

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
}

#include "SDL2/SDL_thread.h"

typedef struct _AudioPacket
{
	AVPacketList* first, * last;
	int nb_packets, size;
	SDL_mutex* mutex;
	SDL_cond* cond;
} AudioPacket;