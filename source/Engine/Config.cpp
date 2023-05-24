#include "Config.h"

#include <iostream>

//cmd parse
#include "cxxopts.hpp"
//config file
#include <toml/toml.hpp>

bool Config::fullscreen;
bool Config::debugHot;
bool Config::debugNoSound;

void Config::parse(int argc, char** argv)
{
	//Parse the config file first so command line arguments can overwrite
	try
	{
		auto data = toml::parse("config.toml");
		Config::fullscreen = toml::find_or<bool>(data, "fullscreen", true);

		const auto& debug = toml::find(data, "debug");
		Config::debugHot = toml::find_or<bool>(debug, "debugHot", false);
		Config::debugNoSound = toml::find_or<bool>(debug, "debugNoSound", false);
		//std::cout << apple << std::endl;
	}
	catch (std::ios_base::failure& e)
	{
		std::cout << e.what() << std::endl;
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
}