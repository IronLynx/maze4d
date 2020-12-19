#pragma once

#include <Utils.h>
#include <Player.h>

class PlayerController
{
public:
	PlayerController(const float moveSpeed, const float mouseSens, Player* player)
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

	Player* player = nullptr;
	const float moveSpeed;
	const float rotateSpeed = 80.0f; // only for buttons
	const float mouseSens;
	bool isMouseRotateW = false;
	std::array<bool, 4 * 2> isMove = { false };
	std::array<bool, 6 * 2> isRotate = { false };
	typedef void (Player::*moveFunc_t)(float, int);
	typedef void (Player::*rotateFunc_t)(float);
};