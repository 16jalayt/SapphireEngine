#include "globals.h"
#include <loguru.hpp>

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
	//if not last byte in chunk
	while ((chunkStart + chunkLen + 8) != (int)inFile.tellg())
	{
		Dependency dep{};
		dep.depType = readShort(inFile);
		dep.label = readShort(inFile);
		dep.condition = readShort(inFile);
		dep.boolean = readShort(inFile);
		dep.time = { readShort(inFile), readShort(inFile), readShort(inFile), readShort(inFile) };
		deps.push_back(dep);
		LOG_F(INFO, "    --Dep DepType:%d Label:%d Cond:%d  Bool:%d", dep.depType, dep.label, dep.condition, dep.boolean);
	}
	return deps;
}

bool checkDeps(std::vector<Dependency> deps)
{
	if (deps.size() == 0)
		return true;
	for (const Dependency dep : deps)
	{
		//DT_EVENT
		if (dep.depType == 2)
		{
			//if should or
			if (dep.boolean)
			{
				//TODO: figure out or. All dependencies?
				if (flags[dep.label - 1000] || dep.condition)
					continue;
				else
					return false;
			}
			else
			{
				if (flags[dep.label - 1000] == dep.condition)
					continue;
				else
					return false;
			}
		}
	}

	return true;
}