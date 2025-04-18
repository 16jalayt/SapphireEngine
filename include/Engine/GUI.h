#pragma once
#include <fstream>

#include "SDL_ptr.h"
#include "Button.h"
#include "Graphics.h"
#include "Sprite.h"
#include <vector>
#include "Globals.h"
#include "Utils.h"
#include "Engine/IMGUIInclude.h"

typedef struct GUI_Rect
{
	SDL_Rect rect;
	int r, g, b;
} GUI_Rect;



namespace Engine
{
	class SE_API GUI
	{
	public:
		GUI();
		virtual ~GUI();
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

#if !defined(NO_IMGUI)
		ImGuiContext* imCtx;
#endif

	protected:
		void ToggleButton(const char* str_id, bool* v);
		void ToggleButtonV2(const char* str_id, bool* v);
		void ShowHelpMarker(const char* desc);
	};
}

using GUIEngine_ptr = std::shared_ptr<Engine::GUI>;
