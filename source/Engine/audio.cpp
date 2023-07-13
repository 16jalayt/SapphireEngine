#include "audio.h"
#include <SDL2/SDL_mixer.h>

#include "Engine/utils.h"
#include "Config.h"
#include <Nancy/Loader.h>
#include <loguru.hpp>

AudioClip_ptr Audio::currentMusic;
//AudioClip_ptr soundChannels[8];
std::vector<AudioClip_ptr> Audio::sounds;

int Audio::Init()
{
	if (Config::debugNoSound) {
		return 0;
	}
	SDL_InitSubSystem(SDL_INIT_AUDIO);

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
	{
		fatalError("%s: Failed to load Audio: %s", __func__, Mix_GetError());
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

void Audio::Quit()
{
	if (Config::debugNoSound) {
		return;
	}
	// stop sounds and free loaded data
	Mix_HaltChannel(-1);
	Mix_CloseAudio();
	Mix_Quit();
}

void Audio::AddSound(std::string sound, int channel, int loop, int volL, int volR)
{
	AddSound(sound, channel, loop, volL, volR, "");
}

void Audio::AddSound(std::string sound, int channel, int loop, int volL, int volR, std::string scene)
{
	AudioClip_ptr player = std::make_shared<AudioClip>();

	//if hdAudio then mus, else cdAudio chunk
	std::string path = Loader::getSoundPath(sound);
	if (path.empty())
	{
		LOG_F(ERROR, "Could not find sound: %s", sound.c_str());
		return;
	}

	LOG_F(INFO, "Opening sound: %s as chunk.", path.c_str());
	player->Clip = SDL_Mix_Chunk_ptr(Mix_LoadWAV(path.c_str()));
	if (player->Clip == NULL)
	{
		LOG_F(ERROR, "Failed to load sound: %s , %s", path.c_str(), Mix_GetError());
		return;
	}

	player->ClipName = path;
	player->channel = channel;
	player->volL = volL;
	player->volR = volR;
	player->loop = loop;
	//Means don't transition
	if (scene != "9999")
		player->changeTo = scene;

	Audio::sounds.push_back(player);

	//TODO: vol here and music
}

void Audio::AddMusic(std::string sound, int channel, int loop, int volL, int volR)
{
	AudioClip_ptr player = std::make_shared<AudioClip>();

	std::string path = Loader::getSoundPath(sound);

	//TODO: check name insted of path?
	if (currentMusic && path == currentMusic->ClipName)
		return;

	LOG_F(INFO, "Opening sound: %s as music.", path.c_str());
	player->Music = SDL_Mix_Music_ptr(Mix_LoadMUS(path.c_str()));
	if (player->Music == NULL)
	{
		LOG_F(ERROR, "Failed to load music: %s , %s", path.c_str(), Mix_GetError());
	}

	player->ClipName = path;
	player->channel = channel;
	player->volL = volL;
	player->volR = volR;
	player->loop = loop;

	currentMusic = player;
}

void Audio::CheckTransitions()
{
	for (auto sound : sounds)
	{
		//if sound not playing and scene transition set
		if (Mix_Playing(sound->channel) != 0 && !sound->changeTo.empty())
		{
			Loader::loadScene(sound->changeTo);
		}
	}
}

void Audio::PlaySound()
{
}

void Audio::PauseSound()
{
}

void Audio::RemoveAllSounds()
{
}

void Audio::AddTransition(std::string scene)
{
}