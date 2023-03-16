#ifdef __SWITCH__
#include <switch.h>
#endif

#include "graphics.h"

#include <SDL2/SDL_image.h>

#include "globals.h"
#include "utils.h"
#include <SDL2/SDL_mixer.h>
#include "AvP_AudioStreaming.h"
#include "audio.h"
#include <iostream>

//Graphics class
//Holds all information dealing with graphics for the game
std::shared_ptr<SDL_Window> Graphics::window;
std::shared_ptr<SDL_Renderer> Graphics::renderer;

int Graphics::init(SDL_Texture_sptr loading_tex)
{
#ifdef __SWITCH__
	romfsInit();
	chdir("romfs:/");
#endif

	//Only init video to display load screen faster.
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		fatalError("%s: SDL could not initialize video! SDL Error: %s", __func__, SDL_GetError());
		return -1;
	}

	//if (!SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl"))
	if (!SDL_SetHintWithPriority(SDL_HINT_RENDER_DRIVER, "opengl", SDL_HINT_OVERRIDE))
	{
		fatalError("%s: Warning: backend fail: %s", __func__, SDL_GetError());
	}

	//Attempt to turn on for window creation then off for texture creation
	//Doesn't seem to have an effect
	//On causes black lines around textures
	/*if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"))
	{
		fatalError("%s: Warning: Linear texture filtering not enabled: %s", __func__, SDL_GetError());
	}*/

	int flags = 0;
#ifdef __SWITCH__
	flags = SDL_WINDOW_SHOWN;
#else
	if (fullscreen)
		flags = SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP;
	else
		//full desk ignores real width
		flags = SDL_WINDOW_SHOWN;

#endif
	window = make_SDL_Window_s(SDL_CreateWindow("Sapphire Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, REAL_WIDTH, REAL_HEIGHT, flags));
	if (!window)
	{
		fatalError("%s: SDL could not create window! SDL Error: %s", __func__, SDL_GetError());
		return -1;
	}
	//SDL_SetWindowResizable(window.get(), SDL_TRUE);

	renderer = make_SDL_Renderer_s(SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
	if (!renderer)
	{
		fatalError("%s: SDL could not create renderer! SDL Error: %s", __func__, SDL_GetError());
		return -1;
	}

	// dump renderer info
	/*SDL_RendererInfo info;
	SDL_GetRendererInfo(renderer.get(), &info);
	std::cout << "Renderer name: " << info.name << '\n';
	std::cout << "Texture formats: " << '\n';
	for (Uint32 i = 0; i < info.num_texture_formats; i++)
	{
		std::cout << SDL_GetPixelFormatName(info.texture_formats[i]) << '\n';
	}*/

	/*int i, display_mode_count;
	SDL_DisplayMode mode;
	Uint32 f;

	SDL_Log("SDL_GetNumVideoDisplays(): %i", SDL_GetNumVideoDisplays());

	display_mode_count = SDL_GetNumDisplayModes(0);
	if (display_mode_count < 1) {
		SDL_Log("SDL_GetNumDisplayModes failed: %s", SDL_GetError());
		return 1;
	}
	SDL_Log("SDL_GetNumDisplayModes: %i", display_mode_count);

	for (i = 0; i < display_mode_count; ++i) {
		if (SDL_GetDisplayMode(0, i, &mode) != 0) {
			SDL_Log("SDL_GetDisplayMode failed: %s", SDL_GetError());
			return 1;
		}
		f = mode.format;

		SDL_Log("Mode %i\tbpp %i\t%s\t%i x %i",
			i, SDL_BITSPERPIXEL(f),
			SDL_GetPixelFormatName(f),
			mode.w, mode.h);
	}
	*/

	//Real height/width seems to be ignored. Giving 1080. works in gl
	/*SDL_DisplayMode current;
	SDL_GetRendererOutputSize(renderer, &current.w, &current.h);
	SDL_GetCurrentDisplayMode(0, &current);
	printf("Display #%d: current display mode is %dx%dpx @ %dhz.\n", 0, current.w, current.h, current.refresh_rate);*/

	loadingscreen(renderer, loading_tex);

	// 800x600 render and 1080 target gets 1.8 for scale
		//SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
		//float renderScaleX;
		//SDL_RenderGetScale(renderer, &renderScaleX, NULL);
		//printf("%f\n", renderScaleX);
	//SDL_RenderSetIntegerScale(renderer, SDL_TRUE);
	//TODO: Check other hints like vsync
	// linear interpolates so bad for pixel work
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer.get(), SCREEN_WIDTH, SCREEN_HEIGHT);
	//SDL_RenderSetIntegerScale(renderer, SDL_TRUE);

	//Init everything else.
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		fatalError("%s: SDL could not initialize everything! SDL Error: %s", __func__, SDL_GetError());
		return -1;
	}

	SDL_initFramerate(&_fpsm);
	//Uint32 time_passed = 0;
	//defaults to 30
	SDL_setFramerate(&_fpsm, 60);

	if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) < 0)
	{
		fatalError("%s: SDL could not initialize image formats! SDL Error: %s", __func__, SDL_GetError());
		return -1;
	}
	/*if (TTF_Init() < 0)
	{
		fatalError("%s: SDL could not initialize font! SDL Error: %s", __func__, SDL_GetError());
		return -1;
	}

	// load font from romfs
	//_font = TTF_OpenFont("data/LeroyLetteringLightBeta01.ttf", 36);
	_font = make_TTF_Font_s(TTF_OpenFont("data/LeroyLetteringLightBeta01.ttf", 36));
	if (!_font)
	{
		fatalError("%s: Failed to load font: %s", __func__, SDL_GetError());
		return -1;
	}*/

	//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

	Audio::Init();

	return 0;
}

std::shared_ptr<SDL_Window> Graphics::getWindow() const
{
	return this->window;
}

/*std::shared_ptr<TTF_Font> Graphics::getFont() const
{
	return this->_font;
}*/

void Graphics::frameWait()
{
	//limit frame rate
	SDL_framerateDelay(&_fpsm);
	//time_passed = SDL_framerateDelay(&fpsm);
}
void Graphics::Quit()
{
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
}
/*
SDL_Texture_ptr Graphics::render_text(const char* text, TTF_Font* font, SDL_Color color, SDL_Rect* rect)
{
	SDL_Surface_ptr surface;
	SDL_Texture_ptr texture;

	surface = SDL_Surface_ptr(TTF_RenderText_Solid(font, text, color));
	texture = SDL_Texture_ptr(SDL_CreateTextureFromSurface(Graphics::renderer.get(), surface.get()));
	rect->w = surface->w;
	rect->h = surface->h;

	return texture;
}*/