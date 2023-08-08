#include "Cursor.h"
#include <loguru.hpp>
#include <SDL2/SDL_image.h>
#include "utils.h"

SDL_Cursor_ptr Cursor::cursors[20];
int Cursor::currentCursor = 0;
int Cursor::baseCursor = 0;
bool Cursor::CursorChanged = false;

SDL_Cursor_ptr loadCursorFromSheet(std::string filename, SDL_Rect rect)
{
	//TODO: make genaric as sheet, and make array version that doesn't dispose of sheet
	SDL_Surface* spriteSheet = IMG_Load(filename.c_str());
	if (!spriteSheet)
	{
		LOG_F(ERROR, "Unable to load cursor Sprite Sheet.\n");
		return NULL;
	}

	SDL_Surface* sprite = SDL_CreateRGBSurface(spriteSheet->flags, rect.w, rect.h, spriteSheet->format->BitsPerPixel, spriteSheet->format->Rmask, spriteSheet->format->Gmask, spriteSheet->format->Bmask, spriteSheet->format->Amask);
	//SDL_Surface* sprite = IMG_Load(filename.c_str());
	if (!sprite)
	{
		LOG_F(ERROR, "Unable to load cursor.\n");
		return NULL;
	}

	SDL_BlitSurface(spriteSheet, &rect, sprite, NULL);

	SDL_FreeSurface(spriteSheet);

	SDL_Cursor_ptr cursor = SDL_Cursor_ptr(SDL_CreateColorCursor(sprite, 10, 10));

	return cursor;
}

int Cursor::Init()
{
	//TODO: make default placeholder

	//Default Magnifying
	cursors[0] = std::move(loadCursorFromSheet("DataFiles/CIFTREE/OBJECT0.png", SDL_Rect{ 506, 3, 30, 40 }));
	//Highlight Magnifying
	cursors[1] = std::move(loadCursorFromSheet("DataFiles/CIFTREE/OBJECT0.png", SDL_Rect{ 537, 3, 30, 40 }));
	//Left Arrow
	cursors[6] = std::move(loadCursorFromSheet("DataFiles/CIFTREE/OBJECT0.png", SDL_Rect{ 568, 167, 30, 40 }));
	//Uturn Arrow
	cursors[14] = std::move(loadCursorFromSheet("DataFiles/CIFTREE/OBJECT0.png", SDL_Rect{ 568, 85, 30, 40 }));
	//Forward Arrow
	cursors[15] = std::move(loadCursorFromSheet("DataFiles/CIFTREE/OBJECT0.png", SDL_Rect{ 475, 290, 30, 40 }));
	return 0;
}

void Cursor::DrawCursor()
{
	if (!CursorChanged)
		currentCursor = baseCursor;
	SDL_SetCursor(cursors[currentCursor].get());
	//CursorChanged = false;
}

void Cursor::setCursor(int num)
{
	currentCursor = num;
	CursorChanged = true;
	//UpdateCursor();
}

//TODO: not needed
void Cursor::resetCursor()
{
	currentCursor = baseCursor;
	//UpdateCursor();
}