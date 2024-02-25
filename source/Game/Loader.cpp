#include "Nancy/Loader.h"
#include <Engine/Scene.h>
#include <loguru.hpp>
#include <Engine/graphics.h>

GUI_ptr currentGUI;

bool Loader::Boot()
{
	//empty scene to not nullptr rendering at begining
	//Scene_ptr scene = Scene_ptr(new Scene("", "Boot"));
	//ChangeScene(Scene_ptr(new Scene("", "Boot", "")));
	loadScene("Boot");

	CIFF::Load_Tree((char*)"DataFiles/CIFTREE.dat");
	//From danger by design. Split between cds
	//CIFF::Load_Tree((char*)"DataFiles/CIFTREE_.dat");
	CIFF::Load_Tree((char*)"DataFiles/PROMOTREE.DAT");
	//load cals, cifs, and loose hifs
	//load scene boot

	//Startup Flags:
	flags[1081 - 1000] = true;
	//DEBUG:
	//fixed flashlight
	flags[1146 - 1000] = true;
	//solved dog tile
	flags[1407 - 1000] = true;

	UIInit();

	//test loose
	if (oldUI)
		//Torchlight
		//HIFF::Load_HIFF("4020");
	//Puzzle Corrador
		//HIFF::Load_HIFF("4150");
		//tun stairs
		//loadScene("4069");
		loadScene("4604");
	// Tunnel by Kennel
	//HIFF::Load_HIFF("4141");
	else
		//scopa
		//HIFF::Load_HIFF("4450");
		loadScene("4450");

	return true;
}

void Loader::UIInit()
{
	//currentGUI = GUI_ptr(new GUI());
	currentGUI = std::make_shared<GUI>();

	if (oldUI)
	{
		SDL_Rect CanvasRect = { 52, 18, 536, 292 };
		currentGUI->canvasRect = { (int)(CanvasRect.x * GlobalScale), (int)(CanvasRect.y * GlobalScale), (int)(CanvasRect.w * GlobalScale), (int)(CanvasRect.h * GlobalScale) };
		currentGUI->canvas = SDL_Texture_ptr(SDL_CreateTexture(Graphics::renderer.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, currentGUI->canvasRect.w, currentGUI->canvasRect.h));

		Sprite_ptr frame = std::make_shared<Sprite>("DataFiles/CIFTREE/frame.png", 0, 0, RenderParent::window);
		currentGUI->AddSprite(frame);
	}
	else
	{
		SDL_Rect uppermatte = ScaledRect_to_SDLRect(Scaled_Rect{ 0, 0, 800, 55 });
		currentGUI->AddRect(GUI_Rect{ uppermatte, 0, 0, 0 });

		currentGUI->canvasRect = { 0, uppermatte.h, 800, 536 };
		//currentGUI->canvasRect = { (int)(CanvasRect.x * GlobalScale), (int)(CanvasRect.y * GlobalScale), (int)(CanvasRect.w * GlobalScale), (int)(CanvasRect.h * GlobalScale) };
		currentGUI->canvas = SDL_Texture_ptr(SDL_CreateTexture(Graphics::renderer.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, currentGUI->canvasRect.w, currentGUI->canvasRect.h));

		SDL_Rect lowermatte = ScaledRect_to_SDLRect(Scaled_Rect{ 0, 536, 800, 600 });
		currentGUI->AddRect(GUI_Rect{ lowermatte, 0, 0, 0 });
	}
}

void Loader::loadScene(int sceneName)
{
	Loader::loadScene(std::to_string(sceneName));
}

void Loader::loadScene(std::string sceneName)
{
	if (currentScene && sceneName == currentScene->sceneFile)
		ReloadScene();
	else
		_ChangeScene(sceneName);
}

std::ifstream Loader::loadTree(std::string treeName)
{
	return std::ifstream();
}

std::ifstream Loader::getDataFile(std::string sceneName)
{
	if (sceneName.empty())
		return std::ifstream();

	//TODO: get from tree not path
	//std::ifstream inFile;
	//NOTE: for testing bacause files are named the same
	/*if (oldUI)
		inFile = std::ifstream("DataFiles/CIFTREE/S" + sceneName + ".hiff", std::ios::in | std::ios::binary | std::ios::ate);
	else
		inFile = std::ifstream("Ciftree/S" + sceneName + ".hiff", std::ios::in | std::ios::binary | std::ios::ate);*/

	std::ifstream inFile = std::ifstream("DataFiles/CIFTREE/S" + sceneName + ".hiff", std::ios::in | std::ios::binary | std::ios::ate);
	if (!inFile.fail()) {
		return inFile;
	}

	inFile = std::ifstream("Ciftree/S" + sceneName + ".hiff", std::ios::in | std::ios::binary | std::ios::ate);
	if (!inFile.fail()) {
		return inFile;
	}

	LOG_F(ERROR, "Could not open HIFF file: %s", sceneName.c_str());
	return std::ifstream();
}

