#include "Engine/Config.h"

//Made changes to loguru.cpp. loguru is also in VCPKG
#define LOGURU_WITH_STREAMS 1
#define TOML11_COLORIZE_ERROR_MESSAGE 1
#include <loguru/loguru.hpp>
//bad practice but vcpkg doesn't compile, so linker fails
#include <loguru/loguru.cpp>
#include <iostream>

//cmd parse
#include <cxxopts.hpp>
//config file
#include <toml.hpp>

bool Config::fullscreen;
bool Config::logfile;

bool Config::debugHot;
bool Config::debugNoSound;
bool Config::lograw;

//TODO:add option for startup scene
//TODO: add some sort of check if missing assets, exit

void Config::parse(int argc, char** argv)
{
	const std::string configName = "config.toml";
	//Parse the config file first so command line arguments can overwrite
	try
	{
		auto data = toml::parse(configName);
		Config::fullscreen = toml::find_or<bool>(data, "fullscreen", true);
		Config::logfile = toml::find_or<bool>(data, "logfile", false);

		const auto& debug = toml::find(data, "debug");
		Config::debugHot = toml::find_or<bool>(debug, "debugHot", false);
		Config::debugNoSound = toml::find_or<bool>(debug, "debugNoSound", false);
		Config::lograw = toml::find_or<bool>(debug, "lograw", false);
		//std::cout << apple << std::endl;
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

	//displayed in help screen
	cxxopts::Options options("Sapphire Engine", "A game engine currently implementing Nancy Drew 7");
	options.add_options()
		("h,help", "Print help")
		("f,fullscreen", "Fullscreen", cxxopts::value<bool>(Config::fullscreen))
		("v,verbosity", "Change console logging level: INFO, WARNING, ERROR")
		;

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

	initLog(argc, argv);
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

	//field length modified at loguru.hpp line 131
	if (Config::lograw)
	{
		//loguru::g_preamble_header = false;
		loguru::g_preamble = false;
	}
	else
	{
		loguru::g_preamble_uptime = false;
		loguru::g_preamble_date = false;
	}

	//init important for crash logging
	loguru::init(argc, argv);
	//Init sets to main thread by default
	//loguru::set_thread_name("Main Thread");
	if (Config::logfile)
		loguru::add_file("game.log", loguru::Truncate, loguru::Verbosity_INFO);
	/*LOG_F(INFO, "I'm hungry for some %.3f!", 3.14159);
	//LOG_S(INFO) << "Some float: " << 3.14;
	LOG_S(ERROR) << "My vec3: " << 3.14;
	//LOG_S(ERROR) << loguru::stacktrace(1).c_str();
	//tracetest();*/

	//LOG_SCOPE_FUNCTION(INFO);
	//VLOG_SCOPE_F(1, "Iteration %d", 5);
}