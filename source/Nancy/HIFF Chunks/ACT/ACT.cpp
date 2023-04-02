#include "Nancy/HIFF Chunks/ACT/ACT.h"
#include <string>

#include "Engine/utils.h"
#include "Engine/Button.h"
#include "Engine/Scene.h"
#include "Nancy/HIFF.h"
#include "Nancy/Loader.h"
#include "Nancy/Dependency.h"

bool ACT::Parse(std::ifstream& inFile, int chunkLen, int chunkStart)
{
	std::string actChunkDesc = readString(inFile, 48);
	unsigned char chunkType = readByte(inFile);

	//num times to execute. single=1 multi=2
	char HSExec = readByte(inFile);

	switch (chunkType)
	{
		//12
		//scene change from flags
	case 12:
	{
		printf("Processing ACT chunk:%u Desc:%s  at:%d\n", chunkType, actChunkDesc.c_str(), chunkStart);

		//Scene to change to
		short changeTo = readShort(inFile);

		skipBytes(inFile, 18);

		std::vector<Dependency> deps = parseDeps(inFile, chunkStart, chunkLen);

		if (!checkDeps(deps))
			break;

		sceneChangeName = std::to_string(changeTo);
		sceneChangeFlag = true;

		break;
	}

	//?
	case 20:
		//Scene change with HS
	case 25:
	{
		printf("Processing ACT chunk:%u Desc:%s  at:%d\n", chunkType, actChunkDesc.c_str(), chunkStart);

		int cursorNumber = readShort(inFile);
		//TODO:set cursor

		//scene to change to
		int changeTo = readShort(inFile);

		//hot rect
		Scaled_Rect hot = Scaled_Rect{ readInt(inFile), readInt(inFile), readInt(inFile), readInt(inFile) };

		std::vector<Dependency> deps = parseDeps(inFile, chunkStart, chunkLen);

		if (!checkDeps(deps))
			break;

		Button_ptr testbutton = std::make_shared<Button>(hot, "");
		nextScene->AddHotzone(testbutton);
		testbutton->callback = [changeTo = changeTo]
		{
			//printf("lambda called\n");
			//TODO: IMPORTANT: lambda creates new thread and deletes object mid execution
			//find way to rejoin
			//set flag to change scene and have sceneproc load
			//sceneChangeName = std::to_string(changeTo);
			//sceneChangeFlag = true;
			ChangeScene(std::to_string(changeTo));
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
		printf("Processing ACT chunk:%u Desc:%s  at:%d\n", chunkType, actChunkDesc.c_str(), chunkStart);

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

		std::vector<Dependency> deps = parseDeps(inFile, chunkStart, chunkLen);

		if (!checkDeps(deps))
			break;

		Sprite_ptr ovl = std::make_shared<Sprite>(Loader::getOVL(ovlImage).c_str(), destRect.x, destRect.y, RenderParent::canvas, srcRect);
		nextScene->AddSprite(ovl);

		//Bad hack because lengh in hiff file is off by 1.
		//TODO:This is because new act chunk needs to be even addr
		//A \0 is padding
		//skipBytes(inFile, chunkLen - 49);
		break;
	}
	//Event Flags with Cursor and HS
	//AT_EVENTFLAGS_CURSOR_HS
	case 98:
	{
		//TODO: incomplete implementation
		printf("Processing ACT chunk:%u Desc:%s  at:%d\n", chunkType, actChunkDesc.c_str(), chunkStart);

		//Flag to update
		short flag = readShort(inFile);
		//truth
		int truth = readShort(inFile);

		//Cursor set to -1?

		//Need cursor value somewhere
		//Currently unknown
		skipBytes(inFile, 8);

		//one of the ones might be PUSH_SCENE=1

		//Frame hotspot is active in?
		int frame = readShort(inFile);
		Scaled_Rect hotZone = { readInt(inFile), readInt(inFile), readInt(inFile), readInt(inFile) };

		Button_ptr testbutton = std::make_shared<Button>(hotZone, "");
		nextScene->AddHotzone(testbutton);
		testbutton->callback = [flag = flag, truth = truth]
		{
			flags[flag - 1000] = truth;
			printf("set scene flag num: %d\n", flag);
			sceneChangeName = currentScene->sceneFile;
			sceneChangeFlag = true;
		};
		if (debugHot)
			testbutton->setDebug(true);

		break;
	}
	//PUSH scene onto stack
	case 110:
	{
		printf("Processing ACT chunk:%u Desc:%s  at:%d\n", chunkType, actChunkDesc.c_str(), chunkStart);

		//Unknown, status flag?
		char unknown = readByte(inFile);

		std::vector<Dependency> deps = parseDeps(inFile, chunkStart, chunkLen);

		if (!checkDeps(deps))
			break;

		//Notes on order
		//hs sets 1000
		//push and sc check dep 1000
		//Manual flag set does't have effect
		//Recheck conditions constantly?

		//Ghost dogs has 575ish EVs
		//Names hard coded into exe

		//Can't check in game. Change ev0, scene reloads and resets

		//Push current scene file name to scene stack
		prevScene = nextScene->sceneFile;

		break;
	}
	//POP scene from stack and transition
	case 111:
	{
		printf("Processing ACT chunk:%u Desc:%s  at:%d\n", chunkType, actChunkDesc.c_str(), chunkStart);

		//Unknown, status flag?
		char unknown = readByte(inFile);

		std::vector<Dependency> deps = parseDeps(inFile, chunkStart, chunkLen);

		if (!checkDeps(deps))
			break;

		//Remove current scene and change to that scene
		sceneChangeName = prevScene;
		sceneChangeFlag = true;
		prevScene = "";

		break;
	}
	//Random sound CC
	case 151:
	{
		printf("Processing ACT chunk:%u Desc:%s  at:%d\n", chunkType, actChunkDesc.c_str(), chunkStart);

		int numSounds = readShort(inFile);
		std::vector<std::string> sounds;
		for (int i = 0; i < numSounds; i++)
		{
			std::string sound = readString(inFile, 33);
			sounds.push_back(sound);
		}

		int channel = readShort(inFile);
		int loop = readInt(inFile);
		int volL = readShort(inFile);
		int volR = readShort(inFile);

		//next scene before sound ends
		bool waitOnSound = readByte(inFile);
		//Scene to change to
		int changeTo = readShort(inFile);

		int i = 0;
		//TODO: cc data
		do
		{
			//Length of closed caption data
			int length = readShort(inFile);
			skipBytes(inFile, length);
			//int unknown = readShort(inFile);
			//no idea why sometimes all cc data and sometimes one chunk
			if (length > 100)
				break;
			i++;
		} while (i < numSounds);

		//No idea
		int unknown = readShort(inFile);
		if (unknown != 0)
			skipBytes(inFile, -2);

		std::vector<Dependency> deps = parseDeps(inFile, chunkStart, chunkLen);

		if (!checkDeps(deps))
			break;

		//TODO: Play sound

		sceneChangeName = std::to_string(changeTo);
		sceneChangeFlag = true;

		break;
	}
	//Sound
	case 159:
	{
		printf("Processing ACT chunk:%u Desc:%s  at:%d\n", chunkType, actChunkDesc.c_str(), chunkStart);

		std::string sound = readString(inFile, 33);

		int channel = readShort(inFile);
		int loop = readInt(inFile);
		int volL = readShort(inFile);
		int volR = readShort(inFile);

		//next scene before sound ends
		bool waitOnSound = readByte(inFile);
		//Scene to change to
		int changeTo = readShort(inFile);

		//Length of closed caption data
		int length = readShort(inFile);
		skipBytes(inFile, length);

		std::vector<Dependency> deps = parseDeps(inFile, chunkStart, chunkLen);

		if (!checkDeps(deps))
			break;

		//TODO: Play sound

		sceneChangeName = std::to_string(changeTo);
		sceneChangeFlag = true;

		break;
	}

	//Mouselight puzzle (Ghost dogs tunnel flashlight OVL)
	case 217:
	{
		printf("Processing ACT chunk:%u Desc:%s  at:%d\n", chunkType, actChunkDesc.c_str(), chunkStart);

		//Name of the cave frame to show
		//Cave background is an OVL over top of a black background
		//So only shows up when flashlight cursor selected
		std::string ovlImage = readString(inFile, 33);

		skipBytes(inFile, 18);

		std::vector<Dependency> deps = parseDeps(inFile, chunkStart, chunkLen);

		if (!checkDeps(deps))
			break;

		Sprite_ptr ovl = std::make_shared<Sprite>(Loader::getOVL(ovlImage).c_str(), 0, 0);
		nextScene->AddSprite(ovl);

		Sprite_ptr mask = std::make_shared<Sprite>("data/lightmask.png", 0, 0);
		nextScene->AddSprite(mask);
		mask->isMask(true);

		break;
	}
	default:
	{
		printf("**Invalid ACT chunk:%u Desc:%s in scene:%s  at:%d\n", chunkType, actChunkDesc.c_str(), nextScene->sceneFile.c_str(), chunkStart);
		//int test = chunkLen - 49;
		skipBytes(inFile, chunkLen - 50);
		break;
	}
	}
	return true;
}