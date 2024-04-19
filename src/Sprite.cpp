#include "Engine/Sprite.h"

#include <SDL2/SDL_image.h>

#include "Engine/Graphics.h"
#include "Engine/Globals.h"
#include "Engine/Utils.h"
#include "Engine/GUI.h"
#include "Engine/Config.h"
#include <loguru.hpp>

using namespace Engine;

Sprite::Sprite(const char* file, int x, int y, RenderParent parent, Scaled_Rect partial)
{
	//TODO: upgrade to string?
	_loaded = false;
	_width = 0;
	_height = 0;
	_visible = true;
	_scale = 1;
	_parent = parent;

	//If file name not null or empty
	if (file && strcmp(file, "") != 0)
	{
		//TODO: move to Loader
		//TODO: upgrade to smart pointer
		//TODO: change all texture loading to IMG_LoadTexture to skip tmpsurf
		SDL_Surface* tmpsurf = IMG_Load(file);
		if (tmpsurf)
		{
			if (partial != Scaled_Rect())
			{
				_srcSpecified = true;
				_src = ScaledRect_to_SDLRect(partial);

				//Need to tweak for some reason
				//to remove green border
				_src.x = _src.x + 1;
				_src.y = _src.y + 1;
				_src.w = _src.w - 1;
				_src.h = _src.h - 1;

				_width = _src.w;
				_height = _src.h;
			}
			else
			{
				_width = tmpsurf->w;
				_height = tmpsurf->h;
			}

			_pos = ScaleRect(x, y, _width, _height);
			_scale = Config::globalScale;

			_tex = SDL_Texture_ptr(SDL_CreateTextureFromSurface(Graphics::renderer.get(), tmpsurf));
			SDL_FreeSurface(tmpsurf);
			_loaded = true;
		}
		else
		{
			LOG_F(ERROR, "Unable to open sprite: %s , %s\n", file, IMG_GetError());
		}
	}
}

Sprite::Sprite(SDL_Texture_ptr texture, int x, int y, RenderParent parent, Scaled_Rect partial)
{
	_loaded = false;
	_width = 0;
	_height = 0;
	_visible = true;
	_scale = 1;
	_parent = parent;

	if (texture)
	{
		if (partial != Scaled_Rect())
		{
			_srcSpecified = true;
			_src = ScaledRect_to_SDLRect(partial);

			//Need to tweak for some reason
			//to remove green border
			_src.x = _src.x + 1;
			_src.y = _src.y + 1;
			_src.w = _src.w - 1;
			_src.h = _src.h - 1;

			_width = _src.w;
			_height = _src.h;
		}
		else
		{
			SDL_QueryTexture(texture.get(), NULL, NULL, &_width, &_height);
		}

		_pos = ScaleRect(x, y, _width, _height);
		_scale = Config::globalScale;

		_tex = std::move(texture);
		_loaded = true;
	}
	else
	{
		LOG_F(ERROR, "Unable to open sprite with texture.\n");
	}
}

Sprite::Sprite(SDL_Texture* texture, int x, int y, RenderParent parent, Scaled_Rect partial)
{
	_loaded = false;
	_width = 0;
	_height = 0;
	_visible = true;
	_scale = 1;
	_parent = parent;

	if (texture)
	{
		if (partial != Scaled_Rect())
		{
			_srcSpecified = true;
			_src = ScaledRect_to_SDLRect(partial);

			//Need to tweak for some reason
			//to remove green border
			_src.x = _src.x + 1;
			_src.y = _src.y + 1;
			_src.w = _src.w - 1;
			_src.h = _src.h - 1;

			_width = _src.w;
			_height = _src.h;
		}
		else
		{
			SDL_QueryTexture(texture, NULL, NULL, &_width, &_height);
		}

		_pos = ScaleRect(x, y, _width, _height);
		_scale = Config::globalScale;
		_center = { _width / 2, _height / 2 };

		_tex = SDL_Texture_ptr(texture);
		_loaded = true;
	}
	else
	{
		LOG_F(ERROR, "Unable to open sprite with texture.\n");
	}
}

