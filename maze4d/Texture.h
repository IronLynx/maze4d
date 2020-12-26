#pragma once

#include <Utils.h>

class Texture
{
public:
	Texture() {}

	void Init(glm::ivec3 colorHSV)
	{
		Random* rnd = Random::GetInstance();
		for (int i = 0; i < TEX_SIZE; i++)
		{
			for (int j = 0; j < TEX_SIZE; j++)
			{
				for (int k = 0; k < TEX_SIZE; k++)
				{
					int initValue = rnd->GetInt(glm::max(0, colorHSV[2] - 20), glm::min(100, colorHSV[2] + 20));
					int value = initValue;
					for (int l = LIGHT_GRAD - 1; l >= 0; l--)
					{
						HSVtoRGB(colorHSV[0], colorHSV[1], value, tex[i][j][k][l].x, tex[i][j][k][l].y, tex[i][j][k][l].z);
						value = (initValue - MIN_VALUE) * l / (LIGHT_GRAD - 1) + MIN_VALUE;
					}
				}
			}
		}
	}

	static const int MIN_VALUE = 5;
	static const int TEX_SIZE = 16;
	static const int LIGHT_GRAD = 16;
	// +1 for the case when int(texCoord * TEX_SIZE) == TEX_SIZE
	glm::u8vec3 tex[TEX_SIZE + 1][TEX_SIZE + 1][TEX_SIZE + 1][LIGHT_GRAD];
};
