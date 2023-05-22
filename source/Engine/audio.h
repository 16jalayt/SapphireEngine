#include <string>
#include <SDL2/SDL_mixer.h>

class Audio
{
public:
	static int Init();
	static void Quit();
};
//Currently due to callback, only one bink can play audio at a time.
static bool binkAudioLock = false;
extern Mix_Music* currentMusic;
extern std::string currentMusicName;
extern Mix_Chunk* soundChannels[8];