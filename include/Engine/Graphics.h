#pragma once

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_framerate.h>
#include <SDL2/SDL_ttf.h>

#include "SDL_ptr.h"
#include "SDL_TTF_ptr.h"
#include "Globals.h"

#if !SDL_VERSION_ATLEAST(2,0,17)
#warning This backend requires SDL 2.0.17+ because of SDL_HINT_IME_SHOW_UI. Disabling debug UI.
  #if !defined(NO_IMGUI)
    #define NO_IMGUI
  #endif
#endif

namespace Engine
{
	class SE_API Graphics
	{
	public:
		virtual ~Graphics();
		int init(std::string loadingScreen = NULL);

		std::shared_ptr<SDL_Window> getWindow() const;
		//std::shared_ptr<TTF_Font> getFont() const;
		void loadingscreen(std::string loadingScreen);

		//SDL_Texture_ptr render_text(const char* text, TTF_Font* font, SDL_Color color, SDL_Rect* rect);

		void frameWait();

		//TODO: use extern and out of class
		static std::shared_ptr<SDL_Window> window;
		static std::shared_ptr<SDL_Renderer> renderer;
		SDL_Texture_sptr loadingTex;
		static void Quit();

	private:
		//std::shared_ptr<SDL_Window> window;
		//std::shared_ptr<TTF_Font> _font;
		FPSmanager _fpsm;

		//std::map<std::string, SDL_Surface*> _spriteSheets;
	};
}

using Graphics_ptr = std::unique_ptr<Engine::Graphics>;
