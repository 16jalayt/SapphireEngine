#include "AudioClip.h"

void AudioClip::Play()
{
	if (Music && Mix_PlayingMusic() == 0)
	{
		Mix_HaltMusic();
		Mix_PlayMusic(Music.get(), loop ? 1 : -1);
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