#include <string>
#include "Graphics.h"
#include "Globals.h"

class SE_API SapphireApp
{
public:
	SapphireApp(std::string gameName = "Sapphire Engine", std::string gameDesc = "A game engine");
	void initControls();
	void startFrame();
	void endFrame();
	void processEvents();
private:
	Graphics_ptr _graphics;
};