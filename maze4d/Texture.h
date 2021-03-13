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
		if (texBuffer != nullptr)
			delete texBuffer;
	}

	void Init(glm::ivec3 colorHSV)
	{
		if (texBuffer != nullptr)
			delete[] texBuffer;
		texBuffer = new glm::u8vec4[Size()];

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

					HSVtoRGB(colorHSV[0], colorHSV[1], initValue, TexByIndex(i, j, k).x, TexByIndex(i, j, k).y, TexByIndex(i, j, k).z);
					TexByIndex(i, j, k).w = 255;
				}
			}
		}
		
		isValid = false;
		GenerateGlTexture();
	}

	void InitLight()
	{
		if (texBuffer != nullptr)
			delete[] texBuffer;
		texBuffer = new glm::u8vec4[Size()];

		for (int x = 0; x < Texture::TEX_SIZE; x++)
			for (int y = 0; y < Texture::TEX_SIZE; y++)
				for (int z = 0; z < Texture::TEX_SIZE; z++)
				{
					int idx = GetIndex(x, y, z);

					int minBorder = Texture::BORDER_SIZE;
					int maxBorder = (int)(Texture::TEX_SIZE - Texture::BORDER_SIZE);
					if (x < minBorder || x >= maxBorder || y < minBorder || y >= maxBorder || z < minBorder || z >= maxBorder)
						texBuffer[idx] = glm::u8vec4(255, 233, 68, 255);
					else
						texBuffer[idx] = glm::u8vec4(255, 252, 229, 255);
				}

		isValid = false;
		GenerateGlTexture();
	}

	static int MIN_VALUE;
	static int TEX_SIZE;
	static int LIGHT_GRAD;
	static int BORDER_SIZE;
	static bool TEX_SMOOTHERING_FLAG;

	GLuint textureId;
	bool isValid = false;
	
	glm::u8vec4 *texBuffer;

	glm::u8vec4& TexByIndex(const int x, const int y, const int z)
	{
		if (texBuffer != nullptr)
			return texBuffer[GetIndex(x, y, z)];

		return texBuffer[0];
	}

	int Size() {
		return (TEX_SIZE)*(TEX_SIZE)*(TEX_SIZE);
	}

	int GetIndex(const int x, const int y, const int z)
	{
		return x*(TEX_SIZE)*(TEX_SIZE) + y*(TEX_SIZE) + z;
	};

	void GenerateGlTexture()
	{
		if (isValid)
			glDeleteTextures(1, &textureId);

		isValid = true;

		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_3D, textureId);
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT); //horizontal wrap method
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT); //vertical wrap method

		// set texture filtering parameters
		GLint smootheringParam = Texture::TEX_SMOOTHERING_FLAG ? GL_LINEAR : GL_NEAREST;
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, smootheringParam); //GL_LINEAR GL_NEAREST
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, smootheringParam);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, Texture::TEX_SIZE, Texture::TEX_SIZE, Texture::TEX_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, texBuffer);
	}




	
	
};

