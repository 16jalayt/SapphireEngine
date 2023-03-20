#include <string>
#include <SDL2/SDL_mixer.h>

class Audio
{
public:
	static int Init();
	static void Quit();
};
extern Mix_Music* currentMusic;
extern std::string currentMusicName;
extern Mix_Chunk* soundChannels[8];