#include "Engine/SapphireApp.h"
#include "Engine/Graphics.h"
#include "Engine/Globals.h"
#include "Engine/Utils.h"
#include "Engine/Cursor.h"

#ifdef __APPLE__
#include <unistd.h>
#endif
#include <time.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef __SWITCH__
#include <switch.h>
#elif !defined(__APPLE__)
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#endif

SapphireApp::SapphireApp(std::string gameName, std::string gameDesc)
{
	GAMENAME = gameName;
	GAMEDESC = gameDesc;

#ifdef __SWITCH__

	//Log to Ryujinx
	consoleDebugInit(debugDevice_SVC);

	//Log to nxlink on actual hardware
	socketInitializeDefault();
	nxlinkStdio();
	//use cerr for both to work

	//Running off sd card now so no romfs
	/*Result rc = romfsInit();
	if (R_FAILED(rc))
		printf("romfsInit: %08X\n", rc);
	chdir("romfs:/");*/

	chdir("/switch/SapphireEngine/");

#endif

#if defined(__APPLE__)
#include <RessourcesDirectory.hpp>
#include <Engine/Config.h>
#include <Engine/Graphics.h>
#include <Engine/Utils.h>
#include <Engine/Cursor.h>
	std::string path = GetRPath();
	printf("resdir: %s\n", path.c_str());
	if (path.empty())
	{
		printf("\nUnable to get resource path\n");
		quit();
	}
	chdir(path.c_str());

	//    //argv = /
	//    int i;
	//        printf("argc:%d\n",argc);
	//        for(i=0;i<argc;i++)
	//        {
	//            printf("arg:%s\n",argv[i]);
	//        }
	//
	//    char* dir= (char *) malloc (50);
	//        dir = getenv("pwd");
	//        printf("Workdir: %s\n", dir);
	//        free(dir);
	//
	//        int result;
	//        result = system("pwd");
	//        printf("%d\n", result);
	//
	//    char cwd[PATH_MAX];
	//       if (getcwd(cwd, sizeof(cwd)) != NULL) {
	//           printf("Current working dir: %s\n", cwd);
	//       } else {
	//           perror("getcwd() error");
	//           return;
	//       }
#endif

	_graphics = std::make_unique<Graphics>();
	// initial subsystem. If error, just exit. Error already printed.
	if (_graphics->init("data/Nintendo_Switch_Logo_resized.png") < 0)
		quit();

	initControls();
}

void SapphireApp::initControls()
{
	/*// SDL_InitSubSystem(SDL_INIT_JOYSTICK);
	SDL_JoystickEventState(SDL_ENABLE);
	SDL_JoystickOpen(0);
	// open CONTROLLER_PLAYER_1 and CONTROLLER_PLAYER_2
   // when railed, both joycons are mapped to joystick #0,
   // else joycons are individually mapped to joystick #0, joystick #1, ...
   // https://github.com/devkitPro/SDL/blob/switch-sdl2/src/joystick/switch/SDL_sysjoystick.c#L45
	for (int i = 0; i < 2; i++)
	{
		if (SDL_JoystickOpen(i) == NULL)
		{
#ifdef __SWITCH__
			fatalError("%s: SDL_JoystickOpen: %s", __func__, SDL_GetError());
			printf("No joysticks connected\n");
#endif
		}
	}*/
	//SWITCH_JoystickRumble(SDL_Joystick * joystick, Uint16 low_frequency_rumble, Uint16 high_frequency_rumble)
}

void SapphireApp::processEvents()
{
}

//Audio_ptr SapphireApp::getAudio()
//{
	//return _audio;
//}

void SapphireApp::startFrame()
{
	SDL_SetRenderDrawColor(Graphics::renderer.get(), 255, 0, 0, 0xFF);

	SDL_RenderClear(Graphics::renderer.get());
}

void SapphireApp::endFrame()
{
	Cursor::DrawCursor();
	/*if (Cursor::CursorChanged)
		{
			Cursor::DrawCursor();
			Cursor::CursorChanged = false;
		}*/

	SDL_RenderPresent(Graphics::renderer.get());

	_graphics->frameWait();
}