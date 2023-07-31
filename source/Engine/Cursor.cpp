#include "Cursor.h"
#include <loguru.hpp>
#include <SDL2/SDL_image.h>

SDL_Cursor_ptr Cursor::cursors[20];
int Cursor::currentCursor = 0;

int Cursor::Init()
{
	//TODO: split up for bettor error handling
	SDL_Cursor_ptr cursor = SDL_Cursor_ptr(SDL_CreateColorCursor(IMG_Load("DataFiles/cur0.png"), 10, 10));
	if (!cursor)
	{
		LOG_F(ERROR, "Unable to load cursor.\n");
		return -1;
	}
	cursors[0] = std::move(cursor);
	return 0;
}

void Cursor::UpdateCursor()
{
	SDL_SetCursor(cursors[currentCursor].get());
}

void Cursor::setCursor(int num)
{
}