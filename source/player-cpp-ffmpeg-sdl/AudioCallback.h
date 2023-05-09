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

class AudioCallback
{
public:
	static void set_audio_instance(FFAudio*);
	static void audio_callback(void*, Uint8*, int);

private:
	AudioCallback() {}

	static FFAudio* audio_instance;
};