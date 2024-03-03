#include "Engine/AudioEngine.h"
#include <SDL2/SDL_mixer.h>
#include <iostream>

#include "Engine/Utils.h"
#include "Engine/Config.h"
#include <loguru.hpp>

/*std::vector<AudioClip_ptr> AudioEngine::getSounds()
{
	return sounds;
}*/
AudioClip_ptr AudioEngine::currentMusic;
std::vector<AudioClip_ptr> AudioEngine::sounds;

int AudioEngine::Init()
{
	//TODO:remove or disable error handling
	if (Config::debugNoSound) {
		return 0;
	}
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
		fatalError("%s: Failed to load AudioEngine: %s", __func__, Mix_GetError());
		return -1;
	}

	if (Mix_Init(MIX_INIT_OGG | MIX_INIT_MP3) < 0)
	{
		fatalError("%s: SDL could not initialize audio formats! SDL Error: %s", __func__, Mix_GetError());
		return -1;
	}

	//TODO:correct number
	Mix_AllocateChannels(20);
	return 0;
}

void AudioEngine::Quit()
{
	if (Config::debugNoSound) {
		return;
	}
	// stop sounds and free loaded data
	Mix_HaltChannel(-1);
	Mix_Quit();
	Mix_CloseAudio();
}

void AudioEngine::AddSound(std::string sound, int channel, int loop, int volL, int volR)
{
	//TODO: generic implementation
	LOG_F(WARNING, "AudioEngine::AddSound Stub");
}

void AudioEngine::AddMusic(std::string sound, int channel, int loop, int volL, int volR)
{
	LOG_F(WARNING, "AudioEngine::AddMusic Stub");
}

void AudioEngine::PlaySound()
{
	LOG_F(WARNING, "AudioEngine::PlaySound Stub");
}

void AudioEngine::PauseSound()
{
	LOG_F(WARNING, "AudioEngine::PauseSound Stub");
}

void AudioEngine::RemoveAllSounds()
{
	//sounds.clear();
}

void AudioEngine::AddTransition(std::string scene)
{
	LOG_F(WARNING, "AudioEngine::AddTransition Stub");
}