#pragma once

#include <Field.h>
#include <Texture.h>

class Cube
{
public:
	Cube() {}

	static void Init(Shader* shader);
	static void BindTextures();

private:
	static void InitBlockTexture(Texture& tex, glm::ivec3 colorHSV, Shader* shader);
	static void InitSelectionTexture(Texture& tex, Shader* shader);
	static void InitLightTexture(Texture& tex, Shader* shader);

	typedef std::array<Texture, EDGES_COUNT> TextureSet_t;
	static TextureSet_t textureSet;
	static Texture textureLight;
	static Texture textureSelection;
};