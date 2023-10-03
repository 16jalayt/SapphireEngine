#define SDL_MAIN_HANDLED

#include <time.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef __SWITCH__
#include <switch.h>
#else
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
//#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_framerate.h>
#include <SDL2/SDL2_rotozoom.h>

//#include "vld.h"
#include <loguru.hpp>

#include "globals.h"
#include "Engine/utils.h"
#include "Engine/graphics.h"
#include "Engine/sprite.h"
#include "Engine/SDL_ptr.h"
#include "Engine/Button.h"
#include "Engine/Scene.h"
#include "Nancy/Loader.h"
#include "Engine/GUI.h"
#include <Engine/Config.h>
#include <Engine/Cursor.h>

/*void tracetest()
{
	LOG_S(ERROR) << loguru::stacktrace(1).c_str();
}*/

int main(int argc, char** argv)
{
#ifndef __SWITCH__
	Config::parse(argc, argv);
#endif

	//TODO: update everything to this syntax?
	Graphics_ptr graphics = std::make_unique<Graphics>();
	//Graphics_ptr graphics = Graphics_ptr(new Graphics());

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
#ifndef __SWITCH__
	ImGuiIO& io = ImGui::GetIO();
#endif
	bool toggle = true;
	int scenenum = 0;
	bool check = false;

	/*std::vector<BinkPlayback_ptr> test;
	std::string path = Loader::getVideoPath("YogiCine_");
	BinkPlayback_ptr menuFMV = make_BinkPlayback_s(new BinkPlayback());
	menuFMV->Open(path, 0, 0, false);
	test.push_back(menuFMV);
	//nextScene->AddMovie(menuFMV);*/

	/*Mix_Chunk* gMedium = Mix_LoadWAV("HDSound/NGD198.ogg");
	//Mix_Chunk* gMedium = Mix_LoadWAV("data/pop1.wav");
	if (gMedium == NULL)
	{
		printf("Failed to load medium sound effect! SDL_mixer Error: %s\n", Mix_GetError());
	}
	Mix_PlayChannel(-1, gMedium, 0);*/

#ifdef __SWITCH__
	while (!exit_requested && appletMainLoop())
#else
	while (!exit_requested)
#endif
	{
		if (sceneReloadFlag)
		{
			ReloadScene();
		}
		if (sceneChangeFlag)
		{
			sceneChangeFlag = false;
			//Used as scene control variable
			//flags[0] = false;
			//HIFF::Load_HIFF(sceneChangeName);
			_LoadScene(sceneChangeName);
		}

		while (SDL_PollEvent(&event))
		{
#ifndef __SWITCH__
			// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
			ImGui_ImplSDL2_ProcessEvent(&event);
#endif
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(Graphics::window.get()))
			{
				exit_requested = 1;
				break;
			}

			switch (event.type)
			{
			case SDL_QUIT:
				exit_requested = 1;
				break;

				// use joystick
#ifdef __SWITCH__
			case SDL_JOYBUTTONDOWN:
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
				if (event.key.keysym.sym == SDLK_ESCAPE)
				{
					LOG_F(ERROR, "\nShutting down engine\n");
					exit_requested = 1;
					break;
				}
#ifndef __SWITCH__
				if (io.WantCaptureKeyboard)
#endif
					break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_FINGERDOWN:
#ifndef __SWITCH__
				if (!io.WantCaptureMouse)
#endif
					currentScene->EventProc(event);
				break;
			case SDL_MOUSEMOTION:
#ifndef __SWITCH__
				if (!io.WantCaptureMouse)
				{
#endif
					//TODO: explicitly set to system cursor for IMGUI?
					Cursor::CursorChanged = false;
					currentScene->EventProc(event);
#ifndef __SWITCH__
				}
#endif
				break;

				/*switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						LOG_F(ERROR, "\nShutting down engine\n");
						exit_requested = 1;
						break;
				default:
					break;
				}*/
#endif
			}
		}

		SDL_SetRenderDrawColor(Graphics::renderer.get(), 255, 0, 0, 0xFF);

		//TODO: only render not canvas?
		SDL_RenderClear(Graphics::renderer.get());

		/*SDL_SetRenderTarget(Graphics::renderer.get(), GUI::canvas.get());
		SDL_RenderClear(Graphics::renderer.get());
		SDL_SetRenderTarget(Graphics::renderer.get(), NULL);*/

		currentScene->Draw();
		currentGUI->Draw();

		// put text on screen
		//if (helloworld_tex)
			//SDL_RenderCopy(Graphics::renderer.get(), helloworld_tex.get(), NULL, &helloworld_rect);

		//bkFMV->Draw();
		/*for (auto& fmv : test) {
			fmv->Draw();
		}*/
		//menuFMV->Draw();

		//Doesn't have to be here. Handled by SDL
		Cursor::DrawCursor();
		/*if (Cursor::CursorChanged)
		{
			Cursor::DrawCursor();
			Cursor::CursorChanged = false;
		}*/

		SDL_RenderPresent(Graphics::renderer.get());

		graphics->frameWait();
	}

	//Now in graphics deconstructor
	//menuFMV->Close();
	//quit();
}