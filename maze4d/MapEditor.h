#pragma once


#include <Field.h>
#include <OneShapeField.h>


class MapEditor : public IInputController, IDrawable
{
public:
	MapEditor(Game* game) 
	{
		this->game = game;
		
	}

	void ClearShaders()
	{
		if (shader != nullptr)
			delete shader;
		shader = nullptr;
	}

	~MapEditor()
	{
		if (shader != nullptr)
			delete shader;
		shader = nullptr;
		if (OneCubeField != nullptr)
			delete OneCubeField;
		if (LightCubeField != nullptr)
			delete LightCubeField;
		if (MapBordersField != nullptr)
			delete MapBordersField;
	}

	void Init()
	{
		
		float ratio = (float)game->viewWidth / game->viewHeight;
		if (LightCubeField != nullptr)
			delete LightCubeField;

		LightCubeField = new Field(glm::ivec4(1, 1, 1, 1), 16, game->cfg);
		LightCubeField->CreateCube(0, 0, 0, 0, new Cell(Field::StdLightCell));
		LightCubeField->Init(game->cfg, -0.0f, -0.9f, 0.3f, 0.3f*ratio);
		//LightCubeField->Init(game->cfg, -01.0f, -01.0f, 2.0f, 2.0f);
		LightCubeField->SetBackgroundColor(255.0f / 255.0f, 252.0f / 255.0f, 229.0f / 255.0f, 0.0f);
		//LightCubeField->SetBackgroundColor(0.0f, 0.0f, 0.0f, 0.0f);

		LightCubeField->SetAntialiasing(2);
		//LightCubeField->SetResolution(100 * 1024 / 768, 100);
		//LightCubeField->SetMaxRayOutDistance(5);

		MapBordersField = new Field(glm::ivec4(3, 3, 3, 3), 16, game->cfg);
		MapBordersField->CreateBorders();
		MapBordersField->CreateCube(1, 1, 1, 1, new Cell(LIGHT_BLOCK, 0, false));
		MapBordersField->Init(game->cfg, -1, -1, 2, 2);
		MapBordersField->SetBackgroundColor(0.0f, 0.0f, 0.0f, 0.0f);
		//MapBorders->SetBackgroundColor(1.0f, 0.5f, 0.5f);

		if (OneCubeField != nullptr)
			delete OneCubeField;

		OneCubeField = new Field(glm::ivec4(3, 3, 3, 3), 16, game->cfg);
		for (int i = 0; i < OneCubeField->GetTotalSize(); i++)
			OneCubeField->CreateCube(i, new Cell(LIGHT_BLOCK, 0, false)); //transparent light block for purpose
		OneCubeField->CreateCube(1, 1, 1, 1);
		OneCubeField->Init(game->cfg, -0.2f, -0.9f, 0.3f, 0.3f*ratio);
		//OneCubeField->Init(game->cfg, -1, -1, 2, 2);
		OneCubeField->SetBackgroundColor(0.0f, 0.0f, 0.0f, 0.0f);
		OneCubeField->SetAntialiasing(2);

		gameGraphics = new GameGraphics(shader, -0.005, -0.005, 0.01, 0.01);
		gameGraphics->InitSelfShader();
		
	}
	
	void AddCube(Cell_t cell = new Cell(1, 255, false))
	{
		glm::ivec4 map = FindFrontCell(&game->player);

		if (!game->field->IsCubeIndexValid(map.x, map.y, map.z, map.w))
			return;

		//int idx = game->field->GetIndex(pos.x, pos.y, pos.z, pos.w);
		game->field->CreateCube(map.x, map.y, map.z, map.w, cell);
	}

	void DeleteCube()
	{
		glm::ivec4 map = FindFrontCell(&game->player);

		if (!game->field->IsCubeIndexValid(map.x, map.y, map.z, map.w))
			return;
		Cell_t cell = new Cell(); //emptyCell

		//int idx = game->field->GetIndex(pos.x, pos.y, pos.z, pos.w);
		game->field->CreateCube(map.x, map.y, map.z, map.w, nullptr);

	}

