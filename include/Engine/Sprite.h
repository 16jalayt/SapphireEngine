#pragma once

#include <SDL2/SDL.h>
#include <string>
#include <functional>

#include "SDL_ptr.h"
#include "Utils.h"
#include "Globals.h"

enum RenderParent { window, canvas };

namespace Engine
{
	class SE_API Sprite
	{
	public:
		Sprite() = default;
		Sprite(const char* fileName, int x = 0, int y = 0, RenderParent parent = RenderParent::canvas, Scaled_Rect partial = Scaled_Rect());
		Sprite(SDL_Texture_ptr texture, int x = 0, int y = 0, RenderParent parent = RenderParent::canvas, Scaled_Rect partial = Scaled_Rect());
		//Sprite(SDL_Renderer_sptr renderer, const char* file, Scaled_Rect rect, int numFrames = 1, int animationSpeed = 0);
		virtual ~Sprite() {};
		void Draw();
		void destroy();
		bool isloaded();
		bool isVisible();
		void Event(SDL_Event event);
		bool MouseCollision(SDL_Event event);
		SDL_Rect getPos();
		void visible(bool visible);
		void setScale(float scale);
		float getScale();
		int getWidth();
		int getHeight();
		void setPos(SDL_Rect pos);
		void isMask(bool mask);

		std::function<void()> callback;
		std::function<void()> hover_event;

	protected:
		bool _loaded = false;
		bool _visible = true;
		int _width = 0;
		int _height = 0;
		SDL_Texture_ptr _tex;
		//x,y,width,height
		SDL_Rect _pos = { 0, 0, 0, 0 };
		SDL_Rect _src = { 0, 0, 0, 0 };
		bool _srcSpecified = false;
		float _scale = 1;
		RenderParent _parent = canvas;
		bool _isMask = false;
	};
}

using Sprite_ptr = std::shared_ptr<Engine::Sprite>;