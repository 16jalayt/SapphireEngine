#include <sstream>
#include <SDL2/SDL_image.h>

#include <oldhertools/lzss.hpp>
#include <oldhertools/crypt_misc.hpp>
#include <oldhertools/colorspace.hpp>

#include "Nancy/AVF.h"
#include "Engine/utils.h"
#include <oldhertools/avf.hpp>

//#include <png++/png.hpp>
#include <iostream>
#include <iomanip>
#include <loguru.hpp>

//TODO: Everything touched by avf leaks memory becasue it is static
std::vector<SDL_Texture_ptr> AVF::parseAVF(const char* file)
{
	std::ifstream inFile = std::ifstream(file, std::ios::in | std::ios::binary | std::ios::ate);
	if (inFile.fail()) {
		inFile.close();
		LOG_F(ERROR, "Unable to open AVF file:%s", file);
		return std::vector<SDL_Texture_ptr>();
	}

	inFile.seekg(0);

	std::string magic = readString(inFile, 15);
	if (magic != "AVF WayneSikes")
	{
		LOG_F(ERROR, "Invalid header in file: %s", file);
		return std::vector<SDL_Texture_ptr>();
	}

	//ver major													ver minor
	if (!AssertShort(inFile, 2, true) || !AssertShort(inFile, 0, true))
	{
		LOG_F(ERROR, "Invalid version in file: %s", file);
		return std::vector<SDL_Texture_ptr>();
	}

	int unknown = readShort(inFile);

	int numEntries = readShort(inFile);
	int width = readShort(inFile);
	int height = readShort(inFile);

	//From OldHerTools
	std::vector<Her::AVF_idxEntry> jumpList = Her::getAVFindex(inFile, numEntries);

	std::vector<SDL_Texture_ptr> frames;
	char* rawData;

	for (auto& i : jumpList) {
		inFile.seekg(i.startAt);
		rawData = new char[i.goFor];
		inFile.read(rawData, i.goFor);
		// decrypt & decompress bytes
		std::vector<uint8_t> frame = Her::dec_LZSS(Her::sub_offsets(rawData, i.goFor));

		SDL_Texture* buffer;
		if (numEntries == 1)
			buffer = RGB555_888_SDL(frame, width, height);
		else
			buffer = RGB555_888_SDL(frame, width, height);
		//buffer = AVFPNG(frame, width, height);

		if (buffer == NULL)
		{
			LOG_F(ERROR, "Failed to load AVF: %s",
				IMG_GetError());
			return std::vector<SDL_Texture_ptr>();
		}
		frames.push_back(SDL_Texture_ptr(buffer));

		delete[] rawData;
	}

	inFile.close();

	return frames;
}

//tmp png is not great method
/*SDL_Texture* AVFPNG(std::vector<uint8_t> frame, int width, int height)
{
	png::image<png::rgb_pixel> frameimg = Her::MakePNG(frame, width, height);

	frameimg.write("tmp.png");
	//SDL_RWops* rw = SDL_RWFromMem(buffer2, size);
	SDL_RWops* rw = SDL_RWFromFile("tmp.png", "r");
	return IMG_LoadTexture_RW(Graphics::renderer.get(), rw, 1); //1 means free rw after loading
	//TODO: delete tmp?
}*/

SDL_Texture* RGB555_888_SDL(std::vector<uint8_t> col555, int width, int height) {
	unsigned char* pixels = nullptr;
	int pitch = 0;
	SDL_Texture* texture = SDL_CreateTexture(Graphics::renderer.get(), SDL_PIXELFORMAT_ARGB1555, SDL_TEXTUREACCESS_STREAMING, width, height);

	// Now let's make our "pixels" pointer point to the texture data.
	if (SDL_LockTexture(texture, nullptr, (void**)&pixels, &pitch))
	{
		// If the locking fails, you might want to handle it somehow. SDL_GetError(); or something here.
		LOG_F(ERROR, "Failed to lock AVF");
	}

	memcpy(pixels, col555.data(), pitch * height);

	// Also don't forget to unlock your texture once you're done.
	SDL_UnlockTexture(texture);

	return texture;
}

/*SDL_Texture* RGB555_888_SDL2drugTrip(std::vector<uint8_t> col555, int width, int height) {
	unsigned char* pixels = nullptr;
	int pitch = 0;
	SDL_Texture* texture = SDL_CreateTexture(Graphics::renderer.get(), SDL_PIXELFORMAT_BGR565, SDL_TEXTUREACCESS_STREAMING, width, height);

	// Now let's make our "pixels" pointer point to the texture data.
	if (SDL_LockTexture(texture, nullptr, (void**)&pixels, &pitch))
	{
		// If the locking fails, you might want to handle it somehow. SDL_GetError(); or something here.
		printf("fail");
	}

	int j = 0;
	for (unsigned int i = 0; i < col555.size(); i += 1) {
		pixels[i] = col555.at(i);
	}

	// Also don't forget to unlock your texture once you're done.
	SDL_UnlockTexture(texture);

	return texture;
}*/
/*
SDL_Texture* AVFBMP(std::vector<uint8_t> frame, int width, int height)
{
	std::vector<uint8_t> frameConv = Her::RGB555_888BGR(frame);

	std::vector<uint8_t> bmpHeader = createBMPHeader(frameConv.size(), width, height);
	frameConv.insert(frameConv.begin(), bmpHeader.begin(), bmpHeader.end());

	SDL_RWops* rw = SDL_RWFromMem(frameConv.data(), frameConv.size());
	return IMG_LoadTexture_RW(Graphics::renderer.get(), rw, 1); //1 means free rw after loading
}

std::vector<uint8_t>  createBMPHeader(int length, int width, int height)
{
	std::ostringstream output;
	//magic
	output.write((char const*)&"BM", sizeof(uint8_t) * 2);

	//length of file (data+header)
	int length1 = length + 54;
	output.write((char const*)&length1, 4);

	//reserved
	//Crashes when passing directly to write
	uint32_t a = 0;
	output.write((char const*)&a, 4);

	//Pixel data offset reletive to start of file
	a = 54;
	output.write((char const*)&a, 4);

	//Image header length
	a = 40;
	output.write((char const*)&a, 4);

	//width
	output.write((char const*)&width, 4);

	//height
	a = 0 - height;
	output.write((char const*)&a, 4);

	//Image planes. Must be 1
	a = 1;
	output.write((char const*)&a, 2);

	//bpp, 24
	a = 24;
	output.write((char const*)&a, 2);

	//compression type
	a = 0;
	output.write((char const*)&a, 4);

	//size of image, no compression, so 0
	output.write((char const*)&a, 4);

	//perfered width
	output.write((char const*)&a, 4);

	//prefered height
	output.write((char const*)&a, 4);

	//Colors used
	output.write((char const*)&a, 4);

	//important colors
	output.write((char const*)&a, 4);

	const std::string& str = output.str();
	std::vector<uint8_t> bmpHeader = std::vector<uint8_t>();
	bmpHeader.insert(bmpHeader.end(), str.begin(), str.end());

	return bmpHeader;
}*/