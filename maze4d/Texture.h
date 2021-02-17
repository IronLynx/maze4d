#pragma once

#include <shader.h>
#include <Utils.h>

class Texture
{
public:
	Texture() 
	{
		MIN_VALUE = 5;
		TEX_SIZE = 16;
		LIGHT_GRAD = 16;
		TEX_SMOOTHERING_FLAG = false;
	}

	~Texture()
	{
		if (tex_new != nullptr)
			delete tex_new;
	}

	void Init(glm::ivec3 colorHSV)
	{
		if (tex_new != nullptr)
			delete[] tex_new;
		tex_new = new glm::u8vec3[Size()];

		Random* rnd = Random::GetInstance();
		for (int i = 0; i < TEX_SIZE; i++)
		{
			for (int j = 0; j < TEX_SIZE; j++)
			{
				for (int k = 0; k < TEX_SIZE; k++)
				{
					int initValue = rnd->GetInt(glm::max(0, colorHSV[2] - 20), glm::min(100, colorHSV[2] + 20));

					int minBorder = BORDER_SIZE;
					int maxBorder = (int)(TEX_SIZE - BORDER_SIZE);
					if (i < minBorder || i >= maxBorder || j < minBorder || j >= maxBorder || k < minBorder || k >= maxBorder)
						initValue = 20;
					

					int value = initValue;

					for (int l = LIGHT_GRAD - 1; l >= 0; l--)
					{
						HSVtoRGB(colorHSV[0], colorHSV[1], value, TexByIndex(i,j,k,l).x, TexByIndex(i, j, k, l).y, TexByIndex(i, j, k, l).z);
						value = (initValue - MIN_VALUE) * l / (LIGHT_GRAD - 1) + MIN_VALUE;						
					}
				}
			}
		}
	}

	static int MIN_VALUE;
	static int TEX_SIZE;
	static int LIGHT_GRAD;
	static float BORDER_SIZE;
	static bool TEX_SMOOTHERING_FLAG;
	
	glm::u8vec3 *tex_new;

	glm::u8vec3& TexByIndex(const int x, const int y, const int z, const int l)
	{
		if (tex_new != nullptr)
			return tex_new[GetIndex(x, y, z, l)];
	}

	int Size() {
		// +1 for the case when int(texCoord * TEX_SIZE) == TEX_SIZE
		return (TEX_SIZE + 1)*(TEX_SIZE + 1)*(TEX_SIZE + 1)*(LIGHT_GRAD);
	}

	int GetIndex(const int x, const int y, const int z, const int l)
	{
		return x*(TEX_SIZE + 1)*(TEX_SIZE + 1)*LIGHT_GRAD + y*(TEX_SIZE + 1)*LIGHT_GRAD + z * LIGHT_GRAD + l;
	};
};

