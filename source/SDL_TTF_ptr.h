#ifndef SDL_TTF_PTR_H
#define SDL_TTF_PTR_H

#include <SDL2/SDL_ttf.h>
#include <memory>

namespace SDL_TTF_Deleter
{
	struct SDL_TTF_Font_Deleter
	{
		void operator()(TTF_Font* font)
		{
			TTF_CloseFont(font);
		}
	};
}

using namespace SDL_TTF_Deleter;

using SDL_TTF_Font_ptr = std::unique_ptr<TTF_Font, SDL_TTF_Font_Deleter>;

template <class... Args>
std::shared_ptr<TTF_Font> make_TTF_Font_s(Args&&... args)
{
	return std::shared_ptr<TTF_Font>(std::forward<Args>(args)...,
		SDL_TTF_Font_Deleter{});
}

#endif // !SDL_TTF_PTR_H
