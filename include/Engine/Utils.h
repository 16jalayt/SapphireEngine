#pragma once

#include <fstream>
#include <string>
#include <SDL2/SDL.h>

#include "SDL_ptr.h"
#include "Globals.h"
#include "Config.h"
#include <vector>

SE_API void switchInit();
SE_API void fatalError(const char* fmt, ...);
SE_API void quit();
SE_API int rand_range(int min, int max);
SE_API SDL_Rect ScaledRect_to_SDLRect(Scaled_Rect rect);
SE_API SDL_Rect ScaleRect(SDL_Rect rect, float scale = Engine::Config::globalScale);
SE_API SDL_Rect ScaleRect(int x, int y, int w, int h, float scale = Engine::Config::globalScale);
SE_API int readInt(std::ifstream& inFile, bool bigEndian = false);
SE_API short readShort(std::ifstream& inFile, bool bigEndian = false);
SE_API bool AssertShort(std::ifstream& inFile, short val, bool bigEndian = false);
SE_API bool AssertInt(std::ifstream& inFile, int val, bool bigEndian = false);
SE_API char readByte(std::ifstream& inFile);
SE_API std::string readString(std::ifstream& inFile, int length);
SE_API void skipBytes(std::ifstream& inFile, int skip);
SE_API int getLength(std::ifstream& inFile);
SE_API std::string PathFixer(std::string path);
SE_API bool FileExists(std::string path);
SE_API std::string FindFilePath(std::string fileName, const std::vector<std::string>& paths, const std::vector<std::string>& extensions);