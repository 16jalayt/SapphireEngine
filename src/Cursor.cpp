#include "Cursor.h"
#include "loguru/loguru.hpp"
#include <SDL2/SDL_image.h>
#include "utils.h"

SDL_Cursor_ptr Cursor::cursors[30];
int Cursor::currentCursor = 0;
int Cursor::baseCursor = 0;
bool Cursor::CursorChanged = false;
SDL_Surface_ptr Cursor::spriteSheet;
std::string Cursor::spriteSheetPath;

SDL_Cursor_ptr Cursor::loadCursorFromSheet(std::string filename, SDL_Rect rect)
{
	//Cache sprite sheet to improve performance
	if (spriteSheetPath != filename)
	{
		spriteSheetPath = filename;
		spriteSheet = SDL_Surface_ptr(IMG_Load(filename.c_str()));
	}
	if (!spriteSheet)
	{
		LOG_F(ERROR, "Unable to load cursor Sprite Sheet: %s.\n", filename.c_str());
		return NULL;
	}

	SDL_Surface* sprite = SDL_CreateRGBSurface(spriteSheet->flags, rect.w, rect.h, spriteSheet->format->BitsPerPixel, spriteSheet->format->Rmask, spriteSheet->format->Gmask, spriteSheet->format->Bmask, spriteSheet->format->Amask);
	//SDL_Surface* sprite = IMG_Load(filename.c_str());
	if (!sprite)
	{
		LOG_F(ERROR, "Unable to load cursor.\n");
		return NULL;
	}

	SDL_BlitSurface(spriteSheet.get(), &rect, sprite, NULL);

	//SDL_FreeSurface(spriteSheet.get());

	//TODO: returns null on switch
	SDL_Cursor_ptr cursor = SDL_Cursor_ptr(SDL_CreateColorCursor(sprite, 10, 10));
	if (!cursor)
	{
		//Does not work on switch so suppress logging
#ifndef __SWITCH__
		LOG_F(ERROR, "Unable to create cursor.\n");
#endif
		return NULL;
	}

	return cursor;
}

int Cursor::Init()
{
	//TODO: make default placeholder, bug with random starting cursor
	//TODO: split out game specific

	std::string sheetName = "DataFiles/CIFTREE/OBJECT0.png";

	//Default Magnifying
	cursors[0] = std::move(loadCursorFromSheet(sheetName, SDL_Rect{ 506, 3, 30, 40 }));
	//Highlight Magnifying
	cursors[1] = std::move(loadCursorFromSheet(sheetName, SDL_Rect{ 537, 3, 30, 40 }));
	//Left Arrow
	cursors[6] = std::move(loadCursorFromSheet(sheetName, SDL_Rect{ 568, 167, 30, 40 }));
	//Right Arrow
	cursors[7] = std::move(loadCursorFromSheet(sheetName, SDL_Rect{ 568, 208, 30, 40 }));
	//Back Arrow
	cursors[9] = std::move(loadCursorFromSheet(sheetName, SDL_Rect{ 506, 290, 30, 40 }));
	//Uturn Arrow
	cursors[14] = std::move(loadCursorFromSheet(sheetName, SDL_Rect{ 568, 85, 30, 40 }));
	//Forward Arrow
	cursors[15] = std::move(loadCursorFromSheet(sheetName, SDL_Rect{ 475, 290, 30, 40 }));
	//Back Arrow (duplicate)
	cursors[16] = std::move(loadCursorFromSheet(sheetName, SDL_Rect{ 506, 290, 30, 40 }));
	//Left Arrow (duplicate)
	cursors[22] = std::move(loadCursorFromSheet(sheetName, SDL_Rect{ 568, 167, 30, 40 }));
	//Right Arrow (duplicate)
	cursors[23] = std::move(loadCursorFromSheet(sheetName, SDL_Rect{ 568, 208, 30, 40 }));

	return 0;
}

void Cursor::DrawCursor()
{
	if (!CursorChanged)
		currentCursor = baseCursor;

	if (!cursors[currentCursor])
	{
		//Does not work on switch so suppress logging
#ifndef __SWITCH__
		LOG_F(ERROR, "Invalid cursor id: %d\n", currentCursor);
#endif
		currentCursor = baseCursor;
	}

	SDL_SetCursor(cursors[currentCursor].get());
}

void Cursor::setCursor(int num)
{
	currentCursor = num;
	CursorChanged = true;
}

void Cursor::resetCursor()
{
	currentCursor = baseCursor;
}