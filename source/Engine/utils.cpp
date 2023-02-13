#ifdef __SWITCH__
#include <switch.h>
#endif

#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "utils.h"
#include "globals.h"
#include "AvP_AudioStreaming.h"
#include "audio.h"
#include "graphics.h"

void fatalError(const char* fmt, ...)
{
#ifdef __SWITCH__
	consoleInit(NULL);
	va_list va;
	va_start(va, fmt);
	vprintf(fmt, va);
	va_end(va);
	printf("\nPlease return to the home menu and restart the application.\n");
	//Keep looping forever. For some reason libnx app crashes on proper exit.
	while (appletMainLoop())
	{
		//Reduce the update interval to not thrash the cpu
		SDL_Delay(50);
		consoleUpdate(NULL);
	}

	quit();
#else
	va_list va;
	va_start(va, fmt);
	vprintf(fmt, va);
	va_end(va);
	printf("\nExiting...\n");
	exit(-1);
#endif
}

void quit()
{
	Graphics::Quit();
	Audio::Quit();

#ifdef __SWITCH__
	romfsExit();
	consoleExit(NULL);
#endif
	exit(-11);
}

int rand_range(int min, int max)
{
	return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

void initControls()
{
	// SDL_InitSubSystem(SDL_INIT_JOYSTICK);
	SDL_JoystickEventState(SDL_ENABLE);
	// open CONTROLLER_PLAYER_1 and CONTROLLER_PLAYER_2
   // when railed, both joycons are mapped to joystick #0,
   // else joycons are individually mapped to joystick #0, joystick #1, ...
   // https://github.com/devkitPro/SDL/blob/switch-sdl2/src/joystick/switch/SDL_sysjoystick.c#L45
	for (int i = 0; i < 2; i++)
	{
		if (SDL_JoystickOpen(i) == NULL)
		{
#ifdef __SWITCH__
			fatalError("%s: SDL_JoystickOpen: %s", __func__, SDL_GetError());
			printf("No joysticks connected\n");
#endif
		}
	}
	//SWITCH_JoystickRumble(SDL_Joystick * joystick, Uint16 low_frequency_rumble, Uint16 high_frequency_rumble)
}

void loadingscreen(SDL_Renderer_sptr renderer, SDL_Texture_sptr loading_tex)
{
	SDL_Surface_ptr loading_surf = SDL_Surface_ptr(IMG_Load("data/Nintendo_Switch_Logo_resized.png"));
	if (loading_surf)
	{
		loading_tex = make_SDL_Texture_s(SDL_CreateTextureFromSurface(renderer.get(), loading_surf.get()));
		//SDL_FreeSurface(loading_surf);
	}
	SDL_Rect fullscreen = { 0, 0, REAL_WIDTH, REAL_HEIGHT };

	//Display loading image by drawing manually
	//Screen will not be cleared until loading done and main loop starts
	//SDL_SetRenderDrawColor(renderer, 0, 100, 0, 0xFF);
	//SDL_SetRenderDrawColor(renderer, 0, 100, 0, 0xFF);
	SDL_RenderClear(renderer.get());
	if (loading_tex)
		SDL_RenderCopy(renderer.get(), loading_tex.get(), NULL, &fullscreen);

	SDL_RenderPresent(renderer.get());
}

SDL_Rect ScaledRect_to_SDLRect(Scaled_Rect rect)
{
	//return { (int)(rect.x * 1.25) + BackgroundOffsetX, (int)(rect.y * 1.25) + BackgroundOffsetY, (int)((rect.x2 - rect.x) * 1.25), (int)((rect.y2 - rect.y) * 1.25) };
	//return { (int)(rect.x * GlobalScale) + Xoffset, (int)(rect.y * GlobalScale) + Yoffset, (int)((rect.x2 - rect.x) * GlobalScale), (int)((rect.y2 - rect.y) * GlobalScale) };
	SDL_Rect newRect;

	//Shouldn't offset because canvas render texture
	/*if (parent == RenderParent::window)
	{
		newRect.x = rect.x;
		newRect.y = rect.y + UpperUIOffset;
	}
	else//RenderParent::canvas
	{
		newRect.x = rect.x + CanvasOffsetX;
		newRect.y = rect.y + CanvasOffsetY;
	}*/

	newRect.x = rect.x;
	newRect.y = rect.y;

	newRect.w = rect.x2 - rect.x;
	newRect.h = rect.y2 - rect.y;
	return newRect;
}
//TODO:remove
/*SDL_Rect UnScaledRect_to_SDLRect(UnScaled_Rect rect, int Yoffset, int Xoffset)
{
	return { rect.x + Xoffset, rect.y + Yoffset, rect.x2 - rect.x , rect.y2 - rect.y };
}*/

SDL_Rect ScaleRect(SDL_Rect rect, float scale)
{
	rect.x = (int)(rect.x * scale);
	rect.y = (int)(rect.y * scale);
	rect.w = (int)(rect.w * scale);
	rect.h = (int)(rect.h * scale);
	return rect;
}

SDL_Rect ScaleRect(int x, int y, int w, int h, float scale)
{
	SDL_Rect rect;
	rect.x = (int)(x * scale);
	rect.y = (int)(y * scale);
	rect.w = (int)(w * scale);
	rect.h = (int)(h * scale);
	return rect;
}

int readInt(std::ifstream& inFile, bool bigEndian)
{
	int result = -1;

	if (bigEndian)
	{
		unsigned char buf[4];
		inFile.read((char*)buf, sizeof(buf) / sizeof(*buf));
		result = (int)buf[3] | (int)buf[2] << 8 | (int)buf[1] << 16 | (int)buf[0] << 24;
	}
	else
		inFile.read((char*)&result, sizeof(result));

	return result;
}

short readShort(std::ifstream& inFile)
{
	short result = -1;

	inFile.read((char*)&result, sizeof(result));

	return result;
}

char readByte(std::ifstream& inFile)
{
	char result = -1;

	inFile.read((char*)&result, sizeof(result));

	return result;
}

std::string readString(std::ifstream& inFile, int length)
{
	char* buf = new char[length];
	inFile.read(buf, length);
	std::string result = std::string(buf, length);
	result.erase(std::find(result.begin(), result.end(), '\0'), result.end());

	return result;
}

void skipBytes(std::ifstream& inFile, int skip)
{
	inFile.seekg(skip, std::ios_base::cur);
}