std::string Loader::getOVL(std::string ovlName)
{
	//TODO: get from tree not path
	if (ovlName.empty())
		return std::string();

	std::ifstream inFile = std::ifstream("DataFiles/CIFTREE/" + ovlName + ".png", std::ios::in | std::ios::binary | std::ios::ate);
	if (!inFile.fail()) {
		inFile.close();
		return "DataFiles/CIFTREE/" + ovlName + ".png";
	}

	inFile = std::ifstream("Ciftree/" + ovlName + ".png", std::ios::in | std::ios::binary | std::ios::ate);
	if (!inFile.fail()) {
		inFile.close();
		return "Ciftree/" + ovlName + ".png";
	}

	LOG_F(ERROR, "Could not open OVL file: %s", ovlName.c_str());
	return std::string();
}

//TODO: make faster? https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exists-using-standard-c-c11-14-17-c
//Note: prefers loose files
std::string Loader::getVideoPath(std::string backName)
{
	if (backName.empty())
		return std::string();

	std::ifstream inFile = std::ifstream("CDVideo/" + backName + ".png", std::ios::in | std::ios::binary | std::ios::ate);
	if (!inFile.fail()) {
		inFile.close();
		return "CDVideo/" + backName + ".png";
	}

	inFile = std::ifstream("CDVideo/" + backName + ".bik", std::ios::in | std::ios::binary | std::ios::ate);
	if (!inFile.fail()) {
		inFile.close();
		return "CDVideo/" + backName + ".bik";
	}

	inFile = std::ifstream("CDVideo/" + backName + ".avf", std::ios::in | std::ios::binary | std::ios::ate);
	if (!inFile.fail()) {
		inFile.close();
		return "CDVideo/" + backName + ".avf";
	}

	inFile = std::ifstream("HDVideo/" + backName + ".png", std::ios::in | std::ios::binary | std::ios::ate);
	if (!inFile.fail()) {
		inFile.close();
		return "HDVideo/" + backName + ".png";
	}

	inFile = std::ifstream("HDVideo/" + backName + ".bik", std::ios::in | std::ios::binary | std::ios::ate);
	if (!inFile.fail()) {
		inFile.close();
		return "HDVideo/" + backName + ".bik";
	}

	inFile = std::ifstream("HDVideo/" + backName + ".avf", std::ios::in | std::ios::binary | std::ios::ate);
	if (!inFile.fail()) {
		inFile.close();
		return "HDVideo/" + backName + ".avf";
	}

	inFile = std::ifstream("Video/" + backName + ".png", std::ios::in | std::ios::binary | std::ios::ate);
	if (!inFile.fail()) {
		inFile.close();
		return "Video/" + backName + ".png";
	}

	inFile = std::ifstream("Video/" + backName + ".bik", std::ios::in | std::ios::binary | std::ios::ate);
	if (!inFile.fail()) {
		inFile.close();
		return "Video/" + backName + ".bik";
	}

	inFile = std::ifstream("Video/" + backName + ".jpg", std::ios::in | std::ios::binary | std::ios::ate);
	if (!inFile.fail()) {
		inFile.close();
		return "Video/" + backName + ".jpg";
	}

	inFile = std::ifstream("Video/" + backName + ".avf", std::ios::in | std::ios::binary | std::ios::ate);
	if (!inFile.fail()) {
		inFile.close();
		return "Video/" + backName + ".avf";
	}

	inFile = std::ifstream("DataFiles/CIFTREE/" + backName + ".png", std::ios::in | std::ios::binary | std::ios::ate);
	if (!inFile.fail()) {
		inFile.close();
		return "DataFiles/CIFTREE/" + backName + ".png";
	}

	LOG_F(ERROR, "Cannot find background: %s", backName.c_str());
	return std::string();
}

std::string Loader::getSoundPath(std::string soundName)
{
	//TODO: handle SILENCE
	if (soundName.empty())
		return std::string();

	std::ifstream inFile("HDSound/" + soundName + ".ogg", std::ios::in | std::ios::binary | std::ios::ate);
	if (!inFile.fail()) {
		inFile.close();
		return "HDSound/" + soundName + ".ogg";
	}

	inFile = std::ifstream("CDSound/" + soundName + ".ogg", std::ios::in | std::ios::binary | std::ios::ate);
	if (!inFile.fail()) {
		inFile.close();
		return "CDSound/" + soundName + ".ogg";
	}

	inFile = std::ifstream("HDSound/" + soundName + ".wav", std::ios::in | std::ios::binary | std::ios::ate);
	if (!inFile.fail()) {
		inFile.close();
		return "HDSound/" + soundName + ".wav";
	}

	inFile = std::ifstream("CDSound/" + soundName + ".wav", std::ios::in | std::ios::binary | std::ios::ate);
	if (!inFile.fail()) {
		inFile.close();
		return "CDSound/" + soundName + ".wav";
	}

	inFile = std::ifstream("HDSound/" + soundName + ".his", std::ios::in | std::ios::binary | std::ios::ate);
	if (!inFile.fail()) {
		inFile.close();
		//LOG_F(ERROR, "HIS not supported yet!\n");
		return "HDSound/" + soundName + ".his";
		//return std::string();
	}

	inFile = std::ifstream("CDSound/" + soundName + ".his", std::ios::in | std::ios::binary | std::ios::ate);
	if (!inFile.fail()) {
		inFile.close();
		//LOG_F(ERROR, "HIS not supported yet!\n");
		return "CDSound/" + soundName + ".his";
		//return std::string();
	}

	return std::string();
}