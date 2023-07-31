#include <SDL2/SDL_mouse.h>
#include "SDL_ptr.h"

class Cursor
{
public:
	static int Init();
	static void UpdateCursor();
	void setCursor(int num);
private:
	static SDL_Cursor_ptr cursors[20];
	static int currentCursor;
};