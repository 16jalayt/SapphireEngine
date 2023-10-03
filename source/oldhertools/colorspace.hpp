// colorspace.hpp --- definitions of colorspace functions

#pragma once

#include <vector>
#include <cstdint>
//#include <png++/png.hpp>

namespace Her {
	std::vector<uint8_t> RGB555_888(std::vector<uint8_t> col555);

	std::vector<uint8_t> RGB555_888BGR(std::vector<uint8_t> col555);
	//png::image<png::rgb_pixel> MakePNG(std::vector<uint8_t> col555, int width, int height);
}