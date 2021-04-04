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
		LightCubeField->CreateCube(0, 0, 0, 0, Field::StdLightCell);
		LightCubeField->Init(game->cfg, -0.0f, -0.9f, 0.3f, 0.3f*ratio);
		//LightCubeField->Init(game->cfg, -01.0f, -01.0f, 2.0f, 2.0f);
		LightCubeField->SetBackgroundColor(255.0f / 255.0f, 252.0f / 255.0f, 229.0f / 255.0f, 0.0f);
		//LightCubeField->SetBackgroundColor(0.0f, 0.0f, 0.0f, 0.0f);

		LightCubeField->SetAntialiasing(2);
		//LightCubeField->SetResolution(100 * 1024 / 768, 100);
		//LightCubeField->SetMaxRayOutDistance(5);

		MapBordersField = new Field(glm::ivec4(3, 3, 3, 3), 16, game->cfg);
		MapBordersField->CreateBorders();
		MapBordersField->CreateCube(1, 1, 1, 1, Cell(LIGHT_BLOCK, 0, false));
		MapBordersField->Init(game->cfg, -1, -1, 2, 2);
		MapBordersField->SetBackgroundColor(0.0f, 0.0f, 0.0f, 0.0f);
		//MapBorders->SetBackgroundColor(1.0f, 0.5f, 0.5f);

		if (OneCubeField != nullptr)
			delete OneCubeField;

		OneCubeField = new Field(glm::ivec4(3, 3, 3, 3), 16, game->cfg);
		for (int i = 0; i < OneCubeField->GetTotalSize(); i++)
			OneCubeField->CreateCube(i, Cell(LIGHT_BLOCK, 0, false)); //transparent light block for purpose
		OneCubeField->CreateCube(1, 1, 1, 1);
		OneCubeField->Init(game->cfg, -0.2f, -0.9f, 0.3f, 0.3f*ratio);
		//OneCubeField->Init(game->cfg, -1, -1, 2, 2);
		OneCubeField->SetBackgroundColor(0.0f, 0.0f, 0.0f, 0.0f);
		OneCubeField->SetAntialiasing(2);

		gameGraphics = new RectangleGraphics(shader, -0.005f, -0.005f, 0.01f, 0.01f);
		gameGraphics->InitSelfShader();
		
	}
	
	void AddCube(Cell cell = Cell(1, 255, false), bool isNear = false)
	{
		glm::ivec4 block = isNear ?
			FindFrontCell(&game->player) :
			FindFrontCellByRay(&game->player, true);

		if (!game->field->IsCubeIndexValid(block.x, block.y, block.z, block.w))
			return;

		game->field->CreateCube(block.x, block.y, block.z, block.w, cell);
	}

	void DeleteCube()
	{
		glm::ivec4 block = FindFrontCellByRay(&game->player);

		if (!game->field->IsCubeIndexValid(block.x, block.y, block.z, block.w))
			return;

		Cell_t cell = new Cell();
		game->field->CreateCube(block.x, block.y, block.z, block.w, Field::StdEmptyCell);
	}

	void ChangeBlockTransparency(float value)
	{
		glm::ivec4 map = FindFrontCellByRay(&game->player);

		if (!game->field->IsCubeIndexValid(map.x, map.y, map.z, map.w))
			return;

		int index = game->field->GetIndex(map.x, map.y, map.z, map.w);
		Cell cell = game->field->GetCube(map);

		if (cell.cellType == EMPTY_BLOCK)
			return;

		float fAlpha = (float)cell.alphaValue + value;
		int alpha;
		alpha = (int)std::max(0.0f, fAlpha);
		if (cell.cellType != LIGHT_BLOCK)
			alpha = (int)std::max(30.0f, fAlpha);
		alpha = (int)std::min(255, alpha);
		cell.alphaValue = alpha;
		game->field->CreateCube(map, Cell(cell));
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
				game->field->CreateCube(i, Cell());
			game->field->CreateBorders();
		}
		if (key == GLFW_KEY_INSERT && action == GLFW_PRESS)
		{
			game->field->CreateBorders();
			game->field->LoadFieldToGL();
		}
		if (key == GLFW_KEY_PAGE_UP && action == GLFW_PRESS)
		{
			ChangeBlockTransparency(10.0f);
		}
		if (key == GLFW_KEY_PAGE_DOWN && action == GLFW_PRESS)
		{
			ChangeBlockTransparency(-10.0f);
		}
	}
	virtual void OnMouseButtonInput(GLFWwindow* window, int button, int action, int mods)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			AddCube(Cell(activeAddingCell));
		}
		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		{
			DeleteCube();
		}

		if ((button == GLFW_MOUSE_BUTTON_MIDDLE || button == GLFW_MOUSE_BUTTON_4) &&
			action == GLFW_PRESS)
		{
			AddCube(Cell(activeAddingCell), true);
		}

	}
	virtual void OnMouseInput(GLFWwindow* window, double xpos, double ypos) {}
	virtual void OnScrollInput(GLFWwindow* window, double xoffset, double yoffset)
	{
		ChangeBlockTransparency((float)yoffset * 10.0f);
	}
	virtual void FreezeController() {}
	virtual void UnFreezeController() {}

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

	glm::ivec4 FindFrontCellByRay(Player* player, bool findPrevious = false)
	{
		const int MAX_DIST = 15; // distance in blocks

		glm::vec4 v = player->rotationMatrix.vx;
		//int edge = 0;

		//which box of the map we're in
		glm::ivec4 block(player->pos);
		glm::ivec4 prevBlock(-1);

		//length of ray from current position to next x or y-side
		glm::vec4 sideDist;

		//length of ray from one x or y-side to next x or y-side
		glm::vec4 deltaDist(glm::abs(1.0f / v.x), glm::abs(1.0f / v.y), glm::abs(1.0f / v.z), glm::abs(1.0f / v.w));

		//what direction to step in x or y-direction (either +1 or -1)
		glm::i8vec4 step;

		//calculate step and initial sideDist
		if (v.x < 0)
		{
			step.x = -1;
			sideDist.x = (player->pos.x - block.x) * deltaDist.x;
		}
		else
		{
			step.x = 1;
			sideDist.x = (block.x + 1.0f - player->pos.x) * deltaDist.x;
		}
		if (v.y < 0)
		{
			step.y = -1;
			sideDist.y = (player->pos.y - block.y) * deltaDist.y;
		}
		else
		{
			step.y = 1;
			sideDist.y = (block.y + 1.0f - player->pos.y) * deltaDist.y;
		}
		if (v.z < 0)
		{
			step.z = -1;
			sideDist.z = (player->pos.z - block.z) * deltaDist.z;
		}
		else
		{
			step.z = 1;
			sideDist.z = (block.z + 1.0f - player->pos.z) * deltaDist.z;
		}
		if (v.w < 0)
		{
			step.w = -1;
			sideDist.w = (player->pos.w - block.w) * deltaDist.w;
		}
		else
		{
			step.w = 1;
			sideDist.w = (block.w + 1.0f - player->pos.w) * deltaDist.w;
		}

		//perform DDA
		for (int dist = 0; dist < MAX_DIST; dist++)
		{
			//jump to next map square, OR in x-direction, OR in y-direction
			if (sideDist.x <= sideDist.y && sideDist.x <= sideDist.z && sideDist.x <= sideDist.w)
			{
				sideDist.x += deltaDist.x;
				block.x += step.x;
				//edge = player->pos.x < map.x ? NEG_X : POS_X;
			}
			else if (sideDist.y <= sideDist.x && sideDist.y <= sideDist.z && sideDist.y <= sideDist.w)
			{
				sideDist.y += deltaDist.y;
				block.y += step.y;
				//edge = player->pos.y < map.y ? NEG_Y : POS_Y;
			}
			else if (sideDist.z <= sideDist.x && sideDist.z <= sideDist.y && sideDist.z <= sideDist.w)
			{
				sideDist.z += deltaDist.z;
				block.z += step.z;
				//edge = player->pos.z < map.z ? NEG_Z : POS_Z;
			}
			else if (sideDist.w < sideDist.x && sideDist.w <= sideDist.y && sideDist.w <= sideDist.z)
			{
				sideDist.w += deltaDist.w;
				block.w += step.w;
				//edge = player->pos.w < map.w ? NEG_W : POS_W;
			}

			if (game->field->IsCubeIndexValid(block.x, block.y, block.z, block.w))
			{
				Cell cell = game->field->GetCube(block.x, block.y, block.z, block.w);
				if (cell.cellType != EMPTY_BLOCK)
					return findPrevious ? prevBlock : block;
			}
			else
			{
				break;
			}

			prevBlock = block;
		}

		return glm::ivec4(-1);
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

		game->field->SetSelectedBlock(FindFrontCellByRay(&game->player));

		gameGraphics->SetNewPosition(-0.005f, -0.005f, 0.01f, 0.01f);
		gameGraphics->Draw(255, 255, 255, 255);
	}

	Game* game = nullptr;
	Field* OneCubeField;

private:
	//OneShapeField* MapBorders;
	Field* MapBordersField;
	//Field* SimpleCubeField;
	Field* LightCubeField;
	//OneShapeField* SimpleCube;
	//OneShapeField* LightCube;
	RectangleGraphics* gameGraphics;
	Shader* shader;
	Cell activeAddingCell = Cell(1, 255, false);
};
