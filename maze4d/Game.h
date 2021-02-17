#pragma once

#include <GameGraphics.h>
#include <Field.h>

#include <Player.h>
#include <PlayerController.h>
#include <Raycaster.h>

#include <Renderer.h>
#include <Cube.h>

#include <Config.h>
//#include <Utils.h>

class Game
{
public:
	Game();

	~Game()
	{
		if (playerController != nullptr)
			delete playerController;
		if (field != nullptr)
			delete field;
		if (cfg != nullptr)
			delete cfg;
		if (renderer != nullptr)
			delete renderer;
		if (mainScene != nullptr)
			delete mainScene;
		if (helperScene1 != nullptr)
			delete helperScene1;
		if (helperScene2 != nullptr)
			delete helperScene2;
		if (shaderGame != nullptr)
			delete shaderGame;
		if (shaderUi != nullptr)
			delete shaderUi;
	}

	void Init();
	void Render(uint8_t* buffer);
	void DrawScene(uint8_t* buffer);
	void ReinitVideoConfig();
	bool NeedReconfigureResolution = false;
	void ClearShaders()
	{
		if (shaderGame != nullptr)
			delete shaderGame;
		if (shaderUi != nullptr)
			delete shaderUi;
		shaderGame = nullptr;
		shaderUi = nullptr;
	}
	

	void NewGame();
	void ApplyNewParameters();

	int viewWidth;
	int viewHeight;
	float viewScale;
	int vsync;
	int CpuRender;

	Player player;
	PlayerController* playerController = nullptr;

	
	Config* cfg = nullptr;
	Field* field = nullptr; //to get roo

private:
	Raycaster raycaster;
	Renderer* renderer = nullptr;
	GameGraphics* mainScene = nullptr;
	GameGraphics* UserInterface = nullptr;
	GameGraphics* helperScene1 = nullptr;
	GameGraphics* helperScene2 = nullptr;
	Shader* shaderGame = nullptr;
	Shader* shaderUi = nullptr;
	void UpdateShaderPlayer(Player curPlayer);
};
