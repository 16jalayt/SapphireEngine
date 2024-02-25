#pragma once

class Config
{
public:
	static void parse(int argc, char** argv);
	static void initLog(int argc, char** argv);

	static bool fullscreen;
	static bool logfile;

	static bool debugHot;
	static bool debugNoSound;
	static bool lograw;
};