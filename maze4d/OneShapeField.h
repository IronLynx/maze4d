#pragma once

#include <Field.h>

/*
class OneShapeField : public IDrawable
{
public:
	//Don't delete Field pointer in your outer code
	//Destructor of OneShapeField will delete this Field during it's own destruction
	OneShapeField(Field* shapeField) : shapeField(shapeField)
	{
		shader = new Shader();
		shader->LoadFromFiles("VertexShader.hlsl", "FragmentRaycasting4d.hlsl");
		shapeField->shader = shader;
		this->shapeField->LoadFieldToGL();
	}

	~OneShapeField()
	{
		if (shader != nullptr)
			delete shader;
		if (gameGraphics != nullptr)
			delete gameGraphics;
	}

	//floats are -1.0f .. + 1.0f
	void Init(Game* game, float posX, float posY, float width, float height)
	{
		glUseProgram(shader->ID);

		float ratioGlobal = (float)game->viewWidth / (float)game->viewHeight;
		float ratioLocal = width / height;

		GLint loc = glGetUniformLocation(shader->ID, "gameResolution");
		glUniform2i(loc, game->viewWidth, game->viewHeight);

		int AntiAliasingEnabled = game->cfg->GetInt("anti_aliasing");
		loc = glGetUniformLocation(shader->ID, "AntiAliasingEnabled");
		glUniform1i(loc, AntiAliasingEnabled);

		Texture::TEX_SIZE = game->cfg->GetInt("cube_pixels");
		Texture::BORDER_SIZE = game->cfg->GetInt("border_pixels");
		Texture::TEX_SMOOTHERING_FLAG = game->cfg->GetBool("texture_smoothering");

		Cube::Init(shader);

		if (gameGraphics != nullptr)
			delete gameGraphics;

		gameGraphics = new GameGraphics(shader, posX, posY, width, height);

		this->shapeField->LoadFieldToGL();
	}

	void SetBackgroundColor(glm::vec4 backgrounColor = glm::vec4(1.0f, 0.5f, 0.5f, 1.0f))
	{
		SetBackgroundColor(backgrounColor.x, backgrounColor.y, backgrounColor.z, backgrounColor.w);
	}
	void SetBackgroundColor(float R, float G, float B, float Alpha = 1.0f)
	{
		glUseProgram(shader->ID);
		GLint loc = glGetUniformLocation(shader->ID, "backgroundColor");
		glUniform4f(loc, R, G, B, Alpha);
	}

	void SetTextureTilesPerCube(float numTiles = 1.0f)
	{
		GLuint loc = glGetUniformLocation(shader->ID, "texturesPerCube");
		glUniform1f(loc, numTiles);
	}

	virtual void Draw()
	{
		shapeField->BindTextures();

		glUseProgram(shader->ID);
		
		GLint loc = glGetUniformLocation(shader->ID, "vx");
		glUniform4f(loc, player.vx.x, player.vx.y, player.vx.z, player.vx.w);

		loc = glGetUniformLocation(shader->ID, "vy");
		glUniform4f(loc, player.vy.x, player.vy.y, player.vy.z, player.vy.w);

		loc = glGetUniformLocation(shader->ID, "vz");
		glUniform4f(loc, player.vz.x, player.vz.y, player.vz.z, player.vz.w);

		loc = glGetUniformLocation(shader->ID, "vw");
		glUniform4f(loc, player.vw.x, player.vw.y, player.vw.z, player.vw.w);

		loc = glGetUniformLocation(shader->ID, "pos");
		glUniform4f(loc, player.pos.x, player.pos.y, player.pos.z, player.pos.w);

		gameGraphics->Draw();
	}

	void ClearShaders()
	{
		if (shader != nullptr)
			delete shader;
		shader = nullptr;
	}

	Field* shapeField;
	Player player;
	
public:
	GameGraphics* gameGraphics = nullptr;
	Shader* shader = nullptr;
};*/