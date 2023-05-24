#define SDL_MAIN_HANDLED
//Added to loguru.cpp. Alternately use VCPKG
#define LOGURU_WITH_STREAMS 1
#include <loguru.hpp>

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
//#include "vld.h"

#include "globals.h"
#include "Engine/utils.h"
#include "Engine/graphics.h"
#include "Engine/sprite.h"
#include "Engine/SDL_ptr.h"
#include "Engine/Button.h"
#include "Engine/AvP_BinkPlayback.h"
#include "Engine/Scene.h"
#include "Nancy/Loader.h"
#include "Engine/GUI.h"
#include <Engine/Config.h>

/*void tracetest()
{
	LOG_S(ERROR) << loguru::stacktrace(1).c_str();
}*/

int main(int argc, char** argv)
{
	Config::parse(argc, argv);

#ifdef _DEBUG
	loguru::g_stderr_verbosity = loguru::Verbosity_MAX;
#else
	loguru::g_stderr_verbosity = loguru::Verbosity_ERROR;
#endif

	//field length modified at loguru.hpp line 131
	loguru::g_preamble_uptime = false;
	loguru::g_preamble_date = false;

	//init important for crash logging
	loguru::init(argc, argv);
	//Init sets to main thread by default
	//loguru::set_thread_name("Main Thread");
	loguru::add_file("game.log", loguru::Truncate, loguru::Verbosity_WARNING);
	/*LOG_F(INFO, "I'm hungry for some %.3f!", 3.14159);
	//LOG_S(INFO) << "Some float: " << 3.14;
	LOG_S(ERROR) << "My vec3: " << 3.14;
	//LOG_S(ERROR) << loguru::stacktrace(1).c_str();
	//tracetest();*/

	//LOG_SCOPE_FUNCTION(INFO);
	//VLOG_SCOPE_F(1, "Iteration %d", 5);

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
	ImGuiIO& io = ImGui::GetIO();
	bool toggle = true;
	int scenenum = 0;
	bool check = false;

	/*std::vector<BinkPlayback_ptr> test;
	std::string path = Loader::getVideoPath("YogiCine_");
	BinkPlayback_ptr menuFMV = make_BinkPlayback_s(new BinkPlayback());
	menuFMV->Open(path, 0, 0, false);
	test.push_back(menuFMV);
	//nextScene->AddMovie(menuFMV);*/

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

		SDL_RenderPresent(Graphics::renderer.get());

		graphics->frameWait();
	}

	//Now in graphics deconstructor
	//menuFMV->Close();
	//quit();
}