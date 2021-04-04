#pragma once

#include <glad/glad.h>
#include <shader.h>
#include <cstring>

class RectangleGraphics
{
public:
	//viewport matrix is -1.0f...1.0f range in both axis
	RectangleGraphics(Shader* shader, float bottomX, float bottomY, float width, float height)
	{
		this->shader = shader;
		InitScene(bottomX, bottomY, width, height);
	}

	~RectangleGraphics()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteTextures(1, &screenTex);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
		if (selfShader)
			glDeleteShader(shader->ID);
	}

	void InitScene(float bottomX, float bottomY, float width, float height)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// set up vertex data (and buffer(s)) and configure vertex attributes
		// ------------------------------------------------------------------
		float vertices[] = {
			// positions          // colors           // texture coords
			bottomX,		   bottomY + height, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f, //4 top left 
			bottomX,           bottomY,			 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, //3 bottom left
			bottomX + width,   bottomY,	         0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, //2 bottom right
			bottomX + width,   bottomY + height, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f  //1 top right
		};

		unsigned int indices[] = {
			0, 1, 3, // first triangle
			1, 2, 3  // second triangle
		};
		
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// color attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// texture coord attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		// load and create a texture 
		// -------------------------
		glGenTextures(1, &screenTex);
		glBindTexture(GL_TEXTURE_2D, screenTex);
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	void Draw(glm::u8vec3 colorRGB)
	{
		glm::u8vec4 colorRGBA = glm::u8vec4(colorRGB.x, colorRGB.y, colorRGB.z, 255);
		Draw(colorRGBA);
	}

	void SetNewPosition(float bottomX, float bottomY, float width, float height)
	{
		float vertices[] = {
			// positions          // colors           // texture coords
			bottomX,		   bottomY + height, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f, //4 top left 
			bottomX,           bottomY,			 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, //3 bottom left
			bottomX + width,   bottomY,	         0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, //2 bottom right
			bottomX + width,   bottomY + height, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f  //1 top right
		};

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	}

	void Draw(uint8_t R, uint8_t G, uint8_t B, uint8_t Alpha = 255)
	{
		Draw(glm::u8vec4(R, G, B, Alpha));
	}

	void Draw(glm::u8vec4 colorRGBA = glm::u8vec4(255, 100, 100,0))
	{
		uint8_t* onePixelBuffer = new uint8_t[4];
		memset(onePixelBuffer, 0, 4);
		onePixelBuffer[0] = colorRGBA.x;
		onePixelBuffer[1] = colorRGBA.y;
		onePixelBuffer[2] = colorRGBA.z;
		onePixelBuffer[3] = colorRGBA.w;

		Draw(onePixelBuffer, 1, 1);

		delete[] onePixelBuffer;
		
	}

	//texture should be in RGBA buffer
	void Draw(uint8_t* textureData, int textureWidth, int textureHeight, GLint internalFormat = GL_RGBA)
	{
		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, screenTex);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, textureWidth, textureHeight, 0, internalFormat, GL_UNSIGNED_BYTE, textureData);

		shader->use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	void InitSelfShader()
	{
		if (selfShader)
			glDeleteShader(shader->ID);

		shader = new Shader();
		shader->LoadFromFiles("VertexShader.hlsl", "FragmentShader.hlsl");

		selfShader = true;
	}

	void InitRemoteShader(Shader* remoteShader)
	{
		if (selfShader)
		{
			glDeleteShader(shader->ID);
			delete shader;
			
		}
		this->shader = remoteShader;
		selfShader = false;
	}


private:
	unsigned int screenTex;
	unsigned int VAO, VBO, EBO;
	Shader* shader = nullptr;
	bool selfShader = false;

};