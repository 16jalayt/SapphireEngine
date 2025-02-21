#pragma once
#include "Engine/Globals.h"
#include <string>

#define TOML11_COLORIZE_ERROR_MESSAGE 1
#define LOGURU_EXPORT SE_API
#define LOGURU_WITH_STREAMS 1
#define LOGURU_FILENAME_WIDTH 15
#define LOGURU_THREADNAME_WIDTH 13
#ifdef __SWITCH__
//For switch only?
#define LOGURU_STACKTRACES 0
#endif
//disable for insecure c functions and a warning about cxx17 standard
#pragma warning( disable : 4996 4038 )

#include <loguru.hpp>
//config file
#include <toml.hpp>

namespace Engine
{
	class SE_API Config
	{
	public:
		static void parse(int argc, char** argv);
		//static void loadStub();
		static void initLog(int argc, char** argv);
		static loguru::Verbosity GetVerbosity();

		static toml::value config;
		static bool fullscreen;
		static bool logfile;

		static bool debugHot;
		static bool debugNoSound;
		static bool debugMenuEnabled;
		static std::string logLevel;
		static bool logRaw;
		static int windowWidth;
		static int windowHeight;
		static int referenceWidth;
		static int referenceHeight;
		static float globalScale;
		static std::string gameName;
		static std::string gameDesc;
	};
}