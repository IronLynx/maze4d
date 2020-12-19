#pragma once

#include <Utils.h>
#include <Config.h>
#include <Player.h>
#include <PlayerController.h>
#include <Raycaster.h>
#include <Field.h>
#include <Renderer.h>
#include <Cube.h>

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
	}

	void Init();
	void Render(uint8_t* buffer);

	int viewWidth;
	int viewHeight;
	float viewScale;
	int vsync;

	Player player;
	PlayerController* playerController = nullptr;

private:
	Raycaster raycaster;
	Field* field = nullptr;
	Config* cfg = nullptr;
	Renderer* renderer = nullptr;
};
