#include <Game.h>

Game::Game()
{
	cfg = new Config();

	Random::GetInstance()->Init(cfg->GetInt("seed"));

	viewWidth = glm::abs(cfg->GetInt("width"));
	viewHeight = glm::abs(cfg->GetInt("height"));
	viewScale = glm::abs(cfg->GetFloat("window_scale"));
	vsync = cfg->GetInt("vsync") != 0 ? 1 : 0;
	CpuRender = cfg->GetInt("cpu_render");
}

void Game::Init()
{
	if (shaderGame == nullptr)
	{
		shaderGame = new Shader();
		shaderGame->LoadFromFiles("VertexShader.hlsl", "FragmentRaycasting4d.hlsl");
	}

	if (shaderUi == nullptr)
	{
		shaderUi = new Shader();
		shaderUi->LoadFromFiles("VertexShader.hlsl", "FragmentShader.hlsl");
	}
	

	glUseProgram(shaderGame->ID);

	GLint loc = glGetUniformLocation(shaderGame->ID, "gameResolution");
	glUniform2i(loc, viewWidth, viewHeight);

	int AntiAliasingEnabled = cfg->GetInt("anti_aliasing");
	loc = glGetUniformLocation(shaderGame->ID, "AntiAliasingEnabled");
	glUniform1i(loc, AntiAliasingEnabled);

	CpuRender = cfg->GetInt("cpu_render");
	loc = glGetUniformLocation(shaderGame->ID, "CpuRender");
	glUniform1i(loc, CpuRender);

	Texture::TEX_SIZE = cfg->GetInt("cube_pixels");
	Texture::BORDER_SIZE = cfg->GetInt("border_pixels");
	Texture::TEX_SMOOTHERING_FLAG = cfg->GetBool("texture_smoothering");

	glm::ivec4 mazeSize = glm::ivec4(
		glm::max(glm::abs(cfg->GetInt("maze_size_x")), 1),
		glm::max(glm::abs(cfg->GetInt("maze_size_y")), 1),
		glm::max(glm::abs(cfg->GetInt("maze_size_z")), 1),
		glm::max(glm::abs(cfg->GetInt("maze_size_w")), 1));
	int mazeRoomSize = glm::max(glm::abs(cfg->GetInt("maze_room_size")), 2);

	Maze maze(mazeSize);
	maze.Generate();

	field = new Field(glm::ivec4(
		mazeSize.x * mazeRoomSize + 1, // +1 - map positive borders
		mazeSize.y * mazeRoomSize + 1,
		mazeSize.z * mazeRoomSize + 1,
		mazeSize.w * mazeRoomSize + 1),
		glm::abs(cfg->GetInt("light_dist")),
		mazeRoomSize);

	field->Init(&maze, shaderGame);

	Cube::Init(shaderGame);

	player.Init(field, cfg->GetBool("ground_rotation"));

	renderer = new Renderer(&player, field, &raycaster, cfg->GetInt("multithreading") != 0, cfg->GetInt("skip_pixels") != 0);

	raycaster.Init(field);

	playerController = new PlayerController(cfg->GetFloat("speed"), cfg->GetFloat("mouse_sens"), &player);

	mainScene = new GameGraphics(shaderGame, -1.0f, -1.0f, 2.0f, 2.0f);
	UserInterface = new GameGraphics(shaderUi, -1.0f, -1.0f, 2.0f, 2.0f);
	helperScene1 = new GameGraphics(shaderGame, 0.6f, 0.55f, 0.35f, 0.35f);
	helperScene2 = new GameGraphics(shaderGame, 0.6f, 0.15f, 0.35f, 0.35f);


}

void Game::ReinitVideoConfig()
{
	viewWidth = glm::abs(cfg->GetInt("width"));
	viewHeight = glm::abs(cfg->GetInt("height"));
	viewScale = glm::abs(cfg->GetFloat("window_scale"));
	vsync = cfg->GetInt("vsync") != 0 ? 1 : 0;
	
}