void Sprite::Draw()
{
	if (_loaded && _visible)
	{
		if (_parent == RenderParent::window)
		{
			SDL_RenderCopyEx(Graphics::renderer.get(), _tex.get(), NULL, &_pos, rotation, &_center, flipped);
		}
		else//RenderParent::canvas
		{
			//SDL_Rect test = ScaleRect(44, 394, 300, 155);
			SDL_SetRenderTarget(Graphics::renderer.get(), GUI::canvas.get());
			//SDL_RenderClear(Graphics::renderer.get());

			if (_isMask)
			{
				SDL_SetTextureBlendMode(_tex.get(), SDL_BLENDMODE_MUL);
				//max 255
				//SDL_SetTextureAlphaMod(_tex.get(), 125);
			}

			if (_srcSpecified)
				SDL_RenderCopyEx(Graphics::renderer.get(), _tex.get(), &_src, &_pos, rotation, &_center, flipped);
			else
				SDL_RenderCopyEx(Graphics::renderer.get(), _tex.get(), NULL, &_pos, rotation, &_center, flipped);
			//Detach the texture
			SDL_SetRenderTarget(Graphics::renderer.get(), NULL);
		}
	}
}

bool Sprite::isloaded()
{
	return _loaded;
}
bool Sprite::isVisible()
{
	return _visible;
}

void Sprite::destroy()
{
	_loaded = false;
}

void Sprite::Event(SDL_Event event)
{
	switch (event.type)
	{
	case SDL_MOUSEBUTTONDOWN:
	case SDL_FINGERDOWN:
		if (MouseCollision(event) && callback)
		{
			//TODO: skeliton key crashes with empty callback
			callback();
		}
		break;
	case SDL_MOUSEMOTION:
		if (MouseCollision(event) && hover_event)
		{
			hover_event();
		}
		break;
	}
}

bool Sprite::MouseCollision(SDL_Event event)
{
	//Select finger or mouse and test range
	int x = event.type == SDL_FINGERDOWN ? int(event.tfinger.x * Config::referenceWidth) : event.motion.x;
	int y = event.type == SDL_FINGERDOWN ? int(event.tfinger.y * Config::referenceHeight) : event.motion.y;

	//subtract off the gui offset
	if (_parent == canvas)
	{
		x = x - GUI::canvasRect.x;
		y = y - GUI::canvasRect.y;
	}

	//LOG_F(ERROR, "Touch at: %d,%d\n", x, y);

	//Mouse is left of the button
	if (x < _pos.x)
	{
		return false;
	}
	//Mouse is right of the button
	if (x > _pos.x + _pos.w)
	{
		return false;
	}
	//Mouse above the button
	if (y < _pos.y)
	{
		return false;
	}
	//Mouse below the button
	if (y > _pos.y + _pos.h)
	{
		return false;
	}

	return true;
}

SDL_Rect Sprite::getPos()
{
	return _pos;
}

void Sprite::visible(bool visible)
{
	_visible = visible;
}

//Sets scale RELETIVE TO CURRENT
void Sprite::setScale(float scale)
{
	//TODO: _scale not accurate due to ScaleRect
	_scale = scale;
	//TODO:reletive to original scale?
	//_pos = { (int)(_pos.x * _scale) ,(int)(_pos.y * _scale), (int)(_pos.w * _scale), (int)(_pos.h * _scale) };
	_pos = ScaleRect(_pos, scale);
}

float Sprite::getScale()
{
	return _scale;
}

int Sprite::getWidth()
{
	return _width;
}

int Sprite::getHeight()
{
	return _height;
}
void Sprite::setPos(SDL_Rect pos)
{
	_pos = pos;
}

void Sprite::isMask(bool mask)
{
	_isMask = mask;
}

void Engine::Sprite::setColorMod(RGB modVal)
{
	setColorMod(modVal.r, modVal.g, modVal.b);
}

void Engine::Sprite::setColorMod(int r, int g, int b)
{
	SDL_SetTextureColorMod(_tex.get(), r, g, b);
}