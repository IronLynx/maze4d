#include <Cube.h>


Cube::TextureSet_t Cube::textureSet;
Texture Cube::textureLight;

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

	textureLight.InitLight();

	LoadToGL(shader);
	LoadLightTextureToGL(shader);
}

void Cube::BindTextures()
{
	for (int edge = 0; edge < 8; edge++)
	{
		glActiveTexture(GL_TEXTURE0 + 50 + edge);
		glBindTexture(GL_TEXTURE_3D, textureSet[edge].textureId);
	}

	glActiveTexture(GL_TEXTURE0 + 50 + 9);
	glBindTexture(GL_TEXTURE_3D, textureLight.textureId);
}

void Cube::LoadToGL(Shader* shader)
{
	GLint* EdgeTextureUnits = new GLint[8];
	glUseProgram(shader->ID);

	BindTextures();
	for (int edge = 0; edge < 8; edge++)
		EdgeTextureUnits[edge] = 50 + edge;

	//actually bind variables
	glUseProgram(shader->ID);
	GLint loc = glGetUniformLocation(shader->ID, "edge3dCube");
	glUniform1iv(loc, 8, EdgeTextureUnits);

	glUseProgram(shader->ID);
	loc = glGetUniformLocation(shader->ID, "light3dCube");
	glUniform1i(loc, 50+9); // Texture unit 4 is for current map.

	delete[] EdgeTextureUnits;

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

					buf[idx + 1] = 68;
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
	glUniform1i(variableId, LightTextureUnit); 

	delete[] buf;
}
