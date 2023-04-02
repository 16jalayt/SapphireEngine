#pragma once

#include <string>

#include "Engine/SDL_ptr.h"
#include "Engine/sprite.h"
#include "Engine/AvP_BinkPlayback.h"
#include "Engine/Button.h"

//CurrentScene is static.
//Use to reference class functions.

class Scene
{
public:
	~Scene();
	Scene();
	void SetHeader(std::string name, std::string sceneFile);
	Sprite_ptr bk;
	BinkPlayback_ptr bkFMV;
	std::vector<Sprite_ptr> ovls;
	std::vector<Button_ptr> hots;
	std::string sceneFile;
	std::string sceneName;
	bool changeMusic;

	void Draw();
	void Run();
	void EventProc(SDL_Event event);
	void setBkg(std::string fileName);
	void AddSprite(Sprite_ptr sprite);
	void AddHotzone(Button_ptr hot);
	void AddMusic(std::string sound, int channel, int loop, int chan1, int chan2);
	void AddSound(std::string sound, int channel, int loop, int chan1, int chan2);

protected:
};
using Scene_ptr = std::shared_ptr<Scene>;

extern Scene_ptr currentScene;
extern Scene_ptr nextScene;
extern bool sceneChangeFlag;
extern bool sceneReloadFlag;
extern std::string sceneChangeName;
extern std::string prevScene;
//extern int sceneNum;
extern bool flags[];
extern std::string flagDesc[];
//extern bool firstLoad;

void ChangeScene(std::string sceneName);
void ReloadScene();