#include "Engine/Utils.h"

#ifdef __SWITCH__
#include <switch.h>
#endif

#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "Engine/AudioEngine.h"
#include "Engine/Globals.h"
#include "Engine/Graphics.h"
#include <loguru.hpp>

#ifdef __SWITCH__
//to test switch romfs
void printfile(const char* path)
{
	FILE* f = fopen(path, "r");
	if (f)
	{
		char mystring[100];
		while (fgets(mystring, sizeof(mystring), f))
		{
			size_t a = strlen(mystring);
			if (mystring[a - 1] == '\n')
			{
				mystring[a - 1] = 0;
				if (mystring[a - 2] == '\r')
					mystring[a - 2] = 0;
			}
			puts(mystring);
		}
		printf(">>EOF<<\n");
		fclose(f);
	}
	else {
		printf("errno is %d, %s\n", errno, strerror(errno));
	}
}
#endif

void fatalError(const char* fmt, ...)
{
#ifdef __SWITCH__
	va_list va;
	va_start(va, fmt);
	vfprintf(stderr, fmt, va);
	va_end(va);

	//For testing
	//printf("fstest!\n");
	//printfile("folder/file.txt");

	//Kernel panics
	quit();

#else
	va_list va;
	va_start(va, fmt);
	vprintf(fmt, va);
	va_end(va);
	LOG_F(ERROR, "\nExiting...\n");
	quit();
#endif
}

//Only gets called if close window
//Closing console will just force close
void quit()
{
	Graphics::Quit();
	AudioEngine::Quit();
	loguru::shutdown();

#ifdef __SWITCH__
	romfsExit();
	consoleExit(NULL);
#endif
	exit(0);
}

int rand_range(int min, int max)
{
	return min + rand() / (RAND_MAX / (max - min + 1) + 1);
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

//TODO: add message
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

short readShort(std::ifstream& inFile, bool bigEndian)
{
	short result = -1;

	if (bigEndian)
	{
		unsigned char buf[2];
		inFile.read((char*)buf, sizeof(buf) / sizeof(*buf));
		result = (int)buf[1] | (int)buf[0] << 8;
	}
	else
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
	delete[] buf;

	return result;
}

void skipBytes(std::ifstream& inFile, int skip)
{
	inFile.seekg(skip, std::ios_base::cur);
}

bool AssertShort(std::ifstream& inFile, short val, bool bigEndian)
{
	short readValue = readShort(inFile, bigEndian);

	if (readValue != val)
	{
		LOG_F(ERROR, "Value at position: %d\n", ((int)inFile.tellg() - 2));
		LOG_F(ERROR, "Expected value %d got %d\n", val, readValue);

		return false;
	}
	return true;
}
bool AssertInt(std::ifstream& inFile, int val, bool bigEndian)
{
	int readValue = readInt(inFile, false);

	if (readValue != val)
	{
		LOG_F(ERROR, "Value at position: %d\n", ((int)inFile.tellg() - 2));
		LOG_F(ERROR, "Expected value %d got %d\n", val, readValue);

		return false;
	}
	return true;
}