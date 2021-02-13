#include <Cube.h>


Cube::TextureSet_t Cube::textureSet;

void Cube::Init(Shader* shader)
{
	textureSet[NEG_X].Init(glm::ivec3(351, 86, 80)); // red
	textureSet[POS_X].Init(glm::ivec3( 32, 86, 80)); // orange
	textureSet[NEG_Y].Init(glm::ivec3( 62, 86, 80)); // yellow
	textureSet[POS_Y].Init(glm::ivec3(128, 86, 80)); // green
	textureSet[NEG_Z].Init(glm::ivec3(180, 86, 80)); // cyan
	textureSet[POS_Z].Init(glm::ivec3(245, 86, 80)); // blue
	textureSet[NEG_W].Init(glm::ivec3(282, 86, 80)); // purple
	textureSet[POS_W].Init(glm::ivec3(  0,  0, 60)); // gray

	LoadToGL(shader);
	LoadLightTextureToGL(shader);
}

void Cube::LoadToGL(Shader* shader)
{
	//return;
	GLsizei size = Texture::TEX_SIZE*Texture::TEX_SIZE*Texture::TEX_SIZE;
	int width = Texture::TEX_SIZE;
	int height = Texture::TEX_SIZE;
	int depth = Texture::TEX_SIZE;


	auto GetIndex = [&](int x, int y, int z)
	{
		return 4 * (z * width*height + y * width + x);
	};

	//int indx = GetIndex(5, 4, 3, 2);

	GLint* EdgeTextureUnits = new GLint[8];
	GLuint* TexturesId = new GLuint[8];
	glUseProgram(shader->ID);

	for (int edge = 0; edge < 8; edge++)
	{
		uint8_t* buf = new uint8_t[size * 4];


		for (int x = 0; x < Texture::TEX_SIZE; x++)
			for (int y = 0; y < Texture::TEX_SIZE; y++)
				for (int z = 0; z < Texture::TEX_SIZE; z++)
				{
					int idx = GetIndex(x, y, z);
					glm::uvec3 pixel = textureSet[edge].TexByIndex(x, y, z, 15);
					buf[idx + 0] = pixel.x;// pixel.x; //Red
					buf[idx + 1] = pixel.y; //Green
					buf[idx + 2] = pixel.z; //Blue
					buf[idx + 3] = 255; //Alpha
				}

		EdgeTextureUnits[edge] = 50 + edge;
		glActiveTexture(GL_TEXTURE0 + EdgeTextureUnits[edge]);

		glGenTextures(1, &TexturesId[edge]);
		glBindTexture(GL_TEXTURE_3D, TexturesId[edge]);
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT); //horizontal wrap method
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT); //vertical wrap method
		
		// set texture filtering parameters
		GLint smootheringParam = Texture::TEX_SMOOTHERING_FLAG ? GL_LINEAR : GL_NEAREST;
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, smootheringParam); //GL_LINEAR GL_NEAREST
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, smootheringParam);

		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, Texture::TEX_SIZE, Texture::TEX_SIZE, Texture::TEX_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf);

		delete[] buf;
	}

	//for (int i=0;i<8;i++) EdgeTextureIds[i] = 50;

	unsigned int max = GL_MAX_TEXTURE_BUFFER_SIZE;
	glUseProgram(shader->ID);
	GLint variableId = glGetUniformLocation(shader->ID, "edge3dCube");
	glUniform1iv(variableId, 8, EdgeTextureUnits); // Texture unit 4 is for current map.

}


void Cube::LoadLightTextureToGL(Shader* shader)
{
	//return;
	GLsizei size = Texture::TEX_SIZE*Texture::TEX_SIZE*Texture::TEX_SIZE;
	int width = Texture::TEX_SIZE;
	int height = Texture::TEX_SIZE;
	int depth = Texture::TEX_SIZE;


	auto GetIndex = [&](int x, int y, int z)
	{
		return 4 * (z * width*height + y * width + x);
	};
	
	GLint LightTextureUnit;
	GLuint TexturesId;
	glUseProgram(shader->ID);

	
	uint8_t* buf = new uint8_t[size * 4];


	for (int x = 0; x < Texture::TEX_SIZE; x++)
		for (int y = 0; y < Texture::TEX_SIZE; y++)
			for (int z = 0; z < Texture::TEX_SIZE; z++)
			{
				int idx = GetIndex(x, y, z);

				int minBorder = Texture::BORDER_SIZE;
				int maxBorder = (int)(Texture::TEX_SIZE - Texture::BORDER_SIZE);
				if (x < minBorder || x >= maxBorder || y < minBorder || y >= maxBorder || z < minBorder || z >= maxBorder)
				{
					buf[idx + 0] = 255;
					buf[idx + 1] = 233;
					buf[idx + 2] = 68;
					buf[idx + 3] = 255; //Alpha
				}
				else
				{
					buf[idx + 0] = 255;
					buf[idx + 1] = 252;
					buf[idx + 2] = 229;
					buf[idx + 3] = 235; //Alpha
				}
			}

	LightTextureUnit = 50 + 9;
	glActiveTexture(GL_TEXTURE0 + LightTextureUnit);

	glGenTextures(1, &TexturesId);
	glBindTexture(GL_TEXTURE_3D, TexturesId);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT); //horizontal wrap method
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT); //vertical wrap method

	// set texture filtering parameters
	GLint smootheringParam = Texture::TEX_SMOOTHERING_FLAG ? GL_LINEAR : GL_NEAREST;
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, smootheringParam); //GL_LINEAR GL_NEAREST
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, smootheringParam);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, Texture::TEX_SIZE, Texture::TEX_SIZE, Texture::TEX_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf);


	glUseProgram(shader->ID);
	GLint variableId = glGetUniformLocation(shader->ID, "light3dCube");
	glUniform1i(variableId, LightTextureUnit); // Texture unit 4 is for current map.
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

	int x = int(texCoord[0] * Texture::TEX_SIZE);
	int y = int(texCoord[1] * Texture::TEX_SIZE);
	int z = int(texCoord[2] * Texture::TEX_SIZE);
	pixel = textureSet[edgeNum].TexByIndex(x, y, z, lightLevel);

	//Add border to light cell
	bool isLightCell = (cell & LIGHT_BLOCK) != 0;	
	if (isLightCell)
	{
		if (glm::abs(texCoord[0] - 0.5f) < (0.5f - Texture::BORDER_SIZE / Texture::TEX_SIZE) &&
			glm::abs(texCoord[1] - 0.5f) < (0.5f - Texture::BORDER_SIZE / Texture::TEX_SIZE) &&
			glm::abs(texCoord[2] - 0.5f) < (0.5f - Texture::BORDER_SIZE / Texture::TEX_SIZE))
		{
			pixel.x = 255;
			pixel.y = 252;
			pixel.z = 229;
		}	
		else
		{
			pixel.x = 255;
			pixel.y = 233;
			pixel.z = 68;
		}
	}
}