void Game::ApplyNewParameters()
{
	if (Texture::TEX_SIZE != cfg->GetInt("cube_pixels") ||
		Texture::BORDER_SIZE != cfg->GetInt("border_pixels") ||
		Texture::TEX_SMOOTHERING_FLAG != cfg->GetBool("texture_smoothering"))
	{
		Texture::TEX_SIZE = cfg->GetInt("cube_pixels");
		Texture::BORDER_SIZE = cfg->GetInt("border_pixels");
		Texture::TEX_SMOOTHERING_FLAG = cfg->GetBool("texture_smoothering");

		Cube::Init(shaderGame);
	}

	if (playerController != nullptr)
		delete playerController;
	if (renderer != nullptr)
		delete renderer;

	//cfg = new Config();
	renderer = new Renderer(&player, field, &raycaster, cfg->GetInt("multithreading") != 0, cfg->GetInt("skip_pixels") != 0);
	playerController = new PlayerController(cfg->GetFloat("speed"), cfg->GetFloat("mouse_sens"), &player);
	player.groundRotation = cfg->GetBool("ground_rotation");


	Random::GetInstance()->Init(cfg->GetInt("seed"));

	viewScale = glm::abs(cfg->GetFloat("window_scale"));
	vsync = cfg->GetInt("vsync") != 0 ? 1 : 0;
	
	
	int newViewWidth = glm::abs(cfg->GetInt("width"));
	int newViewHeight = glm::abs(cfg->GetInt("height"));

	if (newViewHeight != viewHeight || newViewWidth != viewWidth)
		NeedReconfigureResolution = true;

	glUseProgram(shaderGame->ID);

	int AntiAliasingEnabled = cfg->GetInt("anti_aliasing");
	GLuint loc = glGetUniformLocation(shaderGame->ID, "AntiAliasingEnabled");
	glUniform1i(loc, AntiAliasingEnabled);

	CpuRender = cfg->GetInt("cpu_render");
	loc = glGetUniformLocation(shaderGame->ID, "CpuRender");
	glUniform1i(loc, CpuRender);

}

void Game::NewGame()
{
	if (playerController != nullptr)
		delete playerController;
	if (field != nullptr)
		delete field;
	if (renderer != nullptr)
		delete renderer;

	Init();	
}

void Game::UpdateShaderPlayer(Player curPlayer)
{
	glUseProgram(shaderGame->ID);

	GLint loc = glGetUniformLocation(shaderGame->ID, "vx");
	glUniform4f(loc, curPlayer.vx.x, curPlayer.vx.y, curPlayer.vx.z, curPlayer.vx.w);

	loc = glGetUniformLocation(shaderGame->ID, "vy");
	glUniform4f(loc, curPlayer.vy.x, curPlayer.vy.y, curPlayer.vy.z, curPlayer.vy.w);

	loc = glGetUniformLocation(shaderGame->ID, "vz");
	glUniform4f(loc, curPlayer.vz.x, curPlayer.vz.y, curPlayer.vz.z, curPlayer.vz.w);

	loc = glGetUniformLocation(shaderGame->ID, "vw");
	glUniform4f(loc, curPlayer.vw.x, curPlayer.vw.y, curPlayer.vw.z, curPlayer.vw.w);

	loc = glGetUniformLocation(shaderGame->ID, "pos");
	glUniform4f(loc, curPlayer.pos.x, curPlayer.pos.y, curPlayer.pos.z, curPlayer.pos.w);
}

void Game::Render(uint8_t* buffer)
{
	if (CpuRender > 0)
		renderer->FillTexData(buffer, viewWidth, viewHeight);
}

void Game::DrawScene(uint8_t* buffer)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	UpdateShaderPlayer(player);
	mainScene->Draw(buffer, viewWidth, viewHeight);

	if (cfg->GetBool("show_w-rearviews"))
	{
		//create empty texture with 1 transparent pixel
		uint8_t* emptyBuffer = new uint8_t[4];
		memset(emptyBuffer, 0, 4);

		Player wPlayer1 = player;
		wPlayer1.RotateZW(90);
		UpdateShaderPlayer(wPlayer1);
		helperScene1->Draw(emptyBuffer, 1, 1);

		Player wPlayer2 = player;
		wPlayer2.RotateYW(90);
		UpdateShaderPlayer(wPlayer2);
		helperScene2->Draw(emptyBuffer, 1, 1);
	}

	UserInterface->Draw(buffer, viewWidth, viewHeight);
}