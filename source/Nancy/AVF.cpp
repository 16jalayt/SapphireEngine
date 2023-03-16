#include "Nancy/AVF.h"
#include <Engine/utils.h>
#include <oldhertools/lzss.hpp>
#include <oldhertools/crypt_misc.hpp>
#include <oldhertools/colorspace.hpp>
#include <SDL2/SDL_image.h>
#include <sstream>
/* //To test raw images
typedef struct {
	int width;
	int height;
	uint8_t* data;
	size_t size;
} ppm_image;

size_t ppm_save(ppm_image* img, FILE* outfile) {
	size_t n = 0;
	n += fprintf(outfile, "P6\n# THIS IS A COMMENT\n%d %d\n%d\n", img->width, img->height, 0xFF);
	n += fwrite(img->data, 1, img->width * img->height * 3, outfile);
	return n;
}*/

SDL_Texture_ptr AVF::parseAVF(const char* file)
{
	std::ifstream inFile = std::ifstream(file, std::ios::in | std::ios::binary | std::ios::ate);
	if (inFile.fail()) {
		inFile.close();
		printf("Unable to open AVF file:%s\n", file);
		return nullptr;
	}

	inFile.seekg(0);

	std::string magic = readString(inFile, 15);
	if (magic != "AVF WayneSikes")
	{
		printf("Invalid header in file: %s\n", file);
		return nullptr;
	}

	//ver major													ver minor
	if (!AssertShort(inFile, 2, true) || !AssertShort(inFile, 0, true))
	{
		printf("Invalid version in file: %s\n", file);
		return nullptr;
	}

	int unknown = readShort(inFile);

	int numEntries = readShort(inFile);
	int width = readShort(inFile);
	int height = readShort(inFile);

	inFile.ignore(5);

	if (!AssertShort(inFile, 2))
	{
		printf("Unsuppored compression type in file: %s\n", file);
		return nullptr;
	}

	//loop
	int index = readByte(inFile);
	int fileOffset = readInt(inFile);
	int compressedLength = readInt(inFile);
	inFile.ignore(9);

	int tableOffset = inFile.tellg();
	inFile.seekg(fileOffset);

	char* compressedImage = new char[compressedLength];
	inFile.read(compressedImage, compressedLength);

	std::vector<uint8_t> frame = Her::dec_LZSS(Her::sub_offsets(compressedImage, compressedLength));

	//To test raw array before bmp header
	/*uint8_t* array = &frame[0];
	ppm_image* test = new ppm_image();
	test->width = width;
	test->height = height;
	test->size = frame.size();
	test->data = array;

	ppm_save(test, fopen("test.ppm", "w"));
	outFile = std::ofstream("afterCSConv.dat", std::ios::out | std::ios::binary);
	outFile.write((const char*)&frame[0], frame.size());
	outFile.close();*/

	//delete[] compressedImage;
	inFile.seekg(tableOffset, inFile.beg);

	SDL_Texture_ptr buffer = SDL_Texture_ptr(SDL_CreateTexture(Graphics::renderer.get(), SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, width, height));

	std::vector<uint8_t> frameConv = Her::RGB555_888(frame);

	std::vector<uint8_t> bmpHeader = createBMPHeader(frameConv.size(), width, height);
	frameConv.insert(frameConv.begin(), bmpHeader.begin(), bmpHeader.end());

	//To test memory bitmap corectness
	/*std::ofstream outFile("testStream.bmp", std::ios::out | std::ios::binary);
	outFile.write((char*)&frameConv[0], frameConv.size() * sizeof(uint8_t));
	outFile.close();*/

	SDL_RWops* rw = SDL_RWFromMem(&frameConv[0], frameConv.size());
	SDL_Surface* tmpsurf = IMG_Load_RW(rw, 1);
	if (tmpsurf == NULL)
	{
		printf("IMG_Load_RW: %s\n", IMG_GetError());
		return NULL;
	}
	buffer = SDL_Texture_ptr(SDL_CreateTextureFromSurface(Graphics::renderer.get(), tmpsurf));
	SDL_FreeSurface(tmpsurf);

	inFile.close();

	return buffer;
}

std::vector<uint8_t>  AVF::createBMPHeader(int length, int width, int height)
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
}

/*void AVF::playAVF()
{
}*/