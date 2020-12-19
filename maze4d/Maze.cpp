#include <Maze.h>

Maze::Maze(const glm::ivec4 size) : size(size)
{
	rooms = new Room[size.x*size.y*size.z*size.w];
}

Maze::~Maze()
{
	delete[] rooms;
}

bool Maze::IsWallExist(int edge, glm::ivec4 pos)
{
	assert(IsRoomIndexValid(pos.x, pos.y, pos.z, pos.w));
	return rooms[GetIndex(pos.x, pos.y, pos.z, pos.w)].walls[edge];
}

void Maze::Generate()
{
	// Заполняем лабиринт стенами
	for (int x = 0; x < size.x; x++)
	{
		for (int y = 0; y < size.y; y++)
		{
			for (int z = 0; z < size.z; z++)
			{
				for (int w = 0; w < size.w; w++)
				{
					int index = GetIndex(x, y, z, w);
					for (int i = 0; i < EDGES_COUNT; i++)
						rooms[index].walls[i] = true;
					rooms[index].visited = false;
				}
			}
		}
	}

	rooms[GetIndex(size.x - 1, size.y - 1, size.z - 1, size.w - 1)].visited = true;
	int unvisitedCells = size.x*size.y*size.z*size.w - 1;
	int curRoomX = size.x - 1, curRoomY = size.y - 1, curRoomZ = size.z - 1, curRoomW = size.w - 1;
	std::vector<int> stackX, stackY, stackZ, stackW;

	Random* rnd = Random::GetInstance();

	while (unvisitedCells > 0)
	{
		std::vector<int> curNeighbours = FindUnvisitedNeighbours(curRoomX, curRoomY, curRoomZ, curRoomW);
		//Если есть непосещённые соседи
		if (curNeighbours.size() != 0)
		{
			int randNeighbour = rnd->GetInt(0, curNeighbours.size() - 1);
			stackX.push_back(curRoomX);
			stackY.push_back(curRoomY);
			stackZ.push_back(curRoomZ);
			stackW.push_back(curRoomW);
			switch (curNeighbours[randNeighbour])
			{
			case NEG_X:
				rooms[GetIndex(curRoomX, curRoomY, curRoomZ, curRoomW)].walls[NEG_X] = false;
				curRoomX--;
				rooms[GetIndex(curRoomX, curRoomY, curRoomZ, curRoomW)].walls[POS_X] = false;
				break;
			case POS_X:
				rooms[GetIndex(curRoomX, curRoomY, curRoomZ, curRoomW)].walls[POS_X] = false;
				curRoomX++;
				rooms[GetIndex(curRoomX, curRoomY, curRoomZ, curRoomW)].walls[NEG_X] = false;
				break;
			case NEG_Y:
				rooms[GetIndex(curRoomX, curRoomY, curRoomZ, curRoomW)].walls[NEG_Y] = false;
				curRoomY--;
				rooms[GetIndex(curRoomX, curRoomY, curRoomZ, curRoomW)].walls[POS_Y] = false;
				break;
			case POS_Y:
				rooms[GetIndex(curRoomX, curRoomY, curRoomZ, curRoomW)].walls[POS_Y] = false;
				curRoomY++;
				rooms[GetIndex(curRoomX, curRoomY, curRoomZ, curRoomW)].walls[NEG_Y] = false;
				break;
			case NEG_Z:
				rooms[GetIndex(curRoomX, curRoomY, curRoomZ, curRoomW)].walls[NEG_Z] = false;
				curRoomZ--;
				rooms[GetIndex(curRoomX, curRoomY, curRoomZ, curRoomW)].walls[POS_Z] = false;
				break;
			case POS_Z:
				rooms[GetIndex(curRoomX, curRoomY, curRoomZ, curRoomW)].walls[POS_Z] = false;
				curRoomZ++;
				rooms[GetIndex(curRoomX, curRoomY, curRoomZ, curRoomW)].walls[NEG_Z] = false;
				break;
			case NEG_W:
				rooms[GetIndex(curRoomX, curRoomY, curRoomZ, curRoomW)].walls[NEG_W] = false;
				curRoomW--;
				rooms[GetIndex(curRoomX, curRoomY, curRoomZ, curRoomW)].walls[POS_W] = false;
				break;
			case POS_W:
				rooms[GetIndex(curRoomX, curRoomY, curRoomZ, curRoomW)].walls[POS_W] = false;
				curRoomW++;
				rooms[GetIndex(curRoomX, curRoomY, curRoomZ, curRoomW)].walls[NEG_W] = false;
				break;
			default:
				break;
			}
			rooms[GetIndex(curRoomX, curRoomY, curRoomZ, curRoomW)].visited = true;
			unvisitedCells--;
		}
		else if (stackX.size() != 0 && stackY.size() != 0 && stackZ.size() != 0 && stackW.size() != 0)
		{
			curRoomX = stackX[stackX.size() - 1];
			stackX.pop_back();
			curRoomY = stackY[stackY.size() - 1];
			stackY.pop_back();
			curRoomZ = stackZ[stackZ.size() - 1];
			stackZ.pop_back();
			curRoomW = stackW[stackW.size() - 1];
			stackW.pop_back();
		}
	}
}

std::vector<int> Maze::FindUnvisitedNeighbours(int curX, int curY, int curZ, int curW)
{
	std::vector<int> neighbours;

	if (curX > 0            && !rooms[GetIndex(curX - 1, curY, curZ, curW)].visited) { neighbours.push_back(NEG_X); }
	if (curX < (size.x - 1) && !rooms[GetIndex(curX + 1, curY, curZ, curW)].visited) { neighbours.push_back(POS_X); }

	if (curY > 0            && !rooms[GetIndex(curX, curY - 1, curZ, curW)].visited) { neighbours.push_back(NEG_Y); }
	if (curY < (size.y - 1) && !rooms[GetIndex(curX, curY + 1, curZ, curW)].visited) { neighbours.push_back(POS_Y); }

	if (curZ > 0            && !rooms[GetIndex(curX, curY, curZ - 1, curW)].visited) { neighbours.push_back(NEG_Z); }
	if (curZ < (size.z - 1) && !rooms[GetIndex(curX, curY, curZ + 1, curW)].visited) { neighbours.push_back(POS_Z); }

	if (curW > 0            && !rooms[GetIndex(curX, curY, curZ, curW - 1)].visited) { neighbours.push_back(NEG_W); }
	if (curW < (size.w - 1) && !rooms[GetIndex(curX, curY, curZ, curW + 1)].visited) { neighbours.push_back(POS_W); }

	return neighbours;
}

int Maze::GetIndex(const int x, const int y, const int z, const int w)
{
	return x*size.y*size.z*size.w + y*size.z*size.w + z*size.w + w;
};

bool Maze::IsRoomIndexValid(int x, int y, int z, int w)
{
	return (
		x >= 0 && x < size.x &&
		y >= 0 && y < size.y &&
		z >= 0 && z < size.z &&
		w >= 0 && w < size.w);
};