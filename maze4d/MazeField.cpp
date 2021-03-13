#pragma once

#include <MazeField.h>

//const glm::ivec4 MazeField::winMapSize = glm::ivec4(9, 9, 9, 9);

void MazeField::GenerateField(const glm::ivec4 size, const int lightDist, const int roomSize, Config* cfg)
{
	if (field != nullptr)
		delete field;

	this->roomSize = roomSize;
	field = new Field(size, lightDist, cfg);

	field->CreateBorders();
	CreateExit(maze);
	GenerateWalls(maze);
	GenerateLight(maze);

	field->LoadFieldToGL();
}

void MazeField::CreateExit(Maze* maze)
{
	Random* rnd = Random::GetInstance();

	auto createExit = [&](int edge)
	{
		glm::ivec4 start = glm::ivec4(
			(maze->size.x - 1)*roomSize + 1,
			(maze->size.y - 1)*roomSize + 1,
			(maze->size.z - 1)*roomSize + 1,
			(maze->size.w - 1)*roomSize + 1
		);
		glm::ivec4 end = glm::ivec4(
			(maze->size.x)*roomSize,
			(maze->size.y)*roomSize,
			(maze->size.z)*roomSize,
			(maze->size.w)*roomSize
		);
		switch (edge)
		{
		case 0: start.x = (maze->size.x)*roomSize; end.x = (maze->size.x)*roomSize + 1; break;
		case 1: start.y = (maze->size.y)*roomSize; end.y = (maze->size.y)*roomSize + 1; break;
		case 2: start.z = (maze->size.z)*roomSize; end.z = (maze->size.z)*roomSize + 1; break;
		case 3: start.w = (maze->size.w)*roomSize; end.w = (maze->size.w)*roomSize + 1; break;
		default: break;
		}
		for (int x = start.x; x < end.x; x++)
		{
			for (int y = start.y; y < end.y; y++)
			{
				for (int z = start.z; z < end.z; z++)
				{
					for (int w = start.w; w < end.w; w++)
					{
						field->CreateCube(x, y, z, w, Cell(0, 0, true));
						/*
						int index = field->GetIndex(x, y, z, w);
						//curMap[index] = WIN_BLOCK;
						field->curMap[index]->isWinBlock = true;
						field->curMap[index]->cellType = 0;
						//field->cubesCount--;
						*/
					}
				}
			}
		}
	};

	std::vector<int> edgesForExit;
	if (maze->size.x > 1) edgesForExit.push_back(0);
	if (maze->size.y > 1) edgesForExit.push_back(1);
	if (maze->size.z > 1) edgesForExit.push_back(2);
	if (maze->size.w > 1) edgesForExit.push_back(3);
	if (edgesForExit.size() > 0)
		createExit(edgesForExit[rnd->GetInt(0, edgesForExit.size() - 1)]);
}

