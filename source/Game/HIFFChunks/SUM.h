#pragma once
#include <fstream>

#include <Engine/Scene.h>

class SUM
{
public:
	static bool Parse(std::ifstream& inFile, std::string sceneName);
protected:
};
