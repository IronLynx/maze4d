#pragma once

#include <Utils.h>
#include <Player.h>

class PlayerController : public IInputController
{
public:
	PlayerController(const float moveSpeed, const float mouseSens, Player* player, Field* field = nullptr)
		: moveSpeed(moveSpeed), mouseSens(mouseSens), player(player)
	{}

	void Update(double delta)
	{
		static const moveFunc_t moveFuncs[4] = {
			&Player::MoveX, &Player::MoveY, &Player::MoveZ, &Player::MoveW };
		static const rotateFunc_t rotateFuncs[6] = {
			&Player::RotateXY, &Player::RotateXZ, &Player::RotateXW,
			&Player::RotateYZ, &Player::RotateYW, &Player::RotateZW };

		for (size_t i = 0; i < isMove.size(); i++)
			if (isMove[i])
			{
				int sign = i % 2 == 0 ? 1 : -1;
				(player->*moveFuncs[i/2])(float(moveSpeed * delta), sign);
			}

		for (size_t i = 0; i < isRotate.size(); i++)
			if (isRotate[i])
			{
				int sign = i % 2 == 0 ? 1 : -1;
				(player->*rotateFuncs[i/2])(float(rotateSpeed * delta * sign));
			}
	}

	virtual void OnMouseInput(GLFWwindow* window, double xpos, double ypos) 
	{
		if (isFirstCall)
		{
			isFirstCall = false;
			relX = 0;
			relY = 0;
		}

		else
		{
			relX = xpos - lastXpos;
			relY = ypos - lastYpos;
		}
		if (isMouseRotateW)
		{
			player->RotateXW(float(-relY*mouseSens / 10));
			player->RotateZW(float(relX*mouseSens / 10));
		}
		else
		{
			player->RotateXY(float(-relY*mouseSens / 10));
			player->RotateXZ(float(relX*mouseSens / 10));
		}

		lastXpos = xpos;
		lastYpos = ypos;
	}

	virtual void OnKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods) 
	{

		static std::array<int, 4 * 2> moveButtons = {
			GLFW_KEY_W, GLFW_KEY_S,
			GLFW_KEY_R, GLFW_KEY_F,
			GLFW_KEY_D, GLFW_KEY_A,
			GLFW_KEY_T, GLFW_KEY_G
		};
		static std::array<int, 6 * 2> rotateButtons = {
			GLFW_KEY_I, GLFW_KEY_K,
			GLFW_KEY_L, GLFW_KEY_J,
			GLFW_KEY_O, GLFW_KEY_U,
			GLFW_KEY_E, GLFW_KEY_Q,
			GLFW_KEY_Z, GLFW_KEY_C,
			GLFW_KEY_M, GLFW_KEY_N
		};

		auto fillMove = [&](int num)
		{
			if (key == moveButtons[num])
			{
				if (action == GLFW_PRESS)
					isMove[num] = true;
				if (action == GLFW_RELEASE)
					isMove[num] = false;
			}
		};

		auto fillRotate = [&](int num)
		{
			if (key == rotateButtons[num])
			{
				if (action == GLFW_PRESS)
					isRotate[num] = true;
				if (action == GLFW_RELEASE)
					isRotate[num] = false;
			}
		};

		for (size_t i = 0; i < moveButtons.size(); i++)
			fillMove(i);
		for (size_t i = 0; i < rotateButtons.size(); i++)
			fillRotate(i);

		if (key == GLFW_KEY_LEFT_SHIFT)
		{
			if (action == GLFW_PRESS)
				isMouseRotateW = true;
			else if (action == GLFW_RELEASE)
			{
				isMouseRotateW = false;
			}
		}

		if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
			player->AlignRotation();

		if (key == GLFW_KEY_P && action == GLFW_PRESS)
			player->Reset();

		if (key == GLFW_KEY_F8 && action == GLFW_PRESS)
			player->noclip = !player->noclip;

		if (key == GLFW_KEY_B && action == GLFW_PRESS)
			player->ResetBasis();
	}

	virtual void OnScrollInput(GLFWwindow* window, double xoffset, double yoffset)
	{
		//player->RotateZW(float(-yoffset*90.0f));		
	}

	virtual void FreezeController() 
	{ 
		isFirstCall = true; 
		isMouseRotateW = false;
	}

	Player* player = nullptr;
	const float moveSpeed;
	const float rotateSpeed = 80.0f; // only for buttons
	const float mouseSens;

private:
	bool isMouseRotateW = false;
	std::array<bool, 4 * 2> isMove = { false };
	std::array<bool, 6 * 2> isRotate = { false };
	typedef void (Player::*moveFunc_t)(float, int);
	typedef void (Player::*rotateFunc_t)(float);

	bool isFirstCall = true;
	double lastXpos = 0, lastYpos = 0, relX = 0, relY = 0;
};