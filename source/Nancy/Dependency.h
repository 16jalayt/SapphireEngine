#include "globals.h"

typedef struct Dependency
{
	int depType;
	int label;
	bool condition;
	int boolean;
	Scaled_Rect time;
} Dependency;

std::vector<Dependency> parseDeps(std::ifstream& inFile, int chunkLen, int chunkStart)
{
	std::vector<Dependency> deps;
	if ((chunkStart + chunkLen + 8) != (int)inFile.tellg())
	{
		Dependency dep{};
		dep.depType = readShort(inFile);
		dep.label = readShort(inFile);
		dep.condition = readShort(inFile);
		dep.boolean = readShort(inFile);
		dep.time = { readShort(inFile), readShort(inFile), readShort(inFile), readShort(inFile) };
		deps.push_back(dep);
	}
	return deps;
}

bool checkDeps(std::vector<Dependency> deps)
{
	if (deps.size() == 0)
		return true;
	for (Dependency dep : deps)
	{
		//Todo: use condition too
		if (flags[dep.label - 1000] == dep.condition)
			continue;
		else
			return false;
	}

	return true;
}