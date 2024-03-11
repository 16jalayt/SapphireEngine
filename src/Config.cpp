#include "Engine/Config.h"

#define TOML11_COLORIZE_ERROR_MESSAGE 1
#define LOGURU_IMPLEMENTATION 1
#define LOGURU_WITH_STREAMS 1
#define LOGURU_FILENAME_WIDTH 15
#define LOGURU_THREADNAME_WIDTH 13
//disable for insecure c functions and a warning about cxx17 standard
#pragma warning( disable : 4996 4038 )
#include <loguru.hpp>
#include <iostream>

//cmd parse
#include <cxxopts.hpp>
//config file
#include <toml.hpp>
//#include <Engine/Globals.h>

using namespace Engine;

bool Config::fullscreen;
bool Config::logfile;

bool Config::debugHot;
bool Config::debugNoSound;
bool Config::lograw;
int Config::referenceWidth;
int Config::referenceHeight;
int Config::windowWidth;
int Config::windowHeight;
float Config::globalScale;
std::string Config::gameName;
std::string Config::gameDesc;

//TODO:add option for startup scene
//TODO: add some sort of check if missing assets, exit
//TODO: move to user to customize

void Config::parse(int argc, char** argv)
{
	const std::string configName = "config.toml";
	//Parse the config file first so command line arguments can overwrite
	try
	{
		auto data = toml::parse(configName);
		Config::fullscreen = toml::find_or<bool>(data, "fullscreen", true);
		Config::logfile = toml::find_or<bool>(data, "logfile", false);

		Config::gameName = toml::find_or<std::string>(data, "gameName", "Sapphire Engine");
		Config::gameDesc = toml::find_or<std::string>(data, "gameDesc", "A game engine");

		const auto& debug = toml::find(data, "debug");
		Config::debugHot = toml::find_or<bool>(debug, "debugHot", false);
		Config::debugNoSound = toml::find_or<bool>(debug, "debugNoSound", false);
		Config::lograw = toml::find_or<bool>(debug, "lograw", false);
		Config::windowWidth = toml::find_or<int>(data, "windowWidth", 1920);
		Config::windowHeight = toml::find_or<int>(data, "windowHeight", 1080);
		Config::referenceWidth = toml::find_or<int>(data, "referenceWidth", 800);
		Config::referenceHeight = toml::find_or<int>(data, "referenceHeight", 600);
		Config::globalScale = toml::find_or<float>(data, "globalScale", 1.00);
	}
	catch (std::ios_base::failure&)
	{
		std::cout << "The config file: " << configName << " not found. Creating..." << std::endl;
		const toml::value data{
			{"fullscreen", Config::fullscreen},
			{ "logfile", Config::logfile },
		};
		std::ofstream out(configName);
		out << data;
		out.close();
		//TODO: continue if succesful
		exit(-2);
	}
	catch (toml::syntax_error& e)
	{
		std::cout << e.what() << std::endl;
		exit(-3);
	}
	//Missing table (section), probably [debug]. Keep running.
	catch (std::out_of_range)
	{
	}

	//Command line parameters

	//TODO: text
	//displayed in help screen
	cxxopts::Options options(gameName, gameDesc);
	options.add_options()
		("h,help", "Print help")
		("f,fullscreen", "Fullscreen", cxxopts::value<bool>(Config::fullscreen))
		("v,verbosity", "Change console logging level: INFO, WARNING, ERROR");

	try
	{
		auto result = options.parse(argc, argv);
		if (result.count("help"))
		{
			std::cout << options.help() << std::endl;
			exit(-4);
		}
	}
	catch (cxxopts::exceptions::exception)
	{
		printf("Invalid argument");
		exit(-5);
	}
}

void Config::initLog(int argc, char** argv)
{
	//TODO: add to config
#ifdef _DEBUG
	loguru::g_stderr_verbosity = loguru::Verbosity_MAX;
#else
	loguru::g_stderr_verbosity = loguru::Verbosity_ERROR;
#endif

	//Force to max because switch hard to debug
#ifdef __SWITCH__
	loguru::g_stderr_verbosity = loguru::Verbosity_MAX;
#endif

	if (Config::lograw)
	{
		loguru::g_preamble = false;
	}
	else
	{
		loguru::g_preamble_uptime = false;
		loguru::g_preamble_date = false;
		loguru::g_preamble_thread = true;
		loguru::g_preamble_verbose = false;
	}

	//init important for crash logging
	loguru::init(argc, argv);
	//Init sets to main thread by default
	loguru::set_thread_name("Engine Thread");

	if (Config::logfile)
		loguru::add_file("game.log", loguru::Truncate, loguru::Verbosity_INFO);

	//Logging tests/examples
	//LOG_F(INFO, "I'm hungry for some %.3f!", 3.14159);
	//LOG_S(INFO) << "Some float: " << 3.14;
	//LOG_S(ERROR) << "My vec3: " << 3.14;
	//LOG_S(ERROR) << loguru::stacktrace(1).c_str();
	//tracetest();

	//LOG_SCOPE_FUNCTION(INFO);
	//VLOG_SCOPE_F(1, "Iteration %d", 5);
}