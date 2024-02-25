#include <Nancy/HIFFChunks/SUM.h>
#include <string>
#include <Engine/utils.h>
#include <Engine/Button.h>
#include <Nancy/HIFF.h>
#include <Engine/Scene.h>

bool SUM::Parse(std::ifstream& inFile, std::string sceneName)
{
	//Scene Description
	std::string sceneDesc = readString(inFile, 50);
	//scene->sceneName = sceneDesc;

	nextScene->SetHeader(sceneDesc, sceneName);

	//printf("Loading scene: %s\n", sceneDesc.c_str());

	//The name of the scene background.
	//Same length as file name field in cifftree?
	std::string sceneBack = readString(inFile, 33);
	nextScene->setBkg(sceneBack);

	//Ambient sound file
	std::string sound = readString(inFile, 33);

	//Scene specific sound channel
	int channel = readShort(inFile);

	//Loop. 0 infinite
	int loop = readInt(inFile);

	//Audio levels
	int chan1 = readShort(inFile);
	int chan2 = readShort(inFile);

	nextScene->AddMusic(sound, channel, loop, chan1, chan2);

	return true;
}