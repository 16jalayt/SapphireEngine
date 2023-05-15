#include "audio.h"
#include <SDL2/SDL_mixer.h>

#include "globals.h"
#include "Engine/utils.h"
#include "AvP_AudioStreaming.h"

std::string currentMusicName;
Mix_Music* currentMusic;
Mix_Chunk* soundChannels[8];

int Audio::Init()
{
	if (debugNoSound) {
		return 0;
	}
	SDL_InitSubSystem(SDL_INIT_AUDIO);

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
	{
		fatalError("%s: Failed to load Audio: %s", __func__, SDL_GetError());
		return -1;
	}

	if (Mix_Init(MIX_INIT_OGG | MIX_INIT_MP3) < 0)
	{
		fatalError("%s: SDL could not initialize audio formats! SDL Error: %s", __func__, SDL_GetError());
		return -1;
	}

	//Mix_AllocateChannels(5);

	//PlatStartSoundSys();
	return 0;
}

void Audio::Quit()
{
	if (debugNoSound) {
		return;
	}
	// stop sounds and free loaded data
	Mix_HaltChannel(-1);
	Mix_CloseAudio();
	Mix_Quit();
	//PlatEndSoundSys();
}