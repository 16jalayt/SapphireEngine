#pragma once
#include <fstream>

#include "Engine/Scene.h"
#include "Engine/SDL_ptr.h"

typedef struct GUI_Rect
{
	SDL_Rect rect;
	int r, g, b;
} GUI_Rect;

//using GUI_Rect_ptr = std::shared_ptr<GUI_Rect>;

class GUI
{
public:
	GUI();
	~GUI();
	std::vector<Sprite_ptr> statics;
	std::vector<Button_ptr> buttons;
	std::vector<GUI_Rect> rects;

	static std::shared_ptr<SDL_Texture> canvas;
	static SDL_Rect canvasRect;

	void Draw();
	void EventProc(SDL_Event event);
	void AddSprite(Sprite_ptr sprite);
	void AddButton(Button_ptr button);
	void AddRect(GUI_Rect rect);
private:
	void drawCheatSheet();
	void ToggleButton(const char* str_id, bool* v);
	void ToggleButtonV2(const char* str_id, bool* v);
	void ShowHelpMarker(const char* desc);

	bool cheatSheetOpen = true;
};

using GUI_ptr = std::shared_ptr<GUI>;
