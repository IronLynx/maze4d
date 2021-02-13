#pragma once

#include <glad/glad.h>
#include <shader.h>

#include <Texture.h>
#include <Maze.h>


#define WALL_BLOCK  (1 << 0)
#define LIGHT_BLOCK (1 << 1)
#define WIN_BLOCK   (1 << 2)

typedef uint8_t Cell_t;
typedef uint32_t Light_t;

typedef Cell_t* Map_t;
typedef Light_t* LightMap_t;


class Field
{
public:
	Field(const glm::ivec4 size, const int lightDist, const int roomSize);
	~Field();

	void Init(Maze* maze, Shader* shader);
	
	void CreateWinRoom();

	Cell_t GetCube(int x, int y, int z, int w);

	bool IsCubeIndexValid(int x, int y, int z, int w);

	int GetIndex(const int x, const int y, const int z, const int w);

	Map_t curMap;
	LightMap_t curLightMap;

	const int roomSize;

	void LoadMazeToGL(Shader* shader);

	glm::ivec4 size;

private:
	Shader* shader;
	void CreateCube(int x, int y, int z, int w);

	void CreateBorders();

	void CreateExit(Maze* maze);

	void GenerateWalls(Maze* maze);

	void GenerateLight(Maze* maze);

	void GenerateLightRecursive(int px, int py, int pz, int pw, unsigned int level, int side);

	
	const int totalSize;
	const int lightDist;
	int cubesCount = 0;

	Map_t map;
	LightMap_t lightMap;

	Map_t winMap;
	LightMap_t winLightMap;

	const glm::ivec4 winMapSize = glm::ivec4(9, 9, 9, 9);
};

