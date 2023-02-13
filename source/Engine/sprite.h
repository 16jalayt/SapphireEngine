#pragma once

#include <string>
#include <SDL2/SDL.h>
#include "SDL_ptr.h"
#include "utils.h"

enum RenderParent { window, canvas };

class Sprite
{
public:
	Sprite() = default;
	Sprite(const char* fileName, int x = 0, int y = 0, RenderParent parent = RenderParent::canvas, int numFrames = 1, int animationSpeed = 0);
	//Sprite(SDL_Renderer_sptr renderer, const char* file, Scaled_Rect rect, int numFrames = 1, int animationSpeed = 0);
	void Draw();
	virtual ~Sprite();
	void destroy();
	bool isloaded();
	bool isVisible();
	bool MouseCollision(SDL_Event event);
	SDL_Rect getPos();
	void visible(bool visible);
	void setScale(float scale);
	float getScale();
	int getWidth();
	int getHeight();
	void setPos(SDL_Rect pos);

protected:
	bool _loaded = false;
	bool _visible = true;
	int _width = 0;
	int _height = 0;
	SDL_Texture_ptr _tex;
	//x,y,width,height
	SDL_Rect _pos = { 0, 0, 0, 0 };
	int _numFrames = 1;
	int _animationSpeed = 1;
	float _scale = 1;
	RenderParent _parent = canvas;
};

using Sprite_ptr = std::shared_ptr<Sprite>;