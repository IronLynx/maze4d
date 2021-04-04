#pragma once

#include <shader.h>
#include <Player.h>
#include <Config.h>
#include <Cell.h>
#include <RectangleGraphics.h>
#include <Utils.h>

typedef uint32_t Light_t;
typedef Light_t* LightMap_t;

typedef Cell* Cell_t;
typedef Cell_t* Map_t;

class Player;

class RaycastingShader : public IDrawable
{
public:

	RaycastingShader(const glm::ivec4 size, Config* cfg);
	~RaycastingShader();

	virtual void Draw();

	void SetNewMapSize(const glm::ivec4 size);

	void SetFrameSize(float bottomX, float bottomY, float width, float height);
	void SetFrameSize();
	void SetResolution(int viewWidth, int viewHeight);
	void SetMaxRayOutDistance(int distance);
	void SetAntialiasing(int antiAliasRate = 1);//0 - no, 1 - x4, 2 - x9 times sampling
	void SetSelectedBlock(glm::ivec4 pos);
	void ReadConfig(Config* cfg);
	void SetBackgroundColor(glm::vec4 backgrounColor = glm::vec4(1.0f, 0.5f, 0.5f, 1.0f)) { SetBackgroundColor(backgrounColor.x, backgrounColor.y, backgrounColor.z, backgrounColor.w); }
	void SetBackgroundColor(float R, float G, float B, float Alpha = 1.0f);
	void SetCameraView(Player* player);
	void LoadFieldToGL();

	void GenerateFieldTexture(Map_t curMap, LightMap_t curLightMap);

	int GetIndex(const int x, const int y, const int z, const int w);
	int GetIndex(const glm::ivec4 pos);
	bool IsCubeIndexValid(int x, int y, int z, int w);

	void BindTextures();

private:
	int basicResolutionX = 480;
	int basicResolutionY = 360;

	int texWxMax, texWyMax, texWzMax;
	int texSizeX, texSizeY, texSizeZ;
	bool isTextureExists = false;
	Shader* shader;

	float posBottomX, posBottomY, posWidth, posHeight;

	void DebugLoadAll();
	

	void GenerateCurMapTexture(Map_t curMap);
	void GenerateLightMapTexture(LightMap_t curLightMap);

	int GetTexIndex(int x, int y, int z, int w);

	glm::ivec4 size;

	GLuint curMapTextureId, curLightMapTextureId;
	RectangleGraphics* gameGraphics = nullptr;

};