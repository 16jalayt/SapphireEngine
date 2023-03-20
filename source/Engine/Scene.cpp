#include "Scene.h"
#include "Nancy/Loader.h"
#include "audio.h"
#include "Nancy/AVF.h"

Scene_ptr currentScene;
bool sceneChangeFlag = false;
std::string sceneChangeName;

Scene::Scene(std::string bk, std::string name, std::string sceneFile)
{
	printf("\n\n*******New scene: %s: %s*******\n", sceneFile.c_str(), name.c_str());
	sceneName = name;
	changeMusic = false;

	setBkg(bk);
}

Scene::~Scene()
{
	if (bkFMV)
		bkFMV->Close();
}

void Scene::Draw()
{
	if (this != NULL)
	{
		if (bkFMV)
			bkFMV->Draw();
		else if (bk)
			bk->Draw();

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

	printf("Background: %s\n", backName.c_str());

	std::string fileName = Loader::getBackgroundPath(backName);

	//don't need much validation. Sub objects will handle that.
	if (fileName.empty())
		return;

	//determine if video or graphic and open
	std::string ext = fileName.substr(fileName.find("."));

	if (ext == ".bik")
	{
		bkFMV = make_BinkPlayback_s(new BinkPlayback());
		bkFMV->OpenBackground(fileName);
	}
	else if (ext == ".avf")
	{
		bk = Sprite_ptr(new Sprite(AVF::parseAVF(fileName.c_str()), 0, 0));
	}
	else if (ext == ".png" || ext == ".jpg")
	{
		bk = Sprite_ptr(new Sprite(fileName.c_str(), 0, 0));
	}
	else
	{
		printf("Unknown background type in: %s\n", fileName.c_str());
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
			printf("Sound could not be found: %s\n", sound.c_str());
			return;
		}
		//change music
		//only 1 music at a time
		currentMusic = Mix_LoadMUS(path.c_str());
		if (!currentMusic)
		{
			printf("Sound could not be loaded: %s SDL_Error: %s\n", sound.c_str(), SDL_GetError());
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
		printf("Sound could not be found: %s\n", sound.c_str());
		return;
	}

	Mix_Chunk* waves = Mix_LoadWAV(path.c_str());
	if (!waves)
	{
		printf(".WAV sound '%s' could not be loaded!\n SDL_Error: %s\n", "", SDL_GetError());
		return;
	}

	// Play waves sound
	if (Mix_PlayChannel(channel, waves, loop) == -1)
	{
		printf("Waves sound could not be played!\n SDL_Error: %s\n", SDL_GetError());
		Mix_FreeChunk(waves);
		return;
	}
	//Make sure channel paused
	if (!Mix_Paused(channel))
		Mix_Pause(channel);
	//only works on se, not sure how useful in ND over mono volume
	Mix_SetPanning(channel, chan1, chan2);
}

void ChangeScene(Scene_ptr newScene)
{
	currentScene = newScene;
	currentScene->Run();
}

void Scene::Run()
{
	if (!debugNoSound)
	{
		//Start music if not holding over from last scene
		if (currentMusic != NULL && changeMusic)
		{
			changeMusic = false;
			Mix_PauseMusic();
			if (Mix_PlayMusic(currentMusic, -1) == -1)
			{
				printf("Sound could not be played!\n"
					"SDL_Error: %s\n", SDL_GetError());
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