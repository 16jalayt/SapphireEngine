#include <SDL2/SDL_mouse.h>
#include "SDL_ptr.h"
#include <string>
#include "Globals.h"

class SE_API Cursor
{
public:
	static int Init();
	static void DrawCursor();
	static void setCursor(int num);
	static void resetCursor();

	static bool CursorChanged;

private:
	static SDL_Cursor_ptr loadCursorFromSheet(std::string filename, SDL_Rect rect);

	static SDL_Surface_ptr spriteSheet;
	static std::string spriteSheetPath;
	static SDL_Cursor_ptr cursors[30];
	static int currentCursor;
	static int baseCursor;
};