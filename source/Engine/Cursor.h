#include <SDL2/SDL_mouse.h>
#include "SDL_ptr.h"
#include <string>

class Cursor
{
public:
	static int Init();
	static void DrawCursor();
	static void setCursor(int num);
	static void resetCursor();

	static bool CursorChanged;

private:
	static SDL_Cursor_ptr cursors[20];
	static int currentCursor;
	static int baseCursor;
};