void MazeField::GenerateWalls(Maze* maze)
{
	auto createShape = [&](glm::ivec4 start, glm::ivec4 end)
	{
		end.x = glm::min(end.x, field->size.x - 1);
		end.y = glm::min(end.y, field->size.y - 1);
		end.z = glm::min(end.z, field->size.z - 1);
		end.w = glm::min(end.w, field->size.w - 1);

		for (int x = start.x; x < end.x; x++)
		{
			for (int y = start.y; y < end.y; y++)
			{
				for (int z = start.z; z < end.z; z++)
				{
					for (int w = start.w; w < end.w; w++)
					{
						field->CreateCube(x, y, z, w);
					}
				}
			}
		}
	};


	for (int x = 0; x < maze->size.x; x++)
	{
		for (int y = 0; y < maze->size.y; y++)
		{
			for (int z = 0; z < maze->size.z; z++)
			{
				for (int w = 0; w < maze->size.w; w++)
				{
					glm::ivec4 step = glm::ivec4(
						field->size.x / maze->size.x,
						field->size.y / maze->size.y,
						field->size.z / maze->size.z,
						field->size.w / maze->size.w);

					// Create walls (only on negative edges)
					if (maze->IsWallExist(NEG_X, glm::ivec4(x, y, z, w)))
					{
						createShape(glm::ivec4(x*step.x, y*step.y, z*step.z, w*step.w),
							glm::ivec4(x*step.x + 1, (y + 1)*step.y, (z + 1)*step.z, (w + 1)*step.w));
					}
					if (maze->IsWallExist(NEG_Y, glm::ivec4(x, y, z, w)))
					{
						createShape(glm::ivec4(x*step.x, y*step.y, z*step.z, w*step.w),
							glm::ivec4((x + 1)*step.x, y*step.y + 1, (z + 1)*step.z, (w + 1)*step.w));
					}
					if (maze->IsWallExist(NEG_Z, glm::ivec4(x, y, z, w)))
					{
						createShape(glm::ivec4(x*step.x, y*step.y, z*step.z, w*step.w),
							glm::ivec4((x + 1)*step.x, (y + 1)*step.y, z*step.z + 1, (w + 1)*step.w));
					}
					if (maze->IsWallExist(NEG_W, glm::ivec4(x, y, z, w)))
					{
						createShape(glm::ivec4(x*step.x, y*step.y, z*step.z, w*step.w),
							glm::ivec4((x + 1)*step.x, (y + 1)*step.y, (z + 1)*step.z, w*step.w + 1));
					}

					// Create corners (check positive edges in current room and create corners
					// on negative edges in adjoining rooms)
					if (maze->IsWallExist(POS_X, glm::ivec4(x, y, z, w)) && maze->IsWallExist(POS_Y, glm::ivec4(x, y, z, w)))
					{
						createShape(glm::ivec4((x + 1)*step.x, (y + 1)*step.y, z*step.z, w*step.w),
							glm::ivec4((x + 1)*step.x + 1, (y + 1)*step.y + 1, (z + 1)*step.z, (w + 1)*step.w));
					}
					if (maze->IsWallExist(POS_X, glm::ivec4(x, y, z, w)) && maze->IsWallExist(POS_Z, glm::ivec4(x, y, z, w)))
					{
						createShape(glm::ivec4((x + 1)*step.x, y*step.y, (z + 1)*step.z, w*step.w),
							glm::ivec4((x + 1)*step.x + 1, (y + 1)*step.y, (z + 1)*step.z + 1, (w + 1)*step.w));
					}
					if (maze->IsWallExist(POS_X, glm::ivec4(x, y, z, w)) && maze->IsWallExist(POS_W, glm::ivec4(x, y, z, w)))
					{
						createShape(glm::ivec4((x + 1)*step.x, y*step.y, z*step.z, (w + 1)*step.w),
							glm::ivec4((x + 1)*step.x + 1, (y + 1)*step.y, (z + 1)*step.z, (w + 1)*step.w + 1));
					}
					if (maze->IsWallExist(POS_Y, glm::ivec4(x, y, z, w)) && maze->IsWallExist(POS_Z, glm::ivec4(x, y, z, w)))
					{
						createShape(glm::ivec4(x*step.x, (y + 1)*step.y, (z + 1)*step.z, w*step.w),
							glm::ivec4((x + 1)*step.x, (y + 1)*step.y + 1, (z + 1)*step.z + 1, (w + 1)*step.w));
					}
					if (maze->IsWallExist(POS_Y, glm::ivec4(x, y, z, w)) && maze->IsWallExist(POS_W, glm::ivec4(x, y, z, w)))
					{
						createShape(glm::ivec4(x*step.x, (y + 1)*step.y, z*step.z, (w + 1)*step.w),
							glm::ivec4((x + 1)*step.x, (y + 1)*step.y + 1, (z + 1)*step.z, (w + 1)*step.w + 1));
					}
					if (maze->IsWallExist(POS_Z, glm::ivec4(x, y, z, w)) && maze->IsWallExist(POS_W, glm::ivec4(x, y, z, w)))
					{
						createShape(glm::ivec4(x*step.x, y*step.y, (z + 1)*step.z, (w + 1)*step.w),
							glm::ivec4((x + 1)*step.x, (y + 1)*step.y, (z + 1)*step.z + 1, (w + 1)*step.w + 1));
					}
					if (maze->IsWallExist(POS_X, glm::ivec4(x, y, z, w)) && maze->IsWallExist(POS_Y, glm::ivec4(x, y, z, w)) &&
						maze->IsWallExist(POS_Z, glm::ivec4(x, y, z, w)))
					{
						createShape(glm::ivec4((x + 1)*step.x, (y + 1)*step.y, (z + 1)*step.z, w*step.w),
							glm::ivec4((x + 1)*step.x + 1, (y + 1)*step.y + 1, (z + 1)*step.z + 1, (w + 1)*step.w));
					}
					if (maze->IsWallExist(POS_X, glm::ivec4(x, y, z, w)) && maze->IsWallExist(POS_Y, glm::ivec4(x, y, z, w)) &&
						maze->IsWallExist(POS_W, glm::ivec4(x, y, z, w)))
					{
						createShape(glm::ivec4((x + 1)*step.x, (y + 1)*step.y, z*step.z, (w + 1)*step.w),
							glm::ivec4((x + 1)*step.x + 1, (y + 1)*step.y + 1, (z + 1)*step.z, (w + 1)*step.w + 1));
					}
					if (maze->IsWallExist(POS_X, glm::ivec4(x, y, z, w)) && maze->IsWallExist(POS_Z, glm::ivec4(x, y, z, w)) &&
						maze->IsWallExist(POS_W, glm::ivec4(x, y, z, w)))
					{
						createShape(glm::ivec4((x + 1)*step.x, y*step.y, (z + 1)*step.z, (w + 1)*step.w),
							glm::ivec4((x + 1)*step.x + 1, (y + 1)*step.y, (z + 1)*step.z + 1, (w + 1)*step.w + 1));
					}
					if (maze->IsWallExist(POS_Y, glm::ivec4(x, y, z, w)) && maze->IsWallExist(POS_Z, glm::ivec4(x, y, z, w)) &&
						maze->IsWallExist(POS_W, glm::ivec4(x, y, z, w)))
					{
						createShape(glm::ivec4(x*step.x, (y + 1)*step.y, (z + 1)*step.z, (w + 1)*step.w),
							glm::ivec4((x + 1)*step.x, (y + 1)*step.y + 1, (z + 1)*step.z + 1, (w + 1)*step.w + 1));
					}
					if (maze->IsWallExist(POS_X, glm::ivec4(x, y, z, w)) && maze->IsWallExist(POS_Y, glm::ivec4(x, y, z, w)) &&
						maze->IsWallExist(POS_Z, glm::ivec4(x, y, z, w)) && maze->IsWallExist(POS_W, glm::ivec4(x, y, z, w)))
					{
						createShape(glm::ivec4((x + 1)*step.x, (y + 1)*step.y, (z + 1)*step.z, (w + 1)*step.w),
							glm::ivec4((x + 1)*step.x + 1, (y + 1)*step.y + 1, (z + 1)*step.z + 1, (w + 1)*step.w + 1));
					}
				}
			}
		}
	}
}

