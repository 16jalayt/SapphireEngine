#define SDL_MAIN_HANDLED
#include <time.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef __SWITCH__
#include <switch.h>
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
//#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_framerate.h>
#include <SDL2/SDL2_rotozoom.h>
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer.h"

#include "globals.h"
#include "Engine/utils.h"
#include "Engine/graphics.h"
#include "Engine/sprite.h"
#include "Engine/SDL_ptr.h"
#include "Engine/Button.h"
#include "Engine/AvP_BinkPlayback.h"
#include "Engine/Scene.h"
#include "Nancy/Loader.h"
#include "Nancy/GUI.h"

//TODO: config manager

//Fixes openal error when closing console
#ifdef _WIN32
#include <windows.h>
BOOL WINAPI ConsoleHandlerRoutine(DWORD dwCtrlType) {
	if (CTRL_CLOSE_EVENT == dwCtrlType) {
		printf("exiting");
		//hangs?
		//quit();
		return true;
	}

	return false;
}
#endif

#include "imgui_internal.h"

void ToggleButton(const char* str_id, bool* v)
{
	ImVec2 p = ImGui::GetCursorScreenPos();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	float height = ImGui::GetFrameHeight();
	float width = height * 1.55f;
	float radius = height * 0.50f;

	ImGui::InvisibleButton(str_id, ImVec2(width, height));
	if (ImGui::IsItemClicked())
		*v = !*v;

	float t = *v ? 1.0f : 0.0f;

	ImGuiContext& g = *GImGui;
	float ANIM_SPEED = 0.08f;
	if (g.LastActiveId == g.CurrentWindow->GetID(str_id))// && g.LastActiveIdTimer < ANIM_SPEED)
	{
		float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
		t = *v ? (t_anim) : (1.0f - t_anim);
	}

	ImU32 col_bg;
	if (ImGui::IsItemHovered())
		col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.78f, 0.78f, 0.78f, 1.0f), ImVec4(0.64f, 0.83f, 0.34f, 1.0f), t));
	else
		col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.85f, 0.85f, 0.85f, 1.0f), ImVec4(0.56f, 0.83f, 0.26f, 1.0f), t));

	draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
	draw_list->AddCircleFilled(ImVec2(p.x + radius + t * (width - radius * 2.0f), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
}

void ToggleButtonV2(const char* str_id, bool* v)
{
	ImVec4* colors = ImGui::GetStyle().Colors;
	ImVec2 p = ImGui::GetCursorScreenPos();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	float height = ImGui::GetFrameHeight();
	float width = height * 1.55f;
	float radius = height * 0.50f;

	ImGui::InvisibleButton(str_id, ImVec2(width, height));
	if (ImGui::IsItemClicked()) *v = !*v;
	ImGuiContext& gg = *GImGui;
	float ANIM_SPEED = 0.085f;
	if (gg.LastActiveId == gg.CurrentWindow->GetID(str_id))// && g.LastActiveIdTimer < ANIM_SPEED)
		float t_anim = ImSaturate(gg.LastActiveIdTimer / ANIM_SPEED);
	if (ImGui::IsItemHovered())
		draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), ImGui::GetColorU32(*v ? colors[ImGuiCol_ButtonActive] : ImVec4(0.78f, 0.78f, 0.78f, 1.0f)), height * 0.5f);
	else
		draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), ImGui::GetColorU32(*v ? colors[ImGuiCol_Button] : ImVec4(0.85f, 0.85f, 0.85f, 1.0f)), height * 0.50f);
	draw_list->AddCircleFilled(ImVec2(p.x + radius + (*v ? 1 : 0) * (width - radius * 2.0f), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
}

void ShowHelpMarker(const char* desc) {
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered()) {
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(450.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

int main(int argc, char** argv)
{
	//Fixes openal error when closing console
#ifdef _WIN32
	if (SetConsoleCtrlHandler(ConsoleHandlerRoutine, TRUE) == FALSE) {
		// Cannot register your handler? Check GetLastError()
		printf("Error registering windows exit handler.");
	}
#endif

	Graphics_ptr graphics = Graphics_ptr(new Graphics());

	//TODO: dont need anymore?
	SDL_Texture_sptr loading_tex;

	// initial subsystem. If error, just exit. Error already printed.
	if (graphics->init(loading_tex) < 0)
		quit();

	//Test loading screen
	//SDL_Delay(2000);
	initControls();

	//Sprite_ptr frame = std::make_shared<Sprite>("DataFiles/CIFTREE/frame.png", 0, 0, RenderParent::window);
	//frame->setPos(SDL_Rect{ 0,0,(int)(frame->getWidth() * GlobalScale), (int)(frame->getHeight() * GlobalScale) });
	// Scene::ChangeScene(Scene_ptr(new Scene("Video/TUN_KenZR.png")));

	// TODO: smart pointers (in sdl_ptr.h?)

	Loader::Boot();

	// render text as texture
	//SDL_Rect helloworld_rect = { 0, SCREEN_HEIGHT - 36, 0, 0 };
	//SDL_Texture_ptr helloworld_tex = graphics->render_text("Hello, world!", graphics->getFont().get(), { 255, 255, 255, 0 }, &helloworld_rect);

	//BinkPlayback_ptr bkFMV = BinkPlayback_ptr(new BinkPlayback());
	//bkFMV->OpenBackground("Video/TUT_DeskCU_TXT_BG.bik");

	//BinkPlayback_ptr menuFMV = BinkPlayback_ptr(new BinkPlayback());
	//BinkPlayback_ptr menuFMV = make_BinkPlayback_s(new BinkPlayback());
	//menuFMV->Open("Video/RKCINEC.bik", 0, 0, true);

	//16 bits per sample mono
	//_dataBuffers uint8_t
	//Mix_Chunk chunk = new byte[20];

	// loop variables
	int exit_requested = 0;
	int wait = 25;
	SDL_Event event;
	ImGuiIO& io = ImGui::GetIO();
	bool show_another_window = true;
	bool toggle = true;
	int scenenum = 0;
	bool check = false;

#ifdef __SWITCH__
	while (!exit_requested && appletMainLoop())
#else
	while (!exit_requested)
#endif
	{
		if (sceneChangeFlag)
		{
			sceneChangeFlag = false;
			//Used as scene control variable
			flags[0] = false;
			HIFF::Load_HIFF(sceneChangeName.c_str());
		}

		while (SDL_PollEvent(&event))
		{
			// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
			ImGui_ImplSDL2_ProcessEvent(&event);

			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(Graphics::window.get()))
			{
				exit_requested = 1;
				break;
			}

			switch (event.type)
			{
			case SDL_MOUSEBUTTONDOWN:
			case SDL_FINGERDOWN:
				if (!io.WantCaptureMouse)
					currentScene->EventProc(event);
				break;
			case SDL_QUIT:
				exit_requested = 1;
				break;

				// use joystick
			case SDL_JOYBUTTONDOWN:

#ifdef __SWITCH__
				switch (event.type)
				{
				case KEY_LSTICK_UP:
					if (wait > 0)
						wait--;
					break;
				case KEY_LSTICK_DOWN:
					if (wait < 100)
						wait++;
					break;
				case KEY_PLUS:
					if (wait > 0)
						exit_requested = 1;
					break;
			}
#else
			case SDL_KEYDOWN:
				if (io.WantCaptureKeyboard)
					break;
				/* Check the SDLKey values and move change the coords */
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					printf("\nShutting down engine\n");
					exit_requested = 1;
					break;
				}
#endif
		}
	}

		SDL_SetRenderDrawColor(Graphics::renderer.get(), 255, 0, 0, 0xFF);
		SDL_RenderClear(Graphics::renderer.get());

		currentGUI->Draw();
		currentScene->Draw();

		// put text on screen
		//if (helloworld_tex)
			//SDL_RenderCopy(Graphics::renderer.get(), helloworld_tex.get(), NULL, &helloworld_rect);

		//bkFMV->Draw();
		//menuFMV->Draw();

		SDL_RenderPresent(Graphics::renderer.get());

		graphics->frameWait();
}

	//menuFMV->Close();
	//quit();
}