	virtual void OnKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods) 
	{
		if (key == GLFW_KEY_1 && action == GLFW_PRESS)
			activeAddingCell = OneCubeField->GetCube(1, 1, 1, 1);

		if (key == GLFW_KEY_2 && action == GLFW_PRESS)
			activeAddingCell = LightCubeField->GetCube(0,0,0,0);
		/*	*/
		if (key == GLFW_KEY_DELETE && action == GLFW_PRESS)
		{
			for (int i = 0; i < game->field->GetIndex(game->field->size - 1); i++)
				game->field->CreateCube(i, new Cell());
			game->field->CreateBorders();
		}
	}
	virtual void OnMouseButtonInput(GLFWwindow* window, int button, int action, int mods)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			AddCube(new Cell(activeAddingCell));
		}
		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		{
			DeleteCube();
		}

		if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
		{
			game->field->CreateBorders();
			game->field->LoadFieldToGL();
		}

	}
	virtual void OnMouseInput(GLFWwindow* window, double xpos, double ypos) {}
	virtual void OnScrollInput(GLFWwindow* window, double xoffset, double yoffset) 
	{
		glm::ivec4 map = FindFrontCell(&game->player);

		if (!game->field->IsCubeIndexValid(map.x, map.y, map.z, map.w))
			return;

		int index = game->field->GetIndex(map.x, map.y, map.z, map.w);
		Cell cell = game->field->GetCube(map);

		if (cell.cellType == EMPTY_BLOCK)
			return;

		float fAlpha = (float)cell.alphaValue + 10 * (float) yoffset;
		int alpha;
		alpha = (int)std::max(0.0f, fAlpha);
		if(cell.cellType != LIGHT_BLOCK)
			alpha = (int) std::max(30.0f, fAlpha);
		alpha = (int) std::min(255, alpha);
		cell.alphaValue = alpha;
		game->field->CreateCube(map, new Cell(cell));

	}
	virtual void FreezeController() {}
	virtual void UnFreezeController() {}

	Game* game = nullptr;

	glm::ivec4 FindFrontCell(Player* player)
	{
		glm::vec4 pos = game->player.pos;
		glm::ivec4 map(pos);
		while (map == glm::ivec4(pos))
		{
			pos += game->player.rotationMatrix.vx;
		}
		map = glm::ivec4(pos);

		return map;
	}

	virtual void Draw()
	{
		float Dist = 0.0f;
		Player curPlayer = Player();
		curPlayer.ResetBasis();
		RotationMatrix mat = RotationMatrix();

		mat = game->player.rotationMatrix;

		curPlayer = game->player;
		curPlayer.pos.x = game->player.pos.x / (game->field->size.x) + 1;
		curPlayer.pos.y = game->player.pos.y / (game->field->size.y) + 1;
		curPlayer.pos.z = game->player.pos.z / (game->field->size.z) + 1;
		curPlayer.pos.w = game->player.pos.w / (game->field->size.w) + 1;

		MapBordersField->SetCameraView(&curPlayer);
		//MapBordersField->Draw();

		gameGraphics->SetNewPosition(-1, -1, 2, 2);
		//gameGraphics->Draw(2, 2, 2, 210);

		//game->Draw();

		curPlayer.ResetBasis();
		curPlayer.rotationMatrix.vx = game->player.rotationMatrix.vx;
		curPlayer.rotationMatrix.vy = game->player.rotationMatrix.vy;
		curPlayer.rotationMatrix.vz = game->player.rotationMatrix.vz;
		curPlayer.rotationMatrix.vw = game->player.rotationMatrix.vw;

		mat = curPlayer.rotationMatrix;

		Dist = 2;
		if (activeAddingCell.cellType == OneCubeField->GetCube(1, 1, 1, 1).cellType)
			Dist = 1.5;
		curPlayer.pos = glm::vec4(-Dist *  mat.vx.x + 1.5f, -Dist *  mat.vx.y + 1.5f, -Dist * mat.vx.z + 1.5f, -Dist *  mat.vx.w + 1.5f);
		OneCubeField->SetCameraView(&curPlayer);
		OneCubeField->Draw();
		//MapBordersField->SetCameraView(&curPlayer);
		//MapBordersField->Draw();

		Dist = 2;
		if (activeAddingCell.cellType == LightCubeField->GetCube(0, 0, 0, 0).cellType)
			Dist = 1.5;
		curPlayer.pos = glm::vec4(-Dist *  mat.vx.x + 0.5f, -Dist *  mat.vx.y + 0.5f, -Dist *  mat.vx.z + 0.5f, -Dist *  mat.vx.w + 0.5f);
		LightCubeField->SetCameraView(&curPlayer);
		LightCubeField->Draw();

		gameGraphics->SetNewPosition(-0.005, -0.005, 0.01, 0.01);
		gameGraphics->Draw(255, 255, 255, 255);

	}

	Field* OneCubeField;

private:
	//OneShapeField* MapBorders;
	Field* MapBordersField;
	//Field* SimpleCubeField;
	Field* LightCubeField;
	//OneShapeField* SimpleCube;
	//OneShapeField* LightCube;
	GameGraphics* gameGraphics;
	Shader* shader;
	Cell activeAddingCell = Cell(1, 255, false);
};
