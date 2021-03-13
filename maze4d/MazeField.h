#pragma once


#include <Field.h>
#include <Maze.h>
#include <Config.h>

class Field;

class MazeField
{
public:
	void GenerateMaze(const glm::ivec4 size)
	{
		if (maze != nullptr)
			delete maze;

		maze = new Maze(size);
		maze->Generate();
	}

	~MazeField()
	{
		if (maze != nullptr)
			delete maze;
	}

	//Allocates with NEW function, needs to be deleted manually once done
	//static Field* CreateWinRoom(Field* const field);

	//Allocates with NEW function, needs to be deleted manually once done
	void GenerateField(const glm::ivec4 size, const int lightDist, const int roomSize, Config* cfg);

	int roomSize;

	Field* field = nullptr;
private:

	void CreateExit(Maze* maze);

	void GenerateWalls(Maze* maze);

	void GenerateLight(Maze* maze);

	
	Maze* maze = nullptr;
//	static const glm::ivec4 winMapSize;
	
};
