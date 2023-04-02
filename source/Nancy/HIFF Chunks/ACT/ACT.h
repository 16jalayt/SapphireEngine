#pragma once
#include <fstream>

#include "Engine/Scene.h"

class ACT
{
public:
	static bool Parse(std::ifstream& inFile, int chunkLen, int chunkStart);
protected:
};
