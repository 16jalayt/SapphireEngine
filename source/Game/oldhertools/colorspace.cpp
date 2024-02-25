// colorspace.cpp --- various colorspace converters

#include "colorspace.hpp"

//Might not be 555, ARGB1555 works
namespace Her {
	std::vector<uint8_t> RGB555_888(std::vector<uint8_t> col555) {
		std::vector<uint8_t> col888;

		uint8_t colR;
		uint8_t colG;
		uint8_t colB;

		for (unsigned int i = 0; i < col555.size(); i += 2) {
			colR = (col555.at(i + 1) & 0x7C) << 1;
			colG = (col555.at(i + 1) & 0x03) << 6 | (col555.at(i) & 0xE0) >> 2;
			colB = (col555.at(i) & 0x1F) << 3;

			colR |= colR >> 5;
			colG |= colG >> 5;
			colB |= colB >> 5;

			col888.push_back(colR);
			col888.push_back(colG);
			col888.push_back(colB);
		}

		return col888;
	}

	//Rest of file added by 16jalayt
	std::vector<uint8_t> RGB555_888BGR(std::vector<uint8_t> col555) {
		std::vector<uint8_t> col888;

		uint8_t colR;
		uint8_t colG;
		uint8_t colB;

		for (unsigned int i = 0; i < col555.size(); i += 2) {
			colR = (col555.at(i + 1) & 0x7C) << 1;
			colG = (col555.at(i + 1) & 0x03) << 6 | (col555.at(i) & 0xE0) >> 2;
			colB = (col555.at(i) & 0x1F) << 3;

			colR |= colR >> 5;
			colG |= colG >> 5;
			colB |= colB >> 5;

			//16jalayt Flipped byte order from rgb to bgr
			col888.push_back(colB);
			col888.push_back(colG);
			col888.push_back(colR);
		}

		return col888;
	}

	/*png::image<png::rgb_pixel> MakePNG(std::vector<uint8_t> col555, int width, int height) {
		std::vector<uint8_t> col888;
		png::image<png::rgb_pixel> outImg(width, height);

		uint8_t colR;
		uint8_t colG;
		uint8_t colB;

		for (unsigned int i = 0; i < col555.size(); i += 2) {
			colR = (col555.at(i + 1) & 0x7C) << 1;
			colG = (col555.at(i + 1) & 0x03) << 6 | (col555.at(i) & 0xE0) >> 2;
			colB = (col555.at(i) & 0x1F) << 3;

			colR |= colR >> 5;
			colG |= colG >> 5;
			colB |= colB >> 5;

			outImg.set_pixel(i / 2 % width, i / 2 / width, png::rgb_pixel(colR, colG, colB));
			col888.push_back(colR);
			col888.push_back(colG);
			col888.push_back(colB);
		}

		return outImg;
	}*/

	/*void RGB555_888_SDL(std::vector<uint8_t> col555, SDL_Texture*& texture) {
		Uint8 colR;
	Uint8 colG;
	Uint8 colB;

	unsigned char* pixels = nullptr;
	int pitch = 0;
	SDL_Texture* texture = SDL_CreateTexture(Graphics::renderer.get(), SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, width, height);

	// Now let's make our "pixels" pointer point to the texture data.
	if (SDL_LockTexture(texture, nullptr, (void**)&pixels, &pitch))
	{
		// If the locking fails, you might want to handle it somehow. SDL_GetError(); or something here.
		printf("fail");
	}

	int j = 0;
	for (unsigned int i = 0; i < col555.size(); i += 2) {
		colR = (col555.at(i + 1) & 0x7C) << 1;
		colG = (col555.at(i + 1) & 0x03) << 6 | (col555.at(i) & 0xE0) >> 2;
		colB = (col555.at(i) & 0x1F) << 3;

		colR |= colR >> 5;
		colG |= colG >> 5;
		colB |= colB >> 5;

		pixels[j] = colR;
		pixels[j + 1] = colG;
		pixels[j + 2] = colB;
		j += 3;
	}

	// Also don't forget to unlock your texture once you're done.
	SDL_UnlockTexture(texture);

	return texture;
	}*/
}