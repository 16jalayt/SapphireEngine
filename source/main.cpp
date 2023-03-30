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

#ifdef __SWITCH__
	while (!exit_requested && appletMainLoop())
#else
	while (!exit_requested)
#endif
	{
		if (sceneChangeFlag)
		{
			HIFF::Load_HIFF(sceneChangeName.c_str());
			sceneChangeFlag = false;
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

		// Start the Dear ImGui frame
		ImGui_ImplSDLRenderer_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		//bool show_demo_window = true;
		//if (show_demo_window)
		//    ImGui::ShowDemoWindow(&show_demo_window);

		SDL_SetRenderDrawColor(Graphics::renderer.get(), 255, 0, 0, 0xFF);
		SDL_RenderClear(Graphics::renderer.get());

		currentGUI->Draw();
		currentScene->Draw();

		// put text on screen
		//if (helloworld_tex)
			//SDL_RenderCopy(Graphics::renderer.get(), helloworld_tex.get(), NULL, &helloworld_rect);

		//bkFMV->Draw();
		//menuFMV->Draw();

		if (show_another_window)
		{
			//Forces to be immoveable
			//ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::SetNextWindowSize(ImVec2(200, 300));
			// Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Begin("Another Window", &show_another_window,
				ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);

			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				printf("ouch\n");
			ImGui::End();
		}

		// Rendering
		ImGui::Render();
		ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

		SDL_RenderPresent(Graphics::renderer.get());

		graphics->frameWait();
	}

	//menuFMV->Close();
	//quit();
}