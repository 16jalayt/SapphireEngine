#include "Engine/Config.h"

#include <iostream>
//cmd parse
#include <cxxopts.hpp>
#include <Engine/Utils.h>

using namespace Engine;

toml::value Config::config;
bool Config::fullscreen;
bool Config::logfile;

bool Config::debugHot;
bool Config::debugNoSound;
bool Config::debugMenuEnabled;
std::string Config::logLevel;
bool Config::logRaw;
int Config::referenceWidth;
int Config::referenceHeight;
int Config::windowWidth;
int Config::windowHeight;
float Config::globalScale;
std::string Config::gameName = "Sapphire Engine";
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
		Config::config = toml::parse(PathFixer(configName));
		Config::fullscreen = toml::find_or<bool>(config, "fullscreen", true);
		Config::logfile = toml::find_or<bool>(config, "logfile", true);

		Config::gameName = toml::find_or<std::string>(config, "gameName", "Sapphire Engine");
		Config::gameDesc = toml::find_or<std::string>(config, "gameDesc", "A game engine");

		Config::windowWidth = toml::find_or<int>(config, "windowWidth", 1920);
		Config::windowHeight = toml::find_or<int>(config, "windowHeight", 1080);
		Config::referenceWidth = toml::find_or<int>(config, "referenceWidth", 800);
		Config::referenceHeight = toml::find_or<int>(config, "referenceHeight", 600);
		//TODO: calculate?
		Config::globalScale = toml::find_or<float>(config, "globalScale", 1.00);

		//Debug section should be last in case error?
		const auto& debug = toml::find(config, "debug");
		Config::debugHot = toml::find_or<bool>(debug, "debugHot", false);
		Config::debugNoSound = toml::find_or<bool>(debug, "debugNoSound", false);
		Config::debugMenuEnabled = toml::find_or<bool>(debug, "debugMenuEnabled", false);
		Config::logLevel = toml::find_or<std::string>(config, "logLevel", "Error");
		//move from debug?
		Config::logRaw = toml::find_or<bool>(debug, "logRaw", false);
	}
	catch (std::ios_base::failure&)
	{
		//TODO: figure out new syntax for v4
#if TOML11_VERSION_MAJOR == 3
		std::cout << "The config file: " << configName << " not found. Creating..." << std::endl;
		const toml::value data{
			{"fullscreen", Config::fullscreen},
			{ "logfile", Config::logfile },
		};
		std::ofstream out(configName);
		out << data;
		out.close();
		//TODO: continue if succesful
		//exit(-2);
#endif // TOML11_VERSION_MAJOR 3
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

//Not used
/*void Config::loadStub()
{
	auto data = toml::parse("stub.toml");

	Config::gameName = toml::find_or<std::string>(data, "gameName", "Sapphire Engine");
}*/

void Config::initLog(int argc, char** argv)
{
	//Why c++, does string not have basic members
	std::transform(Config::logLevel.begin(), Config::logLevel.end(), Config::logLevel.begin(), ::toupper);
	if (Config::logLevel == "MAX")
		loguru::g_stderr_verbosity = loguru::Verbosity_MAX;
	else if (Config::logLevel == "ERROR")
		loguru::g_stderr_verbosity = loguru::Verbosity_ERROR;
	else if (Config::logLevel == "WARNING")
		loguru::g_stderr_verbosity = loguru::Verbosity_WARNING;
	else if (Config::logLevel == "INFO")
		loguru::g_stderr_verbosity = loguru::Verbosity_INFO;

	//If debug build, force to max
#ifdef _DEBUG
	loguru::g_stderr_verbosity = loguru::Verbosity_MAX;
#endif

	//Force to max because switch is hard to debug
#ifdef __SWITCH__
	loguru::g_stderr_verbosity = loguru::Verbosity_MAX;
#endif

	if (Config::logRaw)
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
		loguru::add_file(PathFixer("game.log").c_str(), loguru::Truncate, loguru::Verbosity_INFO);

	//Logging tests/examples
	//LOG_F(INFO, "I'm hungry for some %.3f!", 3.14159);
	//LOG_S(INFO) << "Some float: " << 3.14;
	//LOG_S(ERROR) << "My vec3: " << 3.14;
	//LOG_S(ERROR) << loguru::stacktrace(1).c_str();
	//tracetest();

	//LOG_SCOPE_FUNCTION(INFO);
	//VLOG_SCOPE_F(1, "Iteration %d", 5);
}

loguru::Verbosity Config::GetVerbosity()
{
	return loguru::g_stderr_verbosity;
}