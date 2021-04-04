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

	MazeField* mazeField = new MazeField();
	mazeField->GenerateMaze(mazeSize);
	glm::ivec4 fieldSize = glm::ivec4(
		mazeSize.x * mazeRoomSize + 1, // +1 - map positive borders
		mazeSize.y * mazeRoomSize + 1,
		mazeSize.z * mazeRoomSize + 1,
		mazeSize.w * mazeRoomSize + 1);
	mazeField->GenerateField(fieldSize, glm::abs(cfg->GetInt("light_dist")), mazeRoomSize, cfg);

	field = mazeField->field;
	field->Init(this->cfg, -1, -1, 2, 2);

	player.defaultPos = glm::vec4(mazeRoomSize / 2.0f + 0.2f);
	player.Init(field, cfg->GetBool("ground_rotation"));
	
	playerController = new PlayerController(cfg->GetFloat("speed"), cfg->GetFloat("mouse_sens"), &player, field);
//	raycaster.Init(field);	
}

void Game::ReinitVideoConfig()
{
	viewWidth = glm::abs(cfg->GetInt("width"));
	viewHeight = glm::abs(cfg->GetInt("height"));
	viewScale = glm::abs(cfg->GetFloat("window_scale"));
	vsync = cfg->GetInt("vsync") != 0 ? 1 : 0;
	glfwSwapInterval(vsync);
}

void Game::ApplyNewParameters()
{

	if (playerController != nullptr)
		delete playerController;
	playerController = new PlayerController(cfg->GetFloat("speed"), cfg->GetFloat("mouse_sens"), &player, field);
	player.groundRotation = cfg->GetBool("ground_rotation");

	Random::GetInstance()->Init(cfg->GetInt("seed"));

	vsync = cfg->GetInt("vsync") != 0 ? 1 : 0;
	glfwSwapInterval(vsync);
	
	int newViewWidth = glm::abs(cfg->GetInt("width"));
	int newViewHeight = glm::abs(cfg->GetInt("height"));

	if (newViewHeight != viewHeight || newViewWidth != viewWidth)
		NeedReconfigureResolution = true;

	field->ReadConfig(cfg);
}

void Game::NewGame()
{
	if (playerController != nullptr)
		delete playerController;
	if (field != nullptr)
		delete field;

	Init();	
}

void Game::NewEditor()
{
	if (playerController != nullptr)
		delete playerController;
	if (field != nullptr)
		delete field;

	int roomSize = glm::max(glm::abs(cfg->GetInt("editor_room_size")), 2);
	roomSize++; //+1 positive borders
	glm::ivec4 fieldSize = glm::ivec4(roomSize, roomSize, roomSize, roomSize);
	int lightDist = glm::abs(cfg->GetInt("editor_light_dist"));


	field = new Field(fieldSize, lightDist, this->cfg);
	field->Init(this->cfg, -1, -1, 2, 2);
	field->CreateCube(glm::vec4(roomSize / 2.0f + 0.2f), Field::StdLightCell);

	player.defaultPos = glm::vec4(roomSize / 2.0f + 0.2f);
	player.Init(field, cfg->GetBool("ground_rotation"));

	playerController = new PlayerController(cfg->GetFloat("speed"), cfg->GetFloat("mouse_sens"), &player, field);
	//	raycaster.Init(field);	
}

void Game::Draw()
{	
	field->SetFrameSize(- 1, -1, 2, 2);
	field->SetCameraView(&player);
	field->Draw();
	//mainScene->Draw(emptyBuffer,1,1);


	if (cfg->GetBool("show_w-rearviews"))
	{
		Player wPlayer1 = player;
		wPlayer1.RotateZW(90);
		field->SetFrameSize(0.6f, 0.55f, 0.35f, 0.35f);
		field->SetCameraView(&wPlayer1);
		field->Draw();
		//helperScene1->Draw(emptyBuffer, 1, 1);

		Player wPlayer2 = player;
		wPlayer2.RotateYW(90);
		field->SetFrameSize(0.6f, 0.15f, 0.35f, 0.35f);
		field->SetCameraView(&wPlayer2);
		field->Draw();		
		//helperScene2->Draw(emptyBuffer, 1, 1);
	}

}