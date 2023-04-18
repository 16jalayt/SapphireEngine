#include "AnimatedSprite.h"

AnimatedSprite::AnimatedSprite(SDL_Texture_ptr textures[], int x, int y, RenderParent parent, Scaled_Rect partial) :Sprite(SDL_Texture_ptr(textures[0].get()), x, y, parent)
{
	if (textures)
	{
	}
	else
	{
		printf("Unable to open animated sprite with texture.\n");
	}
}

void AnimatedSprite::Draw()
{
	if (_visible)
	{
		Sprite::Draw();
	}
}