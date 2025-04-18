//#define LOGURU_EXPORT SE_API;
#include <string>
#include "Engine/Graphics.h"
#include "Engine/Globals.h"
#include "Engine/Audio.h"
#include <memory>

class SE_API SapphireApp
{
public:
	SapphireApp(int argc, char** argv);
	virtual ~SapphireApp() {};
	void initControls();
	void startFrame();
	void endFrame();
	void processEvents();
	//Audio_ptr getAudio();
private:
	Graphics_ptr _graphics;
	//Audio_ptr _audio;
};

using SapphireApp_ptr = std::unique_ptr<SapphireApp>;