#pragma once

#include <Utils.h>

class Maze
{
public:
	Maze(const glm::ivec4 size);
	~Maze();

	bool IsWallExist(int edge, glm::ivec4 pos);

	void Generate();

	const glm::ivec4 size;

private:
	std::vector<int> FindUnvisitedNeighbours(int curX, int curY, int curZ, int curW);

	int GetIndex(const int x, const int y, const int z, const int w);

	bool IsRoomIndexValid(int x, int y, int z, int w);

	struct Room
	{
		bool walls[EDGES_COUNT];
		bool visited;
	};
	Room* rooms = nullptr;
};
