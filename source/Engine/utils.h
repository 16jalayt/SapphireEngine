#pragma once

#include <SDL2/SDL.h>
#include "SDL_ptr.h"
#include "globals.h"
#include "sprite.h"
#include <fstream>
#include <string>

void fatalError(const char* fmt, ...);
void quit();
int rand_range(int min, int max);
void initControls();
void loadingscreen(SDL_Renderer_sptr renderer, SDL_Texture_sptr loading_tex);
SDL_Rect ScaledRect_to_SDLRect(Scaled_Rect rect);
SDL_Rect UnScaledRect_to_SDLRect(UnScaled_Rect rect, int Yoffset, int Xoffset);
SDL_Rect ScaleRect(SDL_Rect rect, float scale = GlobalScale);
SDL_Rect ScaleRect(int x, int y, int w, int h, float scale = GlobalScale);
int readInt(std::ifstream& inFile, bool bigEndian = false);
short readShort(std::ifstream& inFile);
char readByte(std::ifstream& inFile);
std::string readString(std::ifstream& inFile, int length);
void skipBytes(std::ifstream& inFile, int skip);