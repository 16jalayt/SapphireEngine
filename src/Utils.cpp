#include "Engine/Utils.h"

#ifdef __SWITCH__
#include <switch.h>
#endif
#ifdef __VITA__
#include <psp2/kernel/processmgr.h>
#endif

#include <algorithm>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "Engine/Audio.h"
#include "Engine/Globals.h"
#include "Engine/Graphics.h"
#include <loguru.hpp>
#include <sys/stat.h>
#include <vector>
#include <sys/stat.h>

using namespace Engine;

#ifdef __SWITCH__
//TODO: move file to static class?
void switchInit()
{
	//Log to Ryujinx
	consoleDebugInit(debugDevice_SVC);

	//Log to nxlink on actual hardware
	socketInitializeDefault();
	nxlinkStdio();
	//use cerr for both to work

	//Running off sd card now so no romfs
	/*Result rc = romfsInit();
	if (R_FAILED(rc))
		printf("romfsInit: %08X\n", rc);
	chdir("romfs:/");*/

	chdir("/switch/PhantomDogs/");
}

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
	LOG_F(ERROR, "Fatal Error: %s\n", fmt);
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
	Audio::Quit();
	loguru::shutdown();

#ifdef __SWITCH__
	romfsExit();
	consoleExit(NULL);
#endif
#ifdef __VITA__
	//Example just used sdl_quit
	sceKernelExitProcess(0);
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
//4 byte 32 bits
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

//2 byte 16 bits
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

//1 byte 8 bits
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

int getLength(std::ifstream& inFile)
{
	long begin = inFile.tellg();
	inFile.seekg(0, std::ios::end);
	long end = inFile.tellg();
	inFile.seekg(begin, std::ios::beg);
	return end;
}

//Hack to get vita paths correct. Can be used for other platforms too
std::string PathFixer(std::string path)
{
#if defined (__VITA__)
	//hack for startup with no config file
	if (Config::gameName == "Sapphire Engine")
		Config::loadStub();

	if (path.find("ux0") == std::string::npos)
		return "ux0:/data/" + Config::gameName + "/" + path;
	else
		return path;
#else
	return path;
#endif
}

//https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exists-using-standard-c-c11-14-17-c
bool FileExists(std::string path)
{
	/*std::string pathTest = PathFixer(path);
	std::ifstream inFile = std::ifstream(pathTest, std::ios::in | std::ios::binary | std::ios::ate);
	if (inFile.good()) {
		inFile.close();
		return true;
	}
	return false;*/
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0);
}

std::string FindFilePath(std::string fileName, const std::vector<std::string>& paths, const std::vector<std::string>& extensions)
{
	for (std::string path : paths)
	{
		for (std::string ext : extensions)
		{
			//FileExists should be applied at the load site
			if (FileExists(PathFixer(path + fileName + ext)))
				return path + fileName + ext;
		}
	}

	LOG_F(ERROR, "Cannot find file: %s", fileName.c_str());
	return std::string();
}