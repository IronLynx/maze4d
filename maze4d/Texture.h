#pragma once

#include <shader.h>

class Texture
{
public:
	GLuint textureId;

// static
	static int TEX_SIZE;
	static int LIGHT_GRAD;
	static int BORDER_SIZE;
	static bool TEX_SMOOTHERING_FLAG;
	
	static int GetSize()
	{
		return TEX_SIZE * TEX_SIZE * TEX_SIZE;
	}

	static int GetPixelIndex(const int x, const int y, const int z)
	{
		return x * TEX_SIZE * TEX_SIZE + y * TEX_SIZE + z;
	};

	static glm::u8vec4& GetPixel(const int x, const int y, const int z, glm::u8vec4* texBuffer)
	{
		return texBuffer[GetPixelIndex(x, y, z)];
	}

	static void GenerateGlTexture(GLuint& textureId, glm::u8vec4* texBuffer)
	{
		//glDeleteTextures(1, &textureId);
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_3D, textureId);
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT); //horizontal wrap method
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT); //vertical wrap method

		// set texture filtering parameters
		GLint smootheringParam = Texture::TEX_SMOOTHERING_FLAG ? GL_LINEAR : GL_NEAREST;
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, smootheringParam);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, smootheringParam);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, Texture::TEX_SIZE, Texture::TEX_SIZE, Texture::TEX_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, texBuffer);
	}
};
