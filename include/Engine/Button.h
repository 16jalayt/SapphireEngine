#pragma once

#include <SDL2/SDL.h>

#include "Sprite.h"
#include "SDL_ptr.h"
#include "Globals.h"

namespace Engine
{
	class SE_API Button : public Sprite
	{
	public:
		Button(int x, int y, int w, int h, const char* file = NULL, RenderParent parent = RenderParent::canvas, Scaled_Rect partial = Scaled_Rect());
		Button(SDL_Rect rect, const char* file = NULL, RenderParent parent = RenderParent::canvas, Scaled_Rect partial = Scaled_Rect());
		Button(Scaled_Rect rect, const char* file = NULL, RenderParent parent = RenderParent::canvas, Scaled_Rect partial = Scaled_Rect());
		virtual ~Button() {};
		void Draw();
		void Event(SDL_Event event);
		std::function<void()> callback;
		std::function<void()> hover_event;
		bool isEnabled();
		void setEnabled(bool enabled);

	private:
		bool _enabled;
	};
}

using Button_ptr = std::shared_ptr<Engine::Button>;