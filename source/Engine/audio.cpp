#include "audio.h"
#include <SDL2/SDL_mixer.h>
#include <iostream>

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
	//SDL_InitSubSystem(SDL_INIT_AUDIO);

	//Buffer size partially fixes stuttering
	//if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 16384) == -1)
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
	Mix_Quit();
	Mix_CloseAudio();
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
	if (path.empty() && sound != "silence")
	{
		LOG_F(ERROR, "Could not find sound: %s", sound.c_str());
		//Still need to store sound, so can transition
		//return;
	}

	//silence is placeholder for default
	//Path will be empty if sound not found
	if (sound != "silence" && path != "")
	{
		LOG_F(INFO, "Opening sound: %s as chunk.", path.c_str());
		std::string ext = path.substr(path.length() - 4, 4);
		if (ext == ".his")
		{
			std::basic_ifstream<unsigned char> file(path, std::ios::binary);
			if (file)
			{
				std::vector<unsigned char> v;
				//wav
				if (!CheckIfOgg(&file))
				{
					std::ifstream inFile = std::ifstream(path, std::ios::in | std::ios::binary | std::ios::ate);
					if (inFile.fail()) {
						inFile.close();
						LOG_F(ERROR, "Unable to open HIF file:%s", path.c_str());
						//return;
					}

					inFile.seekg(0);

					std::string magic = readString(inFile, 4);
					if (magic != "HIS\0")
					{
						LOG_F(ERROR, "Invalid header in file: %s", path.c_str());
						//return;
					}

					//ver major													ver minor
					if (!AssertShort(inFile, 2) || !AssertShort(inFile, 0))
					{
						LOG_F(ERROR, "Invalid version in file: %s", path.c_str());
						//return;
					}

					short wavFormat = readShort(inFile);
					short numChannels = readShort(inFile);
					int samplerate = readInt(inFile);
					int avgBytesPerSecond = readInt(inFile);
					short bitsPerSample = readShort(inFile);
					short blockAlign = readShort(inFile);
					//Only seems to be valid with wav data
					int fileLength = readInt(inFile);
					//version?
					readShort(inFile);

					inFile.close();

					//Calculated values
					samplerate = samplerate / numChannels;

					//tmp lazy hardcode
					file.seekg(0x18);
					std::vector<unsigned char> soundData = std::vector<unsigned char>((std::istreambuf_iterator<unsigned char>(file)),
						std::istreambuf_iterator<unsigned char>());

					//Construct WAV header for sdl_mixer
					pushSringToVector("RIFF", &v);
					pushIntToVector(fileLength + 32, &v);
					pushSringToVector("WAVEfmt ", &v);
					pushIntToVector(16, &v);
					pushShortToVector(wavFormat, &v);
					pushShortToVector(numChannels, &v);
					pushIntToVector(samplerate, &v);
					pushIntToVector(avgBytesPerSecond, &v);
					pushShortToVector(bitsPerSample, &v);
					pushShortToVector(blockAlign, &v);
					pushSringToVector("data", &v);

					//insert pcm data
					v.insert(v.end(), soundData.begin(), soundData.end());

					//Does  not work
					//player->Clip = SDL_Mix_Chunk_ptr(Mix_QuickLoad_RAW(v.data(), v.size()));
				}
				//ogg
				else
				{
					file.seekg(0x1e, std::ios::beg);
					v = std::vector<unsigned char>((std::istreambuf_iterator<unsigned char>(file)),
						std::istreambuf_iterator<unsigned char>());
				}
				SDL_RWops* rw = SDL_RWFromMem((void*)v.data(), (int)v.size());
				if (!rw)
				{
					LOG_F(ERROR, "Could not create RWop: %s", SDL_GetError());
					//Still need to store sound, so can transition
					//return;
				}
				else
					player->Clip = SDL_Mix_Chunk_ptr(Mix_LoadWAV_RW(rw, 1));
			}
			else
				LOG_F(ERROR, "Failed to load sound file: %s", path.c_str());
		}
		//not .his - ogg or wav
		else
		{
			player->Clip = SDL_Mix_Chunk_ptr(Mix_LoadWAV(path.c_str()));
			if (player->Clip == NULL)
			{
				LOG_F(ERROR, "Failed to load sound: %s , %s", path.c_str(), Mix_GetError());
				return;
			}
		}
		player->ClipName = path;
	}
	else
		LOG_F(INFO, "    Silencing channel %d", channel);

	player->channel = channel;
	player->volL = volL;
	player->volR = volR;
	player->loop = loop;
	//Means just play, don't transition after
	if (scene != "9999")
		player->changeTo = scene;

	Audio::sounds.push_back(player);

	//TODO: vol here and music
}

void Audio::AddMusic(std::string sound, int channel, int loop, int volL, int volR)
{
	if (currentMusic && sound == currentMusic->ClipName)
		return;

	AudioClip_ptr player = std::make_shared<AudioClip>();

	std::string path = Loader::getSoundPath(sound);

	LOG_F(INFO, "Opening sound: %s as music.", path.c_str());

	std::string ext = path.substr(path.length() - 4, 4);
	if (ext == ".his")
	{
		std::basic_ifstream<unsigned char> file(path, std::ios::binary);
		if (file)
		{
			if (CheckIfOgg(&file))
			{
				file.seekg(0x1e);
				player->musicMem = std::vector<unsigned char>((std::istreambuf_iterator<unsigned char>(file)),
					std::istreambuf_iterator<unsigned char>());

				SDL_RWops* rw = SDL_RWFromMem((void*)player->musicMem.data(), (int)player->musicMem.size());
				//Mix_Music* mus = Mix_LoadMUSType_RW(rw, MUS_OGG, 1);
				//Seems to require v to continue to exist
				player->Music = SDL_Mix_Music_ptr(Mix_LoadMUS_RW(rw, 1));
			}
			else
			{
				LOG_F(ERROR, "Only Ogg supported as music right now: %s", path.c_str());
			}
		}
	}
	else
	{
		player->Music = SDL_Mix_Music_ptr(Mix_LoadMUS(path.c_str()));
	}

	if (player->Music.get() == NULL)
	{
		LOG_F(ERROR, "Failed to load music: %s , %s", path.c_str(), Mix_GetError());
	}

	player->ClipName = sound;
	player->channel = channel;
	player->volL = volL;
	player->volR = volR;
	player->loop = loop;

	currentMusic = player;
}

void Audio::CheckTransitions()
{
	//TODO: Doesn't transfer when file not found
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

bool Audio::CheckIfOgg(std::basic_ifstream<unsigned char>* file)
{
	//Where ogg file should start
	file->seekg(0x1E);
	unsigned char testVal = 0;
	file->get(testVal);
	return (char)testVal == 'O';
}

void Audio::pushIntToVector(int value, std::vector<unsigned char>* v)
{
	v->push_back(value);
	v->push_back(value >> 8);
	v->push_back(value >> 16);
	v->push_back(value >> 24);
}

void Audio::pushShortToVector(short value, std::vector<unsigned char>* v)
{
	v->push_back(value);
	v->push_back(value >> 8);
}

void Audio::pushSringToVector(std::string value, std::vector<unsigned char>* v)
{
	v->insert(v->end(), value.begin(), value.end());
}