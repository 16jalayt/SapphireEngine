#pragma once
#include <vector>

#include "Engine/graphics.h"

class AVF
{
public:
	static std::vector<SDL_Texture_ptr> parseAVF(const char* file);

private:
	static std::vector<uint8_t> createBMPHeader(int length, int width, int height);
	static SDL_Texture* AVFPNG(std::vector<uint8_t> frame, int width, int height);
	static SDL_Texture* AVFBMP(std::vector<uint8_t> frame, int width, int height);
};