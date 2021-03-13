#pragma once


#include <Field.h>
#include <Texture.h>

//#include <Utils.h>

class Cube
{
public:
	Cube() {}

	static void Init(Shader* shader);
	static void LoadToGL(Shader* shader);
	static void LoadLightTextureToGL(Shader* shader);
	static void BindTextures();

private:
	typedef std::array<Texture, EDGES_COUNT> TextureSet_t;
	static TextureSet_t textureSet;
	static Texture textureLight;
};