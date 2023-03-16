// colorspace.hpp --- definitions of colorspace functions

#pragma once

#include <vector>
#include <cstdint>
#include <SDL2/SDL_render.h>

namespace Her {
	std::vector<uint8_t> RGB555_888(std::vector<uint8_t> col555);
	void RGB555_888_SDL(std::vector<uint8_t> col555, SDL_Texture*& texture);
	std::vector<uint8_t> RGB888_YCbCr(std::vector<uint8_t> img_RGB);
}