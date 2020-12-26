#include <Cube.h>

Cube::TextureSet_t Cube::textureSet;

void Cube::Init()
{
	textureSet[NEG_X].Init(glm::ivec3(351, 86, 80)); // red
	textureSet[POS_X].Init(glm::ivec3( 32, 86, 80)); // orange
	textureSet[NEG_Y].Init(glm::ivec3( 62, 86, 80)); // yellow
	textureSet[POS_Y].Init(glm::ivec3(128, 86, 80)); // green
	textureSet[NEG_Z].Init(glm::ivec3(180, 86, 80)); // cyan
	textureSet[POS_Z].Init(glm::ivec3(245, 86, 80)); // blue
	textureSet[NEG_W].Init(glm::ivec3(282, 86, 80)); // purple
	textureSet[POS_W].Init(glm::ivec3(  0,  0, 60)); // gray
}

void Cube::GetPixel(int edgeNum, glm::u8vec3& pixel, glm::vec3 texCoord,
	int& px, int& py, int& pz, int& pw, Cell_t& cell, Light_t& light)
{
	int lightLevel = ((light >> (edgeNum * 4)) & (Texture::LIGHT_GRAD - 1));

	// Converting coordinates from absolute to relative
	// to the position of the cube (in range 0..1)
	switch (edgeNum)
	{
	case NEG_X:
	case POS_X:
	{
		texCoord[0] -= py;
		texCoord[1] -= pz;
		texCoord[2] -= pw;
		break;
	}
	case NEG_Y:
	case POS_Y:
	{
		texCoord[0] -= px;
		texCoord[1] -= pz;
		texCoord[2] -= pw;
		break;
	}
	case NEG_Z:
	case POS_Z:
	{
		texCoord[0] -= px;
		texCoord[1] -= py;
		texCoord[2] -= pw;
		break;
	}
	case NEG_W:
	case POS_W:
	{
		texCoord[0] -= px;
		texCoord[1] -= py;
		texCoord[2] -= pz;
		break;
	}
	default:
		break;
	}

	bool isLightCell = (cell & LIGHT_BLOCK) != 0;

	// Add border
	if (glm::abs(texCoord[0] - 0.5f) < (0.5f - 1.0f / Texture::TEX_SIZE) &&
		glm::abs(texCoord[1] - 0.5f) < (0.5f - 1.0f / Texture::TEX_SIZE) &&
		glm::abs(texCoord[2] - 0.5f) < (0.5f - 1.0f / Texture::TEX_SIZE))
	{
		if (isLightCell)
		{
			pixel.x = 255;
			pixel.y = 252;
			pixel.z = 229;
		}
		else
		{
			pixel = textureSet[edgeNum].tex
				[int(texCoord[0] * Texture::TEX_SIZE)]
				[int(texCoord[1] * Texture::TEX_SIZE)]
				[int(texCoord[2] * Texture::TEX_SIZE)]
				[lightLevel];
		}
	}
	else
	{
		if (isLightCell)
		{
			pixel.x = 255;
			pixel.y = 233;
			pixel.z = 68;
		}
		else
		{
			pixel.x = glm::u8(lightLevel * (40.0f / (Texture::LIGHT_GRAD - 1)));
			pixel.y = glm::u8(lightLevel * (40.0f / (Texture::LIGHT_GRAD - 1)));
			pixel.z = glm::u8(lightLevel * (40.0f / (Texture::LIGHT_GRAD - 1)));
		}
	}
}
