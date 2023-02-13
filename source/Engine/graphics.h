#pragma once

#include <map>  //NOT available in devkitpro
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_framerate.h>
#include <SDL2/SDL_ttf.h>
#include "SDL_ptr.h"
#include "SDL_TTF_ptr.h"

class Graphics
{
public:
	int init(SDL_Texture_sptr loading_tex);

	std::shared_ptr<SDL_Window> getWindow() const;
	//std::shared_ptr<TTF_Font> getFont() const;

	//SDL_Texture_ptr render_text(const char* text, TTF_Font* font, SDL_Color color, SDL_Rect* rect);

	void frameWait();

	//TODO: use extern and out of class
	static std::shared_ptr<SDL_Window> window;
	static std::shared_ptr<SDL_Renderer> renderer;
	static void Quit();

private:
	//std::shared_ptr<SDL_Window> window;
	//std::shared_ptr<TTF_Font> _font;
	FPSmanager _fpsm;

	//std::map<std::string, SDL_Surface*> _spriteSheets;
};

using Graphics_ptr = std::unique_ptr<Graphics>;
