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

void Game::Init(Shader* shader)
{
	this->shader = shader;

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

	field->Init(&maze, shader);

	CpuRender = cfg->GetInt("cpu_render");

	int AntiAliasingEnabled = cfg->GetInt("anti_aliasing");
	int loc = glGetUniformLocation(shader->ID, "AntiAliasingEnabled");
	glUniform1i(loc, AntiAliasingEnabled);

	Cube::Init(shader);

	player.Init(field, cfg->GetBool("ground_rotation"));

	renderer = new Renderer(&player, field, &raycaster, cfg->GetInt("multithreading") != 0, cfg->GetInt("skip_pixels") != 0);

	raycaster.Init(field);

	playerController = new PlayerController(cfg->GetFloat("speed"), cfg->GetFloat("mouse_sens"), &player);
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

		Cube::Init(shader);
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
	CpuRender = cfg->GetInt("cpu_render");
	
	int newViewWidth = glm::abs(cfg->GetInt("width"));
	int newViewHeight = glm::abs(cfg->GetInt("height"));

	if (newViewHeight != viewHeight || newViewWidth != viewWidth)
		NeedReconfigureResolution = true;

	int AntiAliasingEnabled = cfg->GetInt("anti_aliasing");
	int loc = glGetUniformLocation(shader->ID, "AntiAliasingEnabled");
	glUniform1i(loc, AntiAliasingEnabled);

}

void Game::NewGame()
{
	if (playerController != nullptr)
		delete playerController;
	if (field != nullptr)
		delete field;
	if (renderer != nullptr)
		delete renderer;

	Init(shader);	
}

void Game::Render(uint8_t* buffer)
{
	renderer->FillTexData(buffer, viewWidth, viewHeight);	
}