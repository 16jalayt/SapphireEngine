#pragma once

class Config
{
public:
	static void parse(int argc, char** argv);
	static bool fullscreen;
	static bool debugHot;
	static bool debugNoSound;
};