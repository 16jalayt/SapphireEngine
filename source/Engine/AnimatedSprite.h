#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include <chrono>

#include "sprite.h"
#include "SDL_ptr.h"
#include "globals.h"

class AnimatedSprite : public Sprite
{
public:
	AnimatedSprite(std::vector<SDL_Texture_ptr> frames, int x, int y, RenderParent parent = RenderParent::canvas, Scaled_Rect partial = Scaled_Rect());
	void Draw();
	bool playing = false;
	int framerate = ANIMATION_RATE;
	std::vector<SDL_Texture_ptr> _frames;
	bool looped = false;
	int currentFrameNum = 0;
	std::chrono::high_resolution_clock::time_point lastFrameTime;

private:
};

using AnimatedSprite_ptr = std::shared_ptr<AnimatedSprite>;