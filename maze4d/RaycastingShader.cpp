#pragma once

#include <RaycastingShader.h>

RaycastingShader::RaycastingShader(const glm::ivec4 size, Config* cfg)
{
	SetNewMapSize(size);

	shader = new Shader();
	shader->LoadFromFiles("VertexShader.hlsl", "FragmentRaycasting4d.hlsl");

	//Cube::Init(shader);

	posBottomX = -1.0f;
	posBottomY = -1.0f; 
	posWidth = 2.0f;
	posHeight = 2.0f;

	gameGraphics = new RectangleGraphics(shader, posBottomX, posBottomY, posWidth, posHeight); //full screen by default

	basicResolutionX = glm::abs(cfg->GetInt("width"));
	basicResolutionY = glm::abs(cfg->GetInt("height"));

	SetFrameSize();
}

RaycastingShader::~RaycastingShader()
{
	glDeleteTextures(1, &curMapTextureId);
	glDeleteTextures(1, &curLightMapTextureId);

	delete shader;
	delete gameGraphics;
}

void RaycastingShader::SetNewMapSize(const glm::ivec4 size)
{
	this->size = size;

	texWxMax = (int)ceil(std::pow(size.w, 1 / 3.0f)); //cube root
	texWyMax = texWxMax;
	texWzMax = texWyMax;

	texSizeX = size.x * texWxMax;
	texSizeY = size.y * texWyMax;
	texSizeZ = size.z * texWzMax;
}

void RaycastingShader::Draw()
{
	//TestLoadAll();

	//SetResolution(1024, 768);

	BindTextures();
	gameGraphics->Draw();
}

void RaycastingShader::SetBackgroundColor(float R, float G, float B, float Alpha)
{
	glUseProgram(shader->ID);
	GLint loc = glGetUniformLocation(shader->ID, "backgroundColor");
	glUniform4f(loc, R, G, B, Alpha);
}

//recalculates resolution according to current frame size
void RaycastingShader::SetFrameSize()
{
	SetFrameSize(posBottomX, posBottomY, posWidth, posHeight);
}

void RaycastingShader::SetFrameSize(float bottomX, float bottomY, float width, float height)
{
	posBottomX = bottomX;
	posBottomY = bottomY;
	posWidth = width;
	posHeight = height;

	gameGraphics->SetNewPosition(bottomX, bottomY, width, height);

	int relativeResolutionX = (int)basicResolutionX * width / 2;
	int relativeResolutionY = (int)basicResolutionY * height / 2;

	SetResolution(relativeResolutionX, relativeResolutionY);
}

void RaycastingShader::SetResolution(int viewWidth, int viewHeight)
{
	glUseProgram(shader->ID);
	GLint loc = glGetUniformLocation(shader->ID, "gameResolution");
	glUniform2i(loc, viewWidth, viewHeight);
}

void RaycastingShader::SetAntialiasing(int antiAliasRate)
{
	antiAliasRate = std::min(antiAliasRate, 2);
	antiAliasRate = std::max(antiAliasRate, 0);

	glUseProgram(shader->ID);

	int AntiAliasingEnabled = antiAliasRate;
	GLuint loc = glGetUniformLocation(shader->ID, "AntiAliasingEnabled");
	glUniform1i(loc, AntiAliasingEnabled);
}

void RaycastingShader::SetMaxRayOutDistance(int distance = 0)
{
	glUseProgram(shader->ID);
	GLint loc = glGetUniformLocation(shader->ID, "maxRenderDistance");
	glUniform1i(loc, distance);
}


