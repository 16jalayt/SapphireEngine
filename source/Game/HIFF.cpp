#include "Nancy/HIFF.h"
#include "Engine/Scene.h"
#include "Loader.h"
#include "HIFFChunks/ACT/ACT.h"
#include "HIFFChunks/SUM.h"
#include <loguru.hpp>

bool HIFF::Load_HIFF(std::string sceneName)
{
	//sceneNum = std::stoi(sceneName);
	sceneChangeName = sceneName;

	std::ifstream inFile = Loader::getDataFile(sceneName);

	//Starts at end of file because | std::ios::ate
	long filesize = (long)inFile.tellg();
	//infile.fail() doesn't seem to work
	if (filesize == -1)
		return false;

	inFile.seekg(0);

	std::string magic = readString(inFile, 4);

	if (magic != "DATA")
	{
		LOG_F(ERROR, "Missing Data chunk in file: %s", sceneName.c_str());
		return false;
	}

	int chunkLenInt = readInt(inFile, true);
	//fails on newer games with flagevnt
	/*if (chunkLenInt != (filesize - 8))
	{
		LOG_F(ERROR, "Invalid data chunk length in file: %s\n", sceneName.c_str());
		return false;
	}*/

	std::string subChunk = readString(inFile, 8);
	if (subChunk == "FLAGEVNT")
	{
		//Always 0?
		int flagevnt = readInt(inFile);

		std::string magic = readString(inFile, 4);

		if (magic != "DATA")
		{
			LOG_F(ERROR, "Missing Data chunk in file: %s", sceneName.c_str());
			return false;
		}

		int chunkLenInt = readInt(inFile, true);

		subChunk = readString(inFile, 8);
	}
	if (subChunk == "SCENTSUM")
	{
		int sumLength = readInt(inFile, true);
		SUM::Parse(inFile, sceneName);
	}
	/*if (sumChunk == "SCENSUM")
	{
		LOG_F(ERROR, "Invalid Summery chunk in file: %s\n", sceneName.c_str());
		return false;
	}*/
	else
	{
		LOG_F(ERROR, "Invalid data chunk:%s in file:%s", subChunk.c_str(), sceneName.c_str());
		return false;
	}

	nextScene->sceneFile = sceneName;

	/////////////////////////////////////////////////

	while (inFile.tellg() != filesize && inFile.tellg() != EOF)
	{
		long chunkStart = (long)inFile.tellg();
		std::string actChunk = readString(inFile, 4);
		int chunkLen = readInt(inFile, true);

		if (actChunk == "ACT")
		{
			ACT::Parse(inFile, chunkLen, chunkStart);
		}
		else
		{
			LOG_F(ERROR, "**Invalid chunk:%s in scene: %s  at: %ld", actChunk.c_str(), sceneName.c_str(), chunkStart);
			skipBytes(inFile, chunkLen);
		}

		//Make sure next chunk is byte aligned
		if (((int)inFile.tellg() % 2) != 0)
			skipBytes(inFile, 1);
	}

	//ChangeScene(newScene);

	inFile.close();
	return true;
}