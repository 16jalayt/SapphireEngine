#define SDL_MAIN_HANDLED
//#include <vld.h>
#include <time.h>
#include <stdio.h>
#include <stdarg.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
//#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_framerate.h>
#include <SDL2/SDL2_rotozoom.h>
#include "Engine/SDL_ptr.h"

#ifdef __SWITCH__
#include <switch.h>
#endif

#include "globals.h"
#include "Engine/utils.h"
#include "Engine/graphics.h"
#include "Engine/sprite.h"
#include "Engine/Button.h"
#include <Engine/AvP_BinkPlayback.h>
#include "Engine/Scene.h"
#include "Nancy/Loader.h"
#include "Nancy/GUI.h"

//TODO: config manager

int main(int argc, char** argv)
{
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
			switch (event.type)
			{
			case SDL_MOUSEBUTTONDOWN:
			case SDL_FINGERDOWN:
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
	quit();
}