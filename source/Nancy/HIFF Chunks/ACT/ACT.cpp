#include <Nancy/HIFF Chunks/ACT/ACT.h>
#include <string>
#include <Engine/utils.h>
#include <Engine/Button.h>
#include <Nancy/HIFF.h>
#include <Engine/Scene.h>
#include <Nancy/Loader.h>

bool ACT::Parse(std::ifstream& inFile, Scene_ptr& scene, int chunkLen)
{
	std::string actChunkDesc = readString(inFile, 48);
	unsigned char chunkType = readByte(inFile);

	//num times to execute. single=1 multi=2
	char HSExec = readByte(inFile);

	switch (chunkType)
	{
		//Scene change
		//TODO:ghostdogs name
	case 25:
	{
		//TODO: Needed?
		if (chunkType == 25)
			int cursorNumber = readShort(inFile);
		//TODO:set cursor

		//scene to change to
		int changeTo = readShort(inFile);

		//hot rect
		Scaled_Rect hot = Scaled_Rect{ readInt(inFile), readInt(inFile), readInt(inFile), readInt(inFile) };

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
		//52 in newer games
		//printf("static overlay not implemented\n");

		std::string ovlImage = readString(inFile, 33);

		//TODO:parse deps func
		//?might not have deps like newer

		//?number of pixel copies from file
		int unknown1 = readShort(inFile);
		int zOrder = readShort(inFile);

		Scaled_Rect destRect = { readInt(inFile), readInt(inFile), readInt(inFile), readInt(inFile) };
		Scaled_Rect srcRect = { readInt(inFile), readInt(inFile), readInt(inFile), readInt(inFile) };

		//?
		int depType = readShort(inFile);
		int depNumber = readShort(inFile);
		//?Truth
		int unknown5 = readByte(inFile);

		skipBytes(inFile, 12);

		Sprite_ptr ovl = std::make_shared<Sprite>(Loader::getOVL(ovlImage).c_str(), destRect.x, destRect.y, RenderParent::canvas, srcRect);
		scene->AddSprite(ovl);

		//Bad hack because lengh in hiff file is off by 1.
		//TODO:This is because new act chunk needs to be even addr
		//A \0 is padding
		//skipBytes(inFile, chunkLen - 49);
		break;
	}
	default:
	{
		printf("Invalid ACT chunk:%u Desc:%s in scene:%s  at:%ld\n", chunkType, actChunkDesc.c_str(), scene->sceneFile.c_str(), (long)inFile.tellg() - 57);
		//int test = chunkLen - 49;
		skipBytes(inFile, chunkLen - 50);
		break;
	}
	}
	return true;
}