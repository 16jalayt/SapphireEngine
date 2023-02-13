#include <Nancy/HIFF Chunks/ACT/ACT.h>
#include <string>
#include <Engine/utils.h>
#include <Engine/Button.h>
#include <Nancy/HIFF.h>
#include <Engine/Scene.h>

bool ACT::Parse(std::ifstream& inFile, Scene_ptr& scene, int chunkLen)
{
	std::string actChunkDesc = readString(inFile, 48);
	unsigned char chunkType = readByte(inFile);

	switch (chunkType)
	{
		//Scene change
		//TODO:ghostdogs name
	case 25:
	{
		//num times to execute. single=1 multi=2
		char HSExec = readByte(inFile);

		if (chunkType == 25)
			int cursorNumber = readShort(inFile);
		//TODO:set cursor

		//scene to change to
		int changeTo = readShort(inFile);

		//hot rect
		Scaled_Rect hot = { readInt(inFile), readInt(inFile), readInt(inFile), readInt(inFile) };

		Button_ptr testbutton = std::make_shared<Button>(hot, "", RenderParent::canvas);
		scene->AddHotzone(testbutton);
		testbutton->callback = [changeTo = changeTo]
		{
			//printf("lambda called\n");
			//TODO: IMPORTANT: lambda creates new thread and deletes object mid execution
			//find way to rejoin
			//set flag to change scene and have sceneproc load
			std::string sceneName = std::to_string(changeTo);
			sceneChangeName = sceneName;
			sceneChangeFlag = true;
			//HIFF::Load_HIFF(string.c_str());
		};
		if (debugHot)
			testbutton->setDebug(true);
		//testbutton->visible(false);
		break;
	}
	//Scene Change with Frame
	/*case 16:
	{
		break;
	}*/
	//sfx
	/*case 158:
	{
		//num times to execute. single=1 multi=2
		char HSExec = readByte(inFile);

		int numSFX = readShort(inFile);
		std::vector<std::string> sfxNames;
		for (int i = 0; i < numSFX; i++)
		{
			sfxNames.push_back(readString(inFile, 33));
		}

		int channel = readShort(inFile);
		int channel = readShort(inFile);
		int channel = readShort(inFile);

		//Audio levels
		int chan1 = readShort(inFile);
		int chan2 = readShort(inFile);

		//int?
		//int refScene

		//TODO:create jukebox like functionality in sound player

		break;
	}*/
	/*case 108:
	{
		printf("clear save not implemented\n");
		//Bad hack because lengh in hiff file is off by 1. HER's fault.
		skipBytes(inFile, chunkLen - 49);
		break;
	}*/
	/*case 211:
	{
		printf("Scopa card game not implemented\n");
		//Bad hack because lengh in hiff file is off by 1. HER's fault.
		skipBytes(inFile, chunkLen - 49);
		break;
	}*/
	//static overlay
	case 55:
	{
		printf("static overlay not implemented\n");
		//Bad hack because lengh in hiff file is off by 1. HER's fault.
		skipBytes(inFile, chunkLen - 49);
		break;
	}
	default:
	{
		printf("Invalid ACT chunk:%u Desc:%s in scene:%s  at:%ld\n", chunkType, actChunkDesc.c_str(), scene->sceneFile.c_str(), (long)inFile.tellg() - 57);
		//int test = chunkLen - 49;
		skipBytes(inFile, chunkLen - 49);
		break;
	}
	}
	return true;
}