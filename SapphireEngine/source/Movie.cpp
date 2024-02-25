#include "Movie.h"
#include <Nancy/AVF.h>
#include <loguru.hpp>

Movie::Movie(std::string fileName, int x, int y, bool isLooped, RenderParent parent, Scaled_Rect partial)
{
	//determine if video or graphic and open
	std::string ext = fileName.substr(fileName.find("."));

	if (ext == ".bik")
	{
		//FMV = make_BinkPlayback_s(new BinkPlayback());
		/*FMV = std::make_shared<BinkPlayback>();
		FMV->OpenPaused(fileName, x, y);
		FMV->_isLooped = isLooped;*/

		FMV = std::make_shared<FFPlayer>(fileName, x, y, isLooped);
	}
	else if (ext == ".avf")
	{
		std::vector<SDL_Texture_ptr> test = AVF::parseAVF(fileName.c_str());
		if (test.empty())
		{
			LOG_F(WARNING, "parseAVF returned empty for: %s\n", fileName.c_str());
			return;
		}
		ASprite = std::make_unique<AnimatedSprite>(std::move(test), x, y);
		//ASprite = AnimatedSprite_ptr(new AnimatedSprite(std::move(AVF::parseAVF(fileName.c_str())), 0, 0));
		ASprite->looped = isLooped;
	}
	else
		LOG_F(WARNING, "Unknown Movie type in: %s\n", fileName.c_str());
}

void Movie::Draw()
{
	if (this != NULL)
	{
		if (FMV)
		{
			/*if (singleThreadVideo)
			{
				if ((Bink_GetCurrentFrameNum(FMV->_handle) < Bink_GetNumFrames(FMV->_handle)) && !FMV->IsEnding()) {
					// Bink_GetNextFrame(fmv->_handle, fmv->_yuvBuffer);
					int test = Bink_GetNextFrame(FMV->_handle, FMV->_yuvBuffer);
					printf("frame:%i\n", test);

					// we have a new frame and we're ready to use it
					FMV->_frameReady = true;
				}
			}*/

			FMV->Draw();
			//if (!FMV->_fmvEnding)
				//FMV->Close();
		}

		else if (ASprite)
			ASprite->Draw();
	}
}

//TODO:
//void setLooped

Movie::~Movie()
{
	//if (FMV)
		//FMV->Close();
}