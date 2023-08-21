#pragma once

#include "FFAudio.h"

class AudioCallback
{
public:
	static void set_audio_instance(FFAudio*);
	static void audio_callback(void*, Uint8*, int);

private:
	AudioCallback() {}
	static FFAudio* audio_instance;
};

static bool stopaudio = false;