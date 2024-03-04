#pragma once

#include "SDL_ptr.h"
#include <SDL2/SDL_mixer.h>
#include <string>
#include <vector>
#include "Globals.h"
#include <memory>

namespace Engine
{
	class SE_API AudioClip
	{
	public:
		std::string ClipName = "";
		SDL_Mix_Music_ptr Music = NULL;
		std::vector<unsigned char> musicMem;
		SDL_Mix_Chunk_ptr Clip = NULL;
		int channel = -1;
		bool loop = false;
		int volL = -1;
		int volR = -1;
		bool hasPlayedOnce = false;
		std::string changeTo = "";

		void Play();

	protected:
	};
}

using AudioClip_ptr = std::shared_ptr<Engine::AudioClip>;