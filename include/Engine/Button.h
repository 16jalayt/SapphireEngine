#pragma once

#include <SDL2/SDL.h>

#include "sprite.h"
#include "SDL_ptr.h"
#include "Globals.h"

class SE_API Button : public Sprite
{
public:
	Button(int x, int y, int w, int h, const char* file = NULL, RenderParent parent = RenderParent::canvas, bool enabled = true);
	Button(SDL_Rect rect, const char* file = NULL, RenderParent parent = RenderParent::canvas, bool enabled = true);
	Button(Scaled_Rect rect, const char* file = NULL, RenderParent parent = RenderParent::canvas, bool enabled = true);
	virtual ~Button() {};
	void Draw();

private:
	//TODO: make enabled in click handler
	bool _enabled;
};

//TODO: need deleter?
using Button_ptr = std::shared_ptr<Button>;