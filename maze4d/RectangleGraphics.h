#pragma once

#include <glad/glad.h>
#include <shader.h>
#include "Utils.h"
class RectangleGraphics :
	public IDrawable
{
public:
	RectangleGraphics();
	~RectangleGraphics();

	void InitScene(float bottomX, float bottomY, float width, float height)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// set up vertex data (and buffer(s)) and configure vertex attributes
		// ------------------------------------------------------------------
		float vertices[] = {
			// positions          // colors           // texture coords
			bottomX + width,   bottomY + height, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
			bottomX + width,   bottomY,	         0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
			bottomX,           bottomY,			 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
			bottomX,		   bottomY + height, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
		};
		unsigned int indices[] = {
			0, 1, 3, // first triangle
			1, 2, 3  // second triangle
		};
		unsigned int VBO, EBO;
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

	void Draw(uint8_t* CpuTexData, int viewWidth, int viewHeight)
	{
		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, screenTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, viewWidth, viewHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, CpuTexData);

		shader->use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

private:
	unsigned int screenTex;
	unsigned int VAO;
	Shader* shader = nullptr;

};

