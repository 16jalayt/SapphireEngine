#include "sprite.h"

#include <SDL2/SDL_image.h>

#include "Engine/graphics.h"
#include "globals.h"
#include "Engine/utils.h"
#include "Nancy/GUI.h"
#include "Nancy/AVF.h"
#include "Nancy/GUI.h"

Sprite::Sprite(const char* file, int x, int y, RenderParent parent, Scaled_Rect partial, int numFrames, int animationSpeed)
{
	//TODO: upgrade to string?
	_loaded = false;
	_width = 0;
	_height = 0;
	_visible = true;
	_scale = 1;
	_parent = parent;

	//TODO:implement
	_numFrames = numFrames;
	_animationSpeed = animationSpeed;

	//If file name not null or empty
	if (file && strcmp(file, "") != 0)
	{
		//TODO: move to Loader
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
			printf("Unable to open sprite: %s , %s\n", file, IMG_GetError());
		}
	}
	//}
}

Sprite::Sprite(SDL_Texture_ptr texture, int x, int y, RenderParent parent, Scaled_Rect partial, int numFrames, int animationSpeed)
{
	_loaded = false;
	_width = 0;
	_height = 0;
	_visible = true;
	_scale = 1;
	_parent = parent;

	//TODO:implement
	_numFrames = numFrames;
	_animationSpeed = animationSpeed;

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
		printf("Unable to open sprite with texture.\n");
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

bool Sprite::MouseCollision(SDL_Event event)
{
	int x = 0;
	int y = 0;
	if (_parent == window)
	{
		//Select finger or mouse and test range
		x = event.type == SDL_FINGERDOWN ? int(event.tfinger.x * SCREEN_WIDTH) : event.motion.x;
		y = event.type == SDL_FINGERDOWN ? int(event.tfinger.y * SCREEN_HEIGHT) : event.motion.y;
	}
	//canvas
	else
	{
		//Select finger or mouse and test range
		x = event.type == SDL_FINGERDOWN ? int(event.tfinger.x * SCREEN_WIDTH) : event.motion.x - GUI::canvasRect.x;
		y = event.type == SDL_FINGERDOWN ? int(event.tfinger.y * SCREEN_HEIGHT) : event.motion.y - GUI::canvasRect.y;
	}

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

Sprite::~Sprite()
{
	destroy();
}