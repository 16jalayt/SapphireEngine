#pragma once
#include <fstream>
#include <Engine/Scene.h>

class ACT
{
public:
	static bool Parse(std::ifstream& inFile, Scene_ptr& scene, int chunkLen, int chunkStart);
protected:
};
