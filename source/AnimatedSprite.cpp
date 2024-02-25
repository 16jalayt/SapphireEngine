#include "AnimatedSprite.h"
#include <loguru.hpp>

AnimatedSprite::AnimatedSprite(std::vector<SDL_Texture_ptr> frames, int x, int y, RenderParent parent, Scaled_Rect partial) :Sprite(SDL_Texture_ptr(frames[0].get()), x, y, parent)
{
	if (!frames.empty())
	{
		_frames = std::move(frames);
		//for(auto frame : frames)
		//_frames.insert( std::move(frame));
		playing = true;
	}
	else
	{
		LOG_F(ERROR, "Unable to open animated sprite with empty vector.\n");
	}
}

void AnimatedSprite::Draw()
{
	if (currentFrameNum >= _frames.size()) {
		playing = false;
	}

	//TODO: make possible to play while not visible
	if (_visible && playing)
	{
		auto now = std::chrono::high_resolution_clock::now();

		if ((1000 / (int32_t)framerate) >= (int32_t)(std::chrono::duration_cast<std::chrono::milliseconds>(lastFrameTime - now).count()))
		{
			//Get pointer and create new smart pointer because texture is unique_ptr
			_tex = SDL_Texture_ptr(_frames.at(currentFrameNum).get());
			Sprite::Draw();
			lastFrameTime = std::chrono::high_resolution_clock::now();
			currentFrameNum++;
		}

		if (looped)
		{
			// handle looping
			if (currentFrameNum == _frames.size()) {
				currentFrameNum = 0;
			}
		}
	}
}