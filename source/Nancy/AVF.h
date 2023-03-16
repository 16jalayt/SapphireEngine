#pragma once

#include <Engine/graphics.h>
#include <vector>

class AVF
{
public:
	static SDL_Texture_ptr parseAVF(const char* file);
	//void playAVF();
private:
	static std::vector<uint8_t> createBMPHeader(int length, int width, int height);
};