void MazeField::GenerateLight(Maze* maze)
{
	Random* rnd = Random::GetInstance();

	std::vector<int> tmpWalls;
	for (int x = 0; x < maze->size.x; x++)
	{
		for (int y = 0; y < maze->size.y; y++)
		{
			for (int z = 0; z < maze->size.z; z++)
			{
				for (int w = 0; w < maze->size.w; w++)
				{
					for (int e = 0; e < EDGES_COUNT; e++)
					{
						if ((e == POS_X || e == NEG_X) && maze->size.x == 1 ||
							(e == POS_Y || e == NEG_Y) && maze->size.y == 1 ||
							(e == POS_Z || e == NEG_Z) && maze->size.z == 1 ||
							(e == POS_W || e == NEG_W) && maze->size.w == 1) continue;
						if (maze->IsWallExist(e, glm::ivec4(x, y, z, w)))
						{
							tmpWalls.push_back(e);
						}
					}

					if (tmpWalls.size() == 0)
					{
						glm::ivec4 light = glm::ivec4(
							x*roomSize + roomSize / 2,
							y*roomSize + roomSize / 2,
							z*roomSize + roomSize / 2,
							w*roomSize + roomSize / 2);
						field->CreateCube(light, Field::StdLightCell);
					}
					else
					{
						int randomEdgeIndex = rnd->GetInt(0, tmpWalls.size() - 1);
						glm::ivec4 light(
							x*roomSize + rnd->GetInt(1, roomSize - 1),
							y*roomSize + rnd->GetInt(1, roomSize - 1),
							z*roomSize + rnd->GetInt(1, roomSize - 1),
							w*roomSize + rnd->GetInt(1, roomSize - 1));

						switch (tmpWalls[randomEdgeIndex])
						{
						case NEG_X: light.x = x*roomSize + 1; break;
						case POS_X: light.x = (x + 1)*roomSize - 1; break;
						case NEG_Y: light.y = y*roomSize + 1; break;
						case POS_Y: light.y = (y + 1)*roomSize - 1; break;
						case NEG_Z: light.z = z*roomSize + 1; break;
						case POS_Z: light.z = (z + 1)*roomSize - 1; break;
						case NEG_W: light.w = w*roomSize + 1; break;
						case POS_W: light.w = (w + 1)*roomSize - 1; break;
						default:
							break;
						}

						if (maze->size.x == 1) light.x = roomSize / 2;
						if (maze->size.y == 1) light.y = roomSize / 2;
						if (maze->size.z == 1) light.z = roomSize / 2;
						if (maze->size.w == 1) light.w = roomSize / 2;

						field->CreateCube(light,  Field::StdLightCell);
					}

					tmpWalls.clear();
				}
			}
		}
	}

	for (int x = 0; x < field->size.x; x++)
	{
		for (int y = 0; y < field->size.y; y++)
		{
			for (int z = 0; z < field->size.z; z++)
			{
				for (int w = 0; w < field->size.w; w++)
				{
					for (int e = 0; e < EDGES_COUNT; e++)
					{
						int level = 15;
						unsigned int scaledLevel = 10;

						int index = field->GetIndex(x, y, z, w);
						if (index == 0)
							int b = 0;
						bool isMapEdge = false;
						if (x == 0 && e == NEG_X) isMapEdge = true;
						if (y == 0 && e == NEG_Y) isMapEdge = true;
						if (z == 0 && e == NEG_Z) isMapEdge = true;
						if (w == 0 && e == NEG_W) isMapEdge = true;
						if (x == field->size.x - 1 && e == POS_X) isMapEdge = true;
						if (y == field->size.y - 1 && e == POS_Y) isMapEdge = true;
						if (z == field->size.z - 1 && e == POS_Z) isMapEdge = true;
						if (w == field->size.w - 1 && e == POS_W) isMapEdge = true;

						if (isMapEdge)
						{
							// clear light
							//field->curLightMap[index] &= ~((Texture::LIGHT_GRAD - 1) << (e * 4));
							// set light
							//field->curLightMap[index] |= scaledLevel << (e * 4);
						}

					}
				}
			}
		}
	}
	int i = 1;
}

