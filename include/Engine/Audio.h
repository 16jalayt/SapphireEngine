#include <string>
#include <vector>

#include <SDL2/SDL_mixer.h>
#include "AudioClip.h"

class Audio
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
	static void AddSound(std::string sound, int channel, int loop, int chan1, int chan2, std::string scene);
	static void PlaySound();
	static void PauseSound();
	static void RemoveAllSounds();
	static void AddTransition(std::string scene);
	static void CheckTransitions();

private:
	static bool CheckIfOgg(SDL_RWops* file);
	static void pushIntToVector(int value, std::vector<char>* v);
	static void pushShortToVector(short value, std::vector<char>* v);
	static void pushSringToVector(std::string value, std::vector<char>* v);
};
//Currently due to callback, only one bink can play audio at a time.
static bool binkAudioLock = false;