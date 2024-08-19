#include "Engine/Button.h"
#include "Engine/Utils.h"
#include "Engine/Graphics.h"
#include "Engine/GUI.h"
#include "Engine/Config.h"
#include <cstring>

using namespace Engine;

Button::Button(int x, int y, int w, int h, const char* file, RenderParent parent, bool enabled, Scaled_Rect partial) :Sprite(file, x, y, parent, partial)
{
	_enabled = enabled;

	//If file name is empty. Must be hot spot
	if (strcmp(file, "") == 0)
	{
		_pos = ScaleRect(x, y, w, h);
		_width = w;
		_height = h;
	}
}

Button::Button(SDL_Rect rect, const char* file, RenderParent parent, bool enabled, Scaled_Rect partial) :Sprite(file, rect.x, rect.y, parent, partial)
{
	_enabled = enabled;

	//If file name is empty. Must be hot spot
	if (strcmp(file, "") == 0)
	{
		_pos = ScaleRect(rect);
		_width = _pos.w;
		_height = _pos.h;
	}
}

Button::Button(Scaled_Rect rect, const char* file, RenderParent parent, bool enabled, Scaled_Rect partial) :Sprite(file, rect.x, rect.y, parent, partial)
{
	_enabled = enabled;

	//If file name is empty. Must be hot spot
	if (strcmp(file, "") == 0)
	{
		//convert rect type and scale
		_pos = ScaleRect(ScaledRect_to_SDLRect(rect));
		_width = _pos.w;
		_height = _pos.h;
	}
}

void Button::Event(SDL_Event event)
{
	if (_visible && _enabled)
		Sprite::Event(event);
}

void Button::Draw()
{
	Sprite::Draw();

	//TODO: make proporty to not have hotzone
	if (_visible && Config::debugHot && _enabled)
	{
		if (_parent == RenderParent::window)
		{
			//SDL_SetRenderDrawColor(renderer.get(), 135, 135, 135, 0xFF);
			SDL_SetRenderDrawColor(Graphics::renderer.get(), 135, 0, 0, 0xFF);
			SDL_RenderFillRect(Graphics::renderer.get(), &_pos);
		}
		else//RenderParent::canvas
		{
			SDL_SetRenderTarget(Graphics::renderer.get(), GUI::canvas.get());
			//TODO:different colors for different hot types
			SDL_SetRenderDrawBlendMode(Graphics::renderer.get(), SDL_BLENDMODE_BLEND);
			SDL_SetRenderDrawColor(Graphics::renderer.get(), 0, 0, 135, 0x50);
			SDL_RenderFillRect(Graphics::renderer.get(), &_pos);
			SDL_SetRenderDrawBlendMode(Graphics::renderer.get(), SDL_BLENDMODE_NONE);

			//Detach the texture
			SDL_SetRenderTarget(Graphics::renderer.get(), NULL);
		}
	}
}