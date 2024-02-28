#include "Engine/Sprite.h"

#include <SDL2/SDL_image.h>

#include "Engine/Graphics.h"
#include "Engine/Globals.h"
#include "Engine/Utils.h"
#include "Engine/GUI.h"
#include <loguru/loguru.hpp>
#include "Engine/Cursor.h"

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
			_scale = GlobalScale;

			_tex = SDL_Texture_ptr(SDL_CreateTextureFromSurface(Graphics::renderer.get(), tmpsurf));
			SDL_FreeSurface(tmpsurf);
			_loaded = true;
		}
		else
		{
			LOG_F(ERROR, "Unable to open sprite: %s , %s\n", file, IMG_GetError());
		}
	}
	//}
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
		_scale = GlobalScale;

		_tex = std::move(texture);
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
			SDL_RenderCopy(Graphics::renderer.get(), _tex.get(), NULL, &_pos);
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
				//SDL_RenderCopy(Graphics::renderer.get(), _tex.get(), &test, &_pos);
				SDL_RenderCopy(Graphics::renderer.get(), _tex.get(), &_src, &_pos);
			else
				SDL_RenderCopy(Graphics::renderer.get(), _tex.get(), NULL, &_pos);
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
	//HoverCheck(event);

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
		HoverCheck(event);
		break;
	}
}

bool Sprite::MouseCollision(SDL_Event event)
{
	//Select finger or mouse and test range
	int x = event.type == SDL_FINGERDOWN ? int(event.tfinger.x * SCREEN_WIDTH) : event.motion.x;
	int y = event.type == SDL_FINGERDOWN ? int(event.tfinger.y * SCREEN_HEIGHT) : event.motion.y;

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

void Sprite::HoverCheck(SDL_Event event)
{
	if (MouseCollision(event))
		Cursor::setCursor(hoverCursor);
	//else
		//Cursor::resetCursor();
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