#include <Player.h>



void Player::Init(Field* field, bool groundRotation)
{
	this->field = field;
	Reset();
	this->groundRotation = groundRotation;

	ResetBasis();
}

void Player::Reset()
{
	ResetBasis();
	basisMatrix.Reset();

	//player orientation
	angleXY = 0.0f; //-90f - 90f acceptable values, this is up-down rotation angle
	angleXZ = 0.0f; //free for all rotation left-right
	angleYZ = 0.0f; //roll
	angleXW = 0.0f;
	angleZW = 0.0f;
	angleYW = 0.0f;

	pos = defaultPos; //glm::vec4(2.0f / 2.0f + 0.2f);
	

	SetCurrentRotation();
}

void Player::ResetBasis()
{
	basisMatrix.Reset();

	ResetToBasis();

	angleYZ = 0; //cancel Roll

	angleXW = 0.0f;
	angleZW = 0.0f;
	angleYW = 0.0f;

	SetCurrentRotation();
}

void Player::ResetToBasis()
{
	rotationMatrix = basisMatrix;
}

void Player::RebaseToCurrent()
{
	basisMatrix = rotationMatrix;
}

void Player::SetCurrentRotation()
{
	if (groundRotation)
	{
		ResetToBasis();

		//pitch angle restriction
		angleXY = std::max(-90.0f, angleXY);
		angleXY = std::min(90.0f, angleXY);

		//Prioritized rotation inside current 3d-slice	
		rotationMatrix.RotateXZ(angleXZ);
		rotationMatrix.RotateXY(angleXY);
	}
}

void Player::AlignRotation()
{
	if (!groundRotation)
		RebaseToCurrent();

	basisMatrix.RoundAngles();
	ResetToBasis();

	//Just for interface visualization
	//No logic bound to these W-angles
	angleXW = 90.0f * basisMatrix.vw.x;
	angleYW = 90.0f * basisMatrix.vw.y;
	angleZW = 90.0f * basisMatrix.vw.z;

	angleYZ = round(angleYZ / 90.0f) * 90.0f;

	SetCurrentRotation();
}

void Player::SetNewPos(glm::vec4 v, float delta, int sign)
{
	glm::vec4 vx = glm::vec4(v.x, 0.0f, 0.0f, 0.0f);
	MoveAndHandleEvent(vx, delta, sign);

	glm::vec4 vy = glm::vec4(0.0f, v.y, 0.0f, 0.0f);
	MoveAndHandleEvent(vy, delta, sign);

	glm::vec4 vz = glm::vec4(0.0f, 0.0f, v.z, 0.0f);
	MoveAndHandleEvent(vz, delta, sign);

	glm::vec4 vw = glm::vec4(0.0f, 0.0f, 0.0f, v.w);
	MoveAndHandleEvent(vw, delta, sign);
}

void Player::MoveAndHandleEvent(glm::vec4 v, float delta, int sign)
{
	float safeDist;
	//Cell_t collideCell;
	//int res = Raycaster::FindCollision(lastPos, v*(float)sign, delta, safeDist, collideCell, noclip, field);
	int res = FindCollision(v*(float)sign, delta, safeDist);

	safeDist = glm::min(safeDist, delta);
	pos += v*safeDist*(float)sign;

	//glorious victory
	if (res == RAY_COLLIDE_WIN_BLOCK)
	{
		WinField::CreateWinRoom(field);
		//~(*field);
		//*field = *winField;
		//field->CreateWinRoom();
		Reset();
		pos = glm::vec4(1.01f, 6.99f, 1.01f, 3.5f);
		defaultPos = pos;
		//lastPos = pos;
		RotateXZ(45.0f);
		RotateXY(-30.0f);
		noclip = false;
	}

	//lastPos = pos;
}

int Player::FindCollision(glm::vec4 v, float targetDist, float& safeDist)
{
	glm::vec4 tmp;
	glm::ivec4 map(tmp);
//	int index;
	Cell cell;
	float step = 0.1f;
	for (float i = 0.0f; ; i += step)
	{
		tmp = pos + v*i;
		map = floor(tmp);

		if (!field->IsCubeIndexValid(map.x, map.y, map.z, map.w))
		{
			safeDist = i - step;
			if (noclip)
				safeDist = targetDist;
			return RAY_COLLIDE_MAP_BORDER; //collision with map border
		}

		cell = field->GetCube(map);

		if (!noclip)
		{
			// Check if ray has hit a wall		
			if (cell.cellType == WALL_BLOCK)
			{
				safeDist = i - step;
				return RAY_COLLIDE_BLOCK; //collision with block
			}

			//glorious victory
			if (cell.isWinBlock)
				return RAY_COLLIDE_WIN_BLOCK;
		}

		if (i > targetDist)
		{
			safeDist = targetDist;
			return RAY_NO_COLLISION; //no collision detected
		}
	}
}

void Player::MoveX(float d, int sign) { SetNewPos(rotationMatrix.vx, d, sign); }
void Player::MoveY(float d, int sign) { SetNewPos(rotationMatrix.vy, d, sign); }
void Player::MoveZ(float d, int sign) { SetNewPos(rotationMatrix.vz, d, sign); }
void Player::MoveW(float d, int sign) { SetNewPos(rotationMatrix.vw, d, sign); }

#define IF_GRND if (groundRotation)

//3d-roll
void Player::RotateYZ(float a)
{
	rotationMatrix.RotateYZ(a);
	AddToAngleDegree(angleYZ, a);

	IF_GRND rotationMatrix.RotateXY(-angleXY); //MouseY, up and down rotation
	IF_GRND rotationMatrix.RotateXZ(-angleXZ); //MouseX, rotate left and right

	IF_GRND RebaseToCurrent();
	SetCurrentRotation();
}

//MouseY
void Player::RotateXY(float a)
{
	AddToAngleDegree(angleXY, a);
	rotationMatrix.RotateXY(a);
	SetCurrentRotation();
}

//MouseX
void Player::RotateXZ(float a)
{
	AddToAngleDegree(angleXZ, a);
	rotationMatrix.RotateXZ(a);
	SetCurrentRotation();
}

//4d-roll
void Player::RotateYW(float a)
{
	IF_GRND ResetToBasis();

	AddToAngleDegree(angleYW, a);
	rotationMatrix.RotateYW(a);

	IF_GRND RebaseToCurrent();
	SetCurrentRotation();
}

//Shift+MouseX
void Player::RotateZW(float a)
{
	IF_GRND rotationMatrix.RotateXY(-angleXY); //MouseX, rotate left and right

	AddToAngleDegree(angleZW, a);
	rotationMatrix.RotateZW(a);

	IF_GRND rotationMatrix.RotateXZ(-angleXZ); //MouseX, rotate left and right

	IF_GRND RebaseToCurrent();
	SetCurrentRotation();
}

//Shift+MouseY
void Player::RotateXW(float a)
{
	IF_GRND rotationMatrix.RotateXY(-angleXY); //MouseX, rotate left and right

	AddToAngleDegree(angleXW, a);
	rotationMatrix.RotateXW(a);

	IF_GRND rotationMatrix.RotateXZ(-angleXZ); //MouseX, rotate left and right

	IF_GRND RebaseToCurrent();
	SetCurrentRotation();
}

//Adds degree to angle and restricts it from -180 to +180 degrees
//Does not make any matrix rotations
void Player::AddToAngleDegree(float& axisAngle, float degree)
{
	axisAngle += degree;

	//All angles allowed to be -180 to 180 degrees going robin-round
	while (axisAngle > 180)
		axisAngle -= 360;
	while (axisAngle < -180)
		axisAngle += 360;
}
