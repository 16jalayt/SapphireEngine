#pragma once

#include "Nancy/CIFF.h"
#include "Nancy/HIFF.h"
#include "GUI.h"
//load
//load video
//load audio
//load ovl
//boot   loads starting assets and starts first script

class Loader
{
public:
	static bool Boot();
	static void UIInit();
	static std::ifstream loadTree(std::string treeName);
	static std::ifstream getDataFile(std::string sceneName);
	static std::string getBackgroundPath(std::string backName);
	static std::string getSoundPath(std::string soundName);
protected:
};

extern GUI_ptr currentGUI;