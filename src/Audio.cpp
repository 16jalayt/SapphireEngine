#include "Engine/Audio.h"
#include <SDL2/SDL_mixer.h>
#include <iostream>

#include "Engine/Utils.h"
#include "Engine/Config.h"
#include <loguru.hpp>

using namespace Engine;

/*std::vector<AudioClip_ptr> Audio::getSounds()
{
	return sounds;
}*/
AudioClip_ptr Audio::currentMusic;
std::vector<AudioClip_ptr> Audio::sounds;

int Audio::Init()
{
	//TODO:remove or disable error handling
	if (Config::debugNoSound) {
		return 0;
	}
	//Should be initialized by graphics on startup with everything call
	//SDL_InitSubSystem(SDL_INIT_AUDIO);

	//Buffer size partially fixes stuttering
	//if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
	//if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 16384) == -1)
	//TODO: try to put back after switch audio
	//if (Mix_OpenAudio(48000, AUDIO_S16, 2, 4096) == -1)
#ifdef __SWITCH__
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
#else
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 16384) == -1)
#endif
	{
		fatalError("%s: Failed to load Audio: %s", __func__, Mix_GetError());
		return -1;
	}

	if (Mix_Init(MIX_INIT_OGG | MIX_INIT_MP3) < 0)
	{
		fatalError("%s: SDL could not initialize audio formats! SDL Error: %s", __func__, Mix_GetError());
		return -1;
	}

	Mix_AllocateChannels(20);
	return 0;
}

void Audio::Quit()
{
	if (Config::debugNoSound) {
		return;
	}
	// stop sounds and free loaded data
	Mix_HaltChannel(-1);
	Mix_Quit();
	Mix_CloseAudio();
}

void Audio::RemoveAllSounds()
{
	sounds.clear();
}

/*
void Audio::AddSound(std::string sound, int channel, int loop, int volL, int volR)
{
	//TODO: generic implementation
	LOG_F(WARNING, "Audio::AddSound Stub");
}

void Audio::AddMusic(std::string sound, int channel, int loop, int volL, int volR)
{
	LOG_F(WARNING, "Audio::AddMusic Stub");
}

void Audio::PlaySound()
{
	LOG_F(WARNING, "Audio::PlaySound Stub");
}

void Audio::PauseSound()
{
	LOG_F(WARNING, "Audio::PauseSound Stub");
}

void Audio::AddTransition(std::string scene)
{
	LOG_F(WARNING, "Audio::AddTransition Stub");
}*/