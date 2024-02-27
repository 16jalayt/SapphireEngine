#include "Engine/SapphireApp.h"
#include "Engine/Graphics.h"

SapphireApp::SapphireApp()
{
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

	Graphics_ptr graphics = std::make_unique<Graphics>();
}