void RaycastingShader::SetCameraView(Player* player)
{
	glUseProgram(shader->ID);
	RotationMatrix* mat = &player->rotationMatrix;

	GLint loc = glGetUniformLocation(shader->ID, "vx");
	//glUniform4f(loc, player->vx.x, player->vx.y, player->vx.z, player->vx.w);
	glUniform4f(loc, mat->vx.x, mat->vx.y, mat->vx.z, mat->vx.w);

	loc = glGetUniformLocation(shader->ID, "vy");
	//glUniform4f(loc, player->vy.x, player->vy.y, player->vy.z, player->vy.w);
	glUniform4f(loc, mat->vy.x, mat->vy.y, mat->vy.z, mat->vy.w);

	loc = glGetUniformLocation(shader->ID, "vz");
	//glUniform4f(loc, player->vz.x, player->vz.y, player->vz.z, player->vz.w);
	glUniform4f(loc, mat->vz.x, mat->vz.y, mat->vz.z, mat->vz.w);

	loc = glGetUniformLocation(shader->ID, "vw");
	//glUniform4f(loc, player->vw.x, player->vw.y, player->vw.z, player->vw.w);
	glUniform4f(loc, mat->vw.x, mat->vw.y, mat->vw.z, mat->vw.w);

	loc = glGetUniformLocation(shader->ID, "pos");
	glUniform4f(loc, player->pos.x, player->pos.y, player->pos.z, player->pos.w);
}

void RaycastingShader::ReadConfig(Config* cfg)
{
	glUseProgram(shader->ID);

	if (Texture::TEX_SIZE != cfg->GetInt("cube_pixels") ||
		Texture::BORDER_SIZE != cfg->GetInt("border_pixels") ||
		Texture::TEX_SMOOTHERING_FLAG != cfg->GetBool("texture_smoothering"))
	{
		Texture::TEX_SIZE = cfg->GetInt("cube_pixels");
		Texture::BORDER_SIZE = cfg->GetInt("border_pixels");
		Texture::TEX_SMOOTHERING_FLAG = cfg->GetBool("texture_smoothering");

		Cube::Init(shader);
	}

	basicResolutionX = glm::abs(cfg->GetInt("width"));
	basicResolutionY = glm::abs(cfg->GetInt("height"));

	SetAntialiasing(cfg->GetInt("anti_aliasing"));
}


void RaycastingShader::BindTextures()
{
	Cube::BindTextures();
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_3D, curMapTextureId);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //GL_NEAREST GL_LINEAR
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_3D, curLightMapTextureId);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void RaycastingShader::LoadFieldToGL()
{
	BindTextures();

	glUseProgram(shader->ID);

	GLint mapSizeLoc = glGetUniformLocation(shader->ID, "mapSize");
	glUniform4i(mapSizeLoc, size.x, size.y, size.z, size.w); // Texture unit 4 is for current map. 

	GLint MapWnAddedSizeLoc = glGetUniformLocation(shader->ID, "MapWnAddedSize");
	glUniform3i(MapWnAddedSizeLoc, texWxMax, texWyMax, texWzMax);

	GLint MapTexSizeLoc = glGetUniformLocation(shader->ID, "MapTexSize");
	glUniform3i(MapTexSizeLoc, texSizeX, texSizeY, texSizeZ);

	glUseProgram(shader->ID);
	GLint curMapLightId = glGetUniformLocation(shader->ID, "currentLightMap");
	glUniform1i(curMapLightId, 2);

	glUseProgram(shader->ID);
	GLint curMapId = glGetUniformLocation(shader->ID, "currentMap");
	glUniform1i(curMapId, 1); // Texture unit 1 is for current map.

	Cube::LoadToGL(shader);
	Cube::LoadLightTextureToGL(shader);
}

//for debug purposes only
void RaycastingShader::DebugLoadAll()
{
	Cube::Init(shader);

	SetAntialiasing(1);

	Player defaultPlayer = Player();
	defaultPlayer.Reset();
	SetCameraView(&defaultPlayer);

	SetResolution(1024, 768);

	BindTextures();

	glUseProgram(shader->ID);

	GLint mapSizeLoc = glGetUniformLocation(shader->ID, "mapSize");
	glUniform4i(mapSizeLoc, size.x, size.y, size.z, size.w); // Texture unit 4 is for current map. 

	GLint MapWnAddedSizeLoc = glGetUniformLocation(shader->ID, "MapWnAddedSize");
	glUniform3i(MapWnAddedSizeLoc, texWxMax, texWyMax, texWzMax);

	GLint MapTexSizeLoc = glGetUniformLocation(shader->ID, "MapTexSize");
	glUniform3i(MapTexSizeLoc, texSizeX, texSizeY, texSizeZ);

	glUseProgram(shader->ID);
	GLint curMapLightId = glGetUniformLocation(shader->ID, "currentLightMap");
	glUniform1i(curMapLightId, 2);

	glUseProgram(shader->ID);
	GLint curMapId = glGetUniformLocation(shader->ID, "currentMap");
	glUniform1i(curMapId, 1); // Texture unit 1 is for current map.


}

int RaycastingShader::GetIndex(const int x, const int y, const int z, const int w)
{
	assert(IsCubeIndexValid(x, y, z, w));
	return x*size.y*size.z*size.w + y*size.z*size.w + z*size.w + w;
}

int RaycastingShader::GetIndex(glm::ivec4 pos)
{
	return GetIndex(pos.x, pos.y, pos.z, pos.w);
}


bool RaycastingShader::IsCubeIndexValid(int x, int y, int z, int w)
{
	return (
		x >= 0 && x < size.x &&
		y >= 0 && y < size.y &&
		z >= 0 && z < size.z &&
		w >= 0 && w < size.w);
}

//Convert 4-axis coordinates to 3-axis coordinates
//Encode w coordinate equally across all xyz coords
//returns INT value which encodes xyz point for 4d point
int RaycastingShader::GetTexIndex(int x, int y, int z, int w)
{
	//Assume w coordinate is index of 3 other coordinates
	//What are they if we have length for each of them?
	int texWz = w / (texWxMax*texWyMax);
	int texWy = (w % (texWxMax*texWyMax)) / texWxMax;
	int texWx = (w % (texWxMax*texWyMax)) % texWxMax;

	//here is our final equation
	assert(w == texWz * texWxMax*texWyMax + texWy*texWxMax + texWx);

	int texX = texWx * size.x + x;
	int texY = texWy * size.y + y;
	int texZ = texWz * size.z + z;

	return 4 * (texZ * texSizeY * texSizeX + texY * texSizeX + texX);
};

void RaycastingShader::GenerateFieldTexture(Map_t curMap, LightMap_t curLightMap)
{
	if (isTextureExists)
	{
		glDeleteTextures(1, &curLightMapTextureId);
		isTextureExists = false;
	}
	isTextureExists = true;

	GenerateCurMapTexture(curMap);
	GenerateLightMapTexture(curLightMap);
}

void RaycastingShader::GenerateLightMapTexture(LightMap_t curLightMap)
{
	//each index is vec4 float structure 
	//vec4.x - alpha channel for whole cube
	//vec4.y - texture type (1 for regular, 2 for light)
	//vec4.z - light level
	unsigned int totalSizeForTexture = texSizeZ * texSizeY * texSizeX + texSizeY * texSizeX + texSizeX; //GetTexIndex(size.x, size.y, size.z, size.w);
	uint8_t* curLightMapTexture = new uint8_t[4 * totalSizeForTexture];
	//floatBitsToInt
	//uint64_t idx = 0;
	for (int x = 0; x < size.x; x++)
		for (int y = 0; y < size.y; y++)
			for (int z = 0; z < size.z; z++)
				for (int w = 0; w < size.w; w++)
				{
					int FieldIdx = GetIndex(x, y, z, w);
					int TexIdx = GetTexIndex(x, y, z, w);

					Light_t light = curLightMap[FieldIdx];
					int LightLevelNegX = ((light >> (NEG_X * 4)) & (Texture::LIGHT_GRAD - 1));
					int LightLevelPosX = ((light >> (POS_X * 4)) & (Texture::LIGHT_GRAD - 1));
					int LightLevelNegY = ((light >> (NEG_Y * 4)) & (Texture::LIGHT_GRAD - 1));
					int LightLevelPosY = ((light >> (POS_Y * 4)) & (Texture::LIGHT_GRAD - 1));
					int LightLevelNegZ = ((light >> (NEG_Z * 4)) & (Texture::LIGHT_GRAD - 1));
					int LightLevelPosZ = ((light >> (POS_Z * 4)) & (Texture::LIGHT_GRAD - 1));
					int LightLevelNegW = ((light >> (NEG_W * 4)) & (Texture::LIGHT_GRAD - 1));
					int LightLevelPosW = ((light >> (POS_W * 4)) & (Texture::LIGHT_GRAD - 1));

					if (LightLevelPosX > 0)
						int i = 0;
					curLightMapTexture[TexIdx + 0] = LightLevelPosX * 16 + LightLevelNegX;
					curLightMapTexture[TexIdx + 1] = LightLevelPosY * 16 + LightLevelNegY;
					curLightMapTexture[TexIdx + 2] = LightLevelPosZ * 16 + LightLevelNegZ;
					curLightMapTexture[TexIdx + 3] = LightLevelPosW * 16 + LightLevelNegW;
				}

	//Generate Light Texture map
	glGenTextures(1, &curLightMapTextureId);
	glBindTexture(GL_TEXTURE_3D, curLightMapTextureId);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, texSizeX, texSizeY, texSizeZ, 0, GL_RGBA, GL_UNSIGNED_BYTE, curLightMapTexture);

	delete[] curLightMapTexture;
}

void RaycastingShader::GenerateCurMapTexture(Map_t curMap)
{
	//each index is vec4 float structure 
	//vec4.x - alpha channel for whole cube
	//vec4.y - texture type (1 for regular, 2 for light)
	//vec4.z - light level
	unsigned int totalSizeForTexture = texSizeZ * texSizeY * texSizeX + texSizeY * texSizeX + texSizeX; //GetTexIndex(size.x, size.y, size.z, size.w);
	uint8_t* curMapTexture = new uint8_t[4 * totalSizeForTexture];
	//floatBitsToInt
	//uint64_t idx = 0;
	for (int x = 0; x < size.x; x++)
		for (int y = 0; y < size.y; y++)
			for (int z = 0; z < size.z; z++)
				for (int w = 0; w < size.w; w++)
				{
					int FieldIdx = GetIndex(x, y, z, w);
					int TexIdx = GetTexIndex(x, y, z, w);
					curMapTexture[TexIdx] = 0; //no block by default
					curMapTexture[TexIdx + 1] = 0;
					curMapTexture[TexIdx + 2] = 0;
					curMapTexture[TexIdx + 3] = 0;

					if (curMap[FieldIdx] == nullptr)
					{
						//do nothing
					}
					else if (curMap[FieldIdx]->cellType == WALL_BLOCK)
					{
						//float CellAlpha = (float) curMap[FieldIdx].alphaValue / 255.0f;
						curMapTexture[TexIdx] = curMap[FieldIdx]->alphaValue; // fully solid block
						curMapTexture[TexIdx + 1] = 10; //regular block type
					}
					else if (curMap[FieldIdx]->cellType == LIGHT_BLOCK)
					{
						curMapTexture[TexIdx] = curMap[FieldIdx]->alphaValue;
						curMapTexture[TexIdx + 1] = 255; //light block type
					}
				}

	//Generate field texture map
	glGenTextures(1, &curMapTextureId);
	glBindTexture(GL_TEXTURE_3D, curMapTextureId);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, texSizeX, texSizeY, texSizeZ, 0, GL_RGBA, GL_UNSIGNED_BYTE, curMapTexture);

	delete[] curMapTexture;
}