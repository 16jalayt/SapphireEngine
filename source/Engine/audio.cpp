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
		player->ClipName = sound;
	}
	else
		player->ClipName = path;

	//silence is placeholder for default
	//Path will be empty if sound not found
	if (sound != "silence")
	{
		if (path != "")
		{
			LOG_F(INFO, "Opening sound: %s as chunk.", path.c_str());
			std::string ext = path.substr(path.length() - 4, 4);
			if (ext == ".his")
			{
				SDL_RWops* file = SDL_RWFromFile(path.c_str(), "rb");
				if (file)
				{
					std::vector<char> v;
					//wav
					if (!CheckIfOgg(file))
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

						//TODO: error handle file
						file->seek(file, 0, RW_SEEK_END);
						//TODO: will return -1 if cannot complete
						unsigned long fileLen = (unsigned long)SDL_RWtell(file);
						//tmp lazy hardcode
						file->seek(file, 0x18, RW_SEEK_SET);

						char* buffer;
						buffer = (char*)malloc(fileLen - 0x18 + 1);
						if (!buffer)
						{
							fprintf(stderr, "Memory error!");
							SDL_RWclose(file);
							return;
						}

						//Read file contents into buffer
						SDL_RWread(file, buffer, fileLen, 1);
						SDL_RWclose(file);

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
						char* end = buffer + fileLen;
						v.insert(v.end(), buffer, end);
						free(buffer);

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
					//ogg
					else
					{
						SDL_RWops* file = SDL_RWFromFile(path.c_str(), "rb");
						file->seek(file, 0x1e, RW_SEEK_SET);
						player->Clip = SDL_Mix_Chunk_ptr(Mix_LoadWAV_RW(file, 1));
					}
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
		}

		player->channel = channel;
		player->volL = volL;
		player->volR = volR;
		player->loop = loop;
		//Means just play, don't transition after
		if (scene != "9999" || scene != "")
			player->changeTo = scene;

		Audio::sounds.push_back(std::move(player));

		//TODO: vol here and music
	}
	else
		LOG_F(INFO, "    Silencing channel %d", channel);
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
		SDL_RWops* file = SDL_RWFromFile(path.c_str(), "rb");
		if (file)
		{
			if (CheckIfOgg(file))
			{
				file->seek(file, 0x1e, RW_SEEK_SET);
				player->Music = SDL_Mix_Music_ptr(Mix_LoadMUS_RW(file, 1));
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

	if (player->Music.get() == NULL && !Config::debugNoSound)
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
	for (int i = 0; i < sounds.size(); i++)
	{
		//TODO: investigate null clip names being created
		//same remote button twice loops
		//clicker can still get stuck sometimes

		if (!sounds.at(i) || sounds.at(i)->ClipName == "")
			LOG_F(WARNING, "clip name empty or null");
		else
		{
			int test = Mix_Playing(sounds.at(i)->channel);
			//if sound not playing and scene transition set
			if (!sounds.at(i)->changeTo.empty() && Mix_Playing(sounds.at(i)->channel) == 0)
			{
				Loader::loadScene(sounds.at(i)->changeTo);
			}
		}
	}
}

void Audio::PlaySound()
{
	LOG_F(WARNING, "Audio::PlaySound Stub");
}

void Audio::PauseSound()
{
	LOG_F(WARNING, "Audio::PauseSound Stub");
}

void Audio::RemoveAllSounds()
{
	sounds.clear();
}

void Audio::AddTransition(std::string scene)
{
	LOG_F(WARNING, "Audio::AddTransition Stub");
}

//problems with switch reading char
bool Audio::CheckIfOgg(SDL_RWops* file)
{
	//Where ogg file should start
	file->seek(file, 0x1e, RW_SEEK_SET);
	char testVal = 0;
	file->read(file, &testVal, 1, 1);
	//LOG_F(ERROR, "testval: %c", testVal);
	return (char)testVal == 'O';
}

void Audio::pushIntToVector(int value, std::vector<char>* v)
{
	v->push_back(value);
	v->push_back(value >> 8);
	v->push_back(value >> 16);
	v->push_back(value >> 24);
}

void Audio::pushShortToVector(short value, std::vector<char>* v)
{
	v->push_back((char)value);
	v->push_back(value >> 8);
}

void Audio::pushSringToVector(std::string value, std::vector<char>* v)
{
	v->insert(v->end(), value.begin(), value.end());
}