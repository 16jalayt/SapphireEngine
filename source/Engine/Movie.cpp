#include "Movie.h"
#include <Nancy/AVF.h>

Movie::Movie(std::string fileName, int x, int y, bool isLooped, RenderParent parent, Scaled_Rect partial)
{
	//determine if video or graphic and open
	std::string ext = fileName.substr(fileName.find("."));

	if (ext == ".bik")
	{
		FMV = make_BinkPlayback_s(new BinkPlayback());
		FMV->Open(fileName, x, y);
	}
	else if (ext == ".avf")
	{
		//ASprite = AnimatedSprite_ptr(new AnimatedSprite(std::move(AVF::parseAVF(fileName.c_str())), 0, 0));
	}
	else
		printf("Unknown Movie type in: %s\n", fileName.c_str());
}

void Movie::Draw()
{
	if (this != NULL)
	{
		if (FMV)
			FMV->Draw();
		else if (ASprite)
			ASprite->Draw();
	}
}

Movie::~Movie()
{
	if (FMV)
		FMV->Close();
}