#pragma once
#include "Globals.h"
#include <string>

namespace Engine
{
	class SE_API Config
	{
	public:
		static void parse(int argc, char** argv);
		static void initLog(int argc, char** argv);

		static bool fullscreen;
		static bool logfile;

		static bool debugHot;
		static bool debugNoSound;
		static bool debugMenuOpen;
		static bool lograw;
		static int windowWidth;
		static int windowHeight;
		static int referenceWidth;
		static int referenceHeight;
		static float globalScale;
		static std::string gameName;
		static std::string gameDesc;
	};
}