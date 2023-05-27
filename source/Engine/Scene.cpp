#include "Scene.h"

#include "audio.h"
#include "Nancy/AVF.h"
#include "Nancy/Loader.h"
#include "Config.h"
#include <loguru.hpp>

Scene_ptr currentScene;
Scene_ptr nextScene;
bool sceneChangeFlag = false;
bool sceneReloadFlag = false;
std::string sceneChangeName;
//int sceneNum = 0;
std::string prevScene;
//bool firstLoad = true;
//Number for ghost dogs
bool flags[576];

Scene::Scene()
{
	sceneFile = "";
	sceneName = "";
	changeMusic = false;
}

void Scene::SetHeader(std::string description, std::string sceneFile)
{
	LOG_F(INFO, "\n\n\n\n*******New scene: %s: %s*******", sceneFile.c_str(), description.c_str());
	sceneName = sceneFile;
}

Scene::~Scene()
{
}

void Scene::Draw()
{
	if (this != NULL)
	{
		if (bkFMV)
		{
			SDL_SetRenderTarget(Graphics::renderer.get(), GUI::canvas.get());
			SDL_RenderCopy(Graphics::renderer.get(), bkFMV.get(), NULL, NULL);
			SDL_SetRenderTarget(Graphics::renderer.get(), NULL);
		}
		else if (bk)
			bk->Draw();

		for (auto& fmv : fmvs) {
			fmv->Draw();
		}

		for (auto& ovl : ovls) {
			ovl->Draw();
		}

		for (auto& hot : hots) {
			hot->Draw();
		}
	}
}

void Scene::EventProc(SDL_Event event)
{
	if (this != NULL && hots.size() != 0)
	{
		for (auto& hot : hots) {
			//if (hot->isVisible())
			hot->Event(event);
		}
	}
}

void Scene::setBkg(std::string backName)
{
	if (backName.empty())
		return;

	LOG_F(INFO, "Background: %s", backName.c_str());

	std::string fileName = Loader::getVideoPath(backName);

	//don't need much validation. Sub objects will handle that.
	if (fileName.empty())
		return;

	//determine if video or graphic and open
	std::string ext = fileName.substr(fileName.find("."));

	if (ext == ".bik")
	{
		//bkFMV = make_BinkPlayback_s(new BinkPlayback());
		//bkFMV->OpenBackground(fileName);
		FFPlayer_ptr player = std::make_shared<FFPlayer>(fileName, 0, 0);
		bkFMV = player->GetFrame();
	}
	else if (ext == ".avf")
	{
		bk = Sprite_ptr(new Sprite(std::move(AVF::parseAVF(fileName.c_str())[0]), 0, 0));
	}
	else if (ext == ".png" || ext == ".jpg")
	{
		bk = Sprite_ptr(new Sprite(fileName.c_str(), 0, 0));
	}
	else
	{
		LOG_F(ERROR, "Unknown background type in: %s", fileName.c_str());
	}
}

void Scene::AddSprite(Sprite_ptr sprite)
{
	ovls.push_back(sprite);
}

void Scene::AddHotzone(Button_ptr hot)
{
	hots.push_back(hot);
}

void Scene::AddMovie(Movie_ptr fmv)
{
	if (fmv)
		fmvs.push_back(fmv);
}

//create music paused
void Scene::AddMusic(std::string sound, int channel, int loop, int chan1, int chan2)
{
	//So as to not restart music at every scene change
	if (currentMusicName != sound)
	{
		changeMusic = true;
		currentMusicName = sound;

		std::string path = Loader::getSoundPath(sound);
		if (path.empty())
		{
			LOG_F(ERROR, "Sound could not be found: %s", sound.c_str());
			return;
		}
		//change music
		//only 1 music at a time
		currentMusic = Mix_LoadMUS(path.c_str());
		if (!currentMusic)
		{
			LOG_F(ERROR, "Sound could not be loaded: %s SDL_Error: %s", sound.c_str(), SDL_GetError());
			return;
		}
	}

	//TODO: make chanel map and play at start?
	//create array at default 8 with err catch. can resize.
	//need deiunit for sound?
}

//create audio paused
void Scene::AddSound(std::string sound, int channel, int loop, int chan1, int chan2)
{
	std::string path = Loader::getSoundPath(sound);
	if (path.empty())
	{
		LOG_F(ERROR, "Sound could not be found: %s", sound.c_str());
		return;
	}

	Mix_Chunk* waves = Mix_LoadWAV(path.c_str());
	if (!waves)
	{
		LOG_F(ERROR, ".WAV sound '%s' could not be loaded! SDL_Error: %s", "", SDL_GetError());
		return;
	}

	// Play waves sound
	if (Mix_PlayChannel(channel, waves, loop) == -1)
	{
		LOG_F(ERROR, "Waves sound could not be played! SDL_Error: %s", SDL_GetError());
		Mix_FreeChunk(waves);
		return;
	}
	//Make sure channel paused
	if (!Mix_Paused(channel))
		Mix_Pause(channel);
	//only works on se, not sure how useful in ND over mono volume
	Mix_SetPanning(channel, chan1, chan2);
}

//For internal use. Call Loader::loadScene instead
void _ChangeScene(std::string sceneName)
{
	if (sceneName != "9999")
	{
		sceneChangeName = sceneName;
		sceneChangeFlag = true;
	}
}

//For internal use. Call Loader::loadScene instead
void _LoadScene(std::string sceneName)
{
	nextScene = Scene_ptr(new Scene());
	//used for navigaiton
	//TODO: go all the way to 30?
	flags[0] = false;
	flags[1] = false;
	flags[2] = false;
	flags[3] = false;
	flags[4] = false;
	HIFF::Load_HIFF(sceneName);
	currentScene = nextScene;
	currentScene->Run();
}

void ReloadScene()
{
	currentScene->ovls.clear();
	currentScene->hots.clear();
	currentScene->fmvs.clear();
	HIFF::Load_HIFF(currentScene->sceneFile);
	currentScene->Run();
}

void Scene::Run()
{
	for (auto& fmv : fmvs) {
		if (fmv->FMV)
			fmv->FMV->_paused = false;
	}

	if (!Config::debugNoSound)
	{
		//Start music if not holding over from last scene
		if (currentMusic != NULL && changeMusic)
		{
			changeMusic = false;
			Mix_PauseMusic();
			if (Mix_PlayMusic(currentMusic, -1) == -1)
			{
				LOG_F(ERROR, "Sound could not be played!\n"
					"SDL_Error: %s", SDL_GetError());
			}
		}

		//Unpause all valid audio channals
		for (int i = 0; i < sizeof(soundChannels) / sizeof(Mix_Chunk*); i++)
		{
			//only toggle if paused
			if (soundChannels[i] != NULL && Mix_Paused(i))
			{
				Mix_Pause(i);
			}
		}
	}
	//start video
}