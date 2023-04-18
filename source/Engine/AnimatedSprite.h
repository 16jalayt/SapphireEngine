#pragma once

#include <SDL2/SDL.h>

#include "sprite.h"
#include "SDL_ptr.h"
#include "globals.h"

class AnimatedSprite : public Sprite
{
public:
	AnimatedSprite(SDL_Texture_ptr textures[], int x, int y, RenderParent parent = RenderParent::canvas, Scaled_Rect partial = Scaled_Rect());
	void Draw();
	bool playing = false;
	int framerate = ANIMATION_RATE;

private:
};

using AnimatedSprite_ptr = std::shared_ptr<AnimatedSprite>;