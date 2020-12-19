#include <Game.h>

Game::Game()
{
	cfg = new Config();

	Random::GetInstance()->Init(cfg->GetInt("seed"));

	viewWidth = glm::abs(cfg->GetInt("width"));
	viewHeight = glm::abs(cfg->GetInt("height"));
	viewScale = glm::abs(cfg->GetFloat("window_scale"));
	vsync = cfg->GetInt("vsync") != 0 ? 1 : 0;
}

void Game::Init()
{
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
	field->Init(&maze);

	Cube::Init();

	player.Init(field);

	renderer = new Renderer(&player, field, &raycaster, cfg->GetInt("multithreading") != 0, cfg->GetInt("skip_pixels") != 0);

	raycaster.Init(field);

	playerController = new PlayerController(cfg->GetFloat("speed"), cfg->GetFloat("mouse_sens"), &player);
}

void Game::Render(uint8_t* buffer)
{
	renderer->FillTexData(buffer, viewWidth, viewHeight);
}