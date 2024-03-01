#pragma once

#include <string>
#include <vector>

#include <SDL2/SDL_mixer.h>
#include "AudioClip.h"
#include "Globals.h"
#include <memory>

class SE_API Audio
{
public:
	static int Init();
	static void Quit();

	static AudioClip_ptr currentMusic;
	//static AudioClip_ptr soundChannels[8];
	static std::vector<AudioClip_ptr> sounds;

	static void AddSound(std::string sound, int channel, int loop, int chan1, int chan2);
	//TODO: add persist bool
	static void AddMusic(std::string sound, int channel, int loop, int chan1, int chan2);
	static void PlaySound();
	static void PauseSound();
	static void RemoveAllSounds();
	static void AddTransition(std::string scene);
	//static std::vector<AudioClip_ptr> getSounds();

private:
};

using Audio_ptr = std::unique_ptr<Audio>;