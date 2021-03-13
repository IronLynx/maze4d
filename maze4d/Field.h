#pragma once

#include <glad/glad.h>
#include <shader.h>

#include <Texture.h>
#include <Maze.h>
#include <Player.h>
#include <Cube.h>
#include <Cell.h>
#include <Config.h>
#include <GameGraphics.h>
#include <RaycastingShader.h>
#include <Utils.h>

#define EMPTY_BLOCK 0
#define WALL_BLOCK 1
#define LIGHT_BLOCK 2
#define WIN_BLOCK 3


typedef uint32_t Light_t;
typedef Light_t* LightMap_t;

typedef Cell* Cell_t;
typedef Cell_t* Map_t;

class Player;
class Cell;
class RaycastingShader;

class Field : public IDrawable
{
public:
	Field(const glm::ivec4 size, const int lightDist, Config* cfg);
	~Field();

	//some video configurations
	void Init(Config* cfg, float posX, float posY, float width, float height);
	void NewField(const glm::ivec4 size, const int lightDist);

	//Proxy to FragmentShader
	void SetFrameSize(float bottomX, float bottomY, float width, float height);
	void SetResolution(int viewWidth, int viewHeight);
	void SetMaxRayOutDistance(int distance = 0);
	void SetBackgroundColor(glm::vec4  col = glm::vec4(1.0f, 0.5f, 0.5f, 1.0f));
	void SetBackgroundColor(float R, float G, float B, float Alpha = 1.0f);
	void SetCameraView(Player* player);
	void Field::SetAntialiasing(int antiAliasRate = 1);
	void ReadConfig(Config* cfg);

	virtual void Draw();
	void Draw(Player* player);

	//Shader (Fragment Raycasting) control functions
	
	void LoadFieldToGL();
	bool isTextureValid = false;

	bool IsCubeIndexValid(int x, int y, int z, int w);

	int GetIndex(const int x, const int y, const int z, const int w);
	int GetIndex(const glm::ivec4 pos);

	//Map_t curMap;
	static const Cell StdLightCell;
	
	//Editor's instumenrs
	void CreateBorders();

	//setters for curMap
	void CreateCube(int x, int y, int z, int w, Cell_t cell = new Cell(WALL_BLOCK, 255, false));
	void CreateCube(glm::ivec4 pos, Cell_t cell = new Cell(WALL_BLOCK, 255, false)) { CreateCube(pos.x, pos.y, pos.z, pos.w, cell); }
	void CreateCube(int Idx, Cell_t cell = new Cell(WALL_BLOCK, 255, false));

	//getters for curMap
	Cell GetCube(glm::ivec4 pos) { return GetCube(pos.x, pos.y, pos.z, pos.w); }
	Cell GetCube(int x, int y, int z, int w) { return GetCube(GetIndex(x, y, z, w)); }
	Cell GetCube(int index);

	void GenerateLight(int x, int y, int z, int w) { GenerateLight(glm::ivec4(x, y, z, w)); };
	void GenerateLight(glm::ivec4 pos);
	void GenerateLight(int idx) { GenerateLight(idx4(idx)); };

	void CreateLightCube(int x, int y, int z, int w);

	glm::ivec4 size;
	
	int GetTotalSize() const { return this->totalSize; }
	int GetLightDist() const { return this->lightDist; }
	

private:
	//Shader* shader;
	RaycastingShader* rayShader;


	//for deletion private Shader members:
	//void GenerateFieldTexture();

	LightMap_t curLightMap;
	Map_t curMap;

	int totalSize;
	int lightDist;

	void GenerateLightRecursive(int px, int py, int pz, int pw, unsigned int level, int side);
	//void ClearLightArea(int x, int y, int z, int w);

	glm::ivec4 idx4(int indexInt);

	void RecalculateLight();

	void MapConstructor(const glm::ivec4 size, const int lightDist);
	void MapDestructor();

	int cubesCount = 0;

	//Map_t map;
	LightMap_t lightMap;
	Map_t map;
	
};

