#pragma once

#include <Utils.h>
#include <Field.h>
#include <Texture.h>

class Cube
{
public:
	Cube() {}

	static void Init();

	static void GetPixel(int edgeNum, glm::u8vec3& pixel, glm::vec3 texCoord,
		int& px, int& py, int& pz, int& pw, Cell_t& cell, Light_t& light);

private:
	typedef std::array<Texture, EDGES_COUNT> TextureSet_t;
	static TextureSet_t textureSet;
};