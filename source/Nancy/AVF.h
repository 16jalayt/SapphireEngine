#pragma once
#include <vector>

#include "Engine/graphics.h"

class AVF
{
public:
	static std::vector<SDL_Texture_ptr> parseAVF(const char* file);
	//void playAVF();
private:
	static std::vector<uint8_t> createBMPHeader(int length, int width, int height);
};