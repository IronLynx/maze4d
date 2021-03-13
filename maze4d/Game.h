#pragma once

#include <RectangleGraphics.h>
#include <Field.h>
#include <MazeField.h>

#include <Player.h>
#include <PlayerController.h>

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

	}

	void Init();
	void Draw();
	void ReinitVideoConfig();
	bool NeedReconfigureResolution = false;

	

	void NewGame();
	void NewEditor();

	void ApplyNewParameters();

	int viewWidth;
	int viewHeight;
	float viewScale;
	int vsync;
	//int CpuRender;

	bool editorToolsEnabled = false;

	Player player;
	PlayerController* playerController = nullptr;

	
	Config* cfg = nullptr;
	Field* field = nullptr; //to get roo
private:
//	Raycaster raycaster;
	
	void UpdateShaderPlayer(Player curPlayer);
};
