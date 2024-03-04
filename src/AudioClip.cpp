#include "Engine/AudioClip.h"

using namespace Engine;

void AudioClip::Play()
{
	if (Music && !Mix_PlayingMusic())
	{
		Mix_HaltMusic();
		//TODO: music does not loop
		Mix_PlayMusic(Music.get(), loop ? 0 : -1);
	}
	else if (Clip)
	{
		//TODO: error handle
		if (!hasPlayedOnce)
		{
			Mix_PlayChannel(channel, Clip.get(), loop ? 0 : -1);
			hasPlayedOnce = true;
		}
	}
}

//TODO: stop