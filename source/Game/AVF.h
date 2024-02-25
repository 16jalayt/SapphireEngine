#pragma once
#include <vector>

#include "Engine/graphics.h"

class AVF
{
public:
	static std::vector<SDL_Texture_ptr> parseAVF(const char* file);

private:
};

SDL_Texture* AVFPNG(std::vector<uint8_t> frame, int width, int height);
//SDL_Texture* AVFBMP(std::vector<uint8_t> frame, int width, int height);
//std::vector<uint8_t> createBMPHeader(int length, int width, int height);
SDL_Texture* RGB555_888_SDL(std::vector<uint8_t> col555, int width, int height);