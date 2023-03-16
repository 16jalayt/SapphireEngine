// colorspace.cpp --- various colorspace converters

#include "colorspace.hpp"

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

			//16jalayt Flipped byte order from rgb to bgr
			col888.push_back(colB);
			col888.push_back(colG);
			col888.push_back(colR);
		}

		return col888;
	}

	//added by 16jalayt
	/*void RGB555_888_SDL(std::vector<uint8_t> col555, SDL_Texture*& texture) {
		Uint8 colR;
		Uint8 colG;
		Uint8 colB;

		Uint32* pixels = nullptr;
		int pitch = 0;
		Uint32 format;

		// Get the size of the texture.
		int w, h;
		SDL_QueryTexture(texture, &format, nullptr, &w, &h);

		// Now let's make our "pixels" pointer point to the texture data.
		if (SDL_LockTexture(texture, nullptr, (void**)&pixels, &pitch))
		{
			// If the locking fails, you might want to handle it somehow. SDL_GetError(); or something here.
			printf("fail");
		}

		SDL_PixelFormat pixelFormat;
		pixelFormat.format = format;

		for (unsigned int i = 0; i < col555.size(); i += 2) {
			colR = (col555.at(i + 1) & 0x7C) << 1;
			colG = (col555.at(i + 1) & 0x03) << 6 | (col555.at(i) & 0xE0) >> 2;
			colB = (col555.at(i) & 0x1F) << 3;

			colR |= colR >> 5;
			colG |= colG >> 5;
			colB |= colB >> 5;

			// Now you want to format the color to a correct format that SDL can use.
		// Basically we convert our RGB color to a hex-like BGR color.
			Uint32 color = SDL_MapRGB(&pixelFormat, 50, 50, 50);

			// Before setting the color, we need to know where we have to place it.
			Uint32 pixelPosition = (i / 3 / w) * (pitch / sizeof(unsigned int)) + (3 % w);

			// Now we can set the pixel(s) we want.
			pixels[pixelPosition] = color;
		}

		// Also don't forget to unlock your texture once you're done.
		SDL_UnlockTexture(texture);

		return;
	}*/

	// converts RGB to Y'CbCr 4:4:4 (using Rec. 470M)
	// important to note is that theora won't take the data like this; it
	// prefers a planar separation.
	std::vector<uint8_t> RGB888_YCbCr(std::vector<uint8_t> img_RGB) {
		std::vector<uint8_t> img_YCbCr;

		double Y;
		double Pb;
		double Pr; // intermediate values; YCbCr values go straight into vector

		double R;
		double G;
		double B;

		// subfuncs for calculations
		auto trunc = [](double n) {return (int)n; };
		auto clamp = [](int n) { if (n < 0) { return 0; } else if (n > 255) { return 255; } else { return n; }};

		for (unsigned int i = 0; i < img_RGB.size(); i += 3) {
			// normalize for equations
			R = img_RGB.at(i) / 255.0;
			G = img_RGB.at(i + 1) / 255.0;
			B = img_RGB.at(i + 2) / 255.0;

			Y = .299 * R + .587 * G + .114 * B;
			Pb = -0.16873589113148 * R - 0.33126410922777 * G + 0.5 * B;
			Pr = 0.5 * R - 0.41868759026505 * G - 0.08131241038760 * B;

			img_YCbCr.push_back(clamp(trunc(219 * Y + 16))); // Y
			img_YCbCr.push_back(clamp(trunc(224 * Pb + 128))); // Cb
			img_YCbCr.push_back(clamp(trunc(224 * Pr + 128))); // Cr
		}

		return img_YCbCr;
	}
}