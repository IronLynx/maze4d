#include <Player.h>

#include <fstream>
#include <sstream>
#include <iomanip> 
#include <math.h>
#include <set>
#include <Utils.h>

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

	//player orientation
	angleXY = 0.0f; //-90f - 90f acceptable values, this is up-down rotation angle
	angleXZ = 0.0f; //free for all rotation left-right
	angleYZ = 0.0f; //roll
	angleXW = 0.0f;
	angleZW = 0.0f;
	angleYW = 0.0f;

	pos = glm::vec4(field->roomSize / 2.0f + 0.2f);
	lastPos = pos;

	SetCurrentRotation();
}

void Player::ResetBasis()
{
	vx_basis = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	vy_basis = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	vz_basis = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
	vw_basis = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	ResetToBasis();

	angleYZ = 0; //cancel Roll

	angleXW = 0.0f;
	angleZW = 0.0f;
	angleYW = 0.0f;

	SetCurrentRotation();
}

void Player::Rotate(float a, glm::vec4& va, glm::vec4& vb)
{
	float c = cosd(a);
	float s = sind(a);
	glm::vec4 va1 = va * c;
	glm::vec4 vb1 = vb * s;

	float c2 = cosd(a + 90.0f);
	float s2 = sind(a + 90.0f);
	glm::vec4 va2 = va * c2;
	glm::vec4 vb2 = vb * s2;

	va = glm::normalize(va1 + vb1);
	vb = glm::normalize(va2 + vb2);

	float errorAngle = 90.0f - angleBetweenVecs(va, vb);
	if (abs(errorAngle) > 0.001)
	{
		float c3 = cosd(errorAngle + 90.0f);
		float s3 = sind(errorAngle + 90.0f);
		glm::vec4 va3 = va * c3;
		glm::vec4 vb3 = vb * s3;
		vb = glm::normalize(va3 + vb3);
	}
}

void Player::ResetToBasis()
{
	vx = vx_basis;
	vy = vy_basis;
	vz = vz_basis;
	vw = vw_basis;
}

void Player::RebaseToCurrent()
{
	vx_basis = vx;
	vy_basis = vy;
	vz_basis = vz;
	vw_basis = vw;
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
		Rotate(angleXZ, vx, vz); //MouseX, rotate left and right			
		Rotate(angleXY, vx, vy); //MouseY, up and down rotation
	}
}

void Player::AlignRotation()
{
	if (!groundRotation)
		RebaseToCurrent();

	RoundBasisAngles();
	ResetToBasis();

	//Just for interface visualization
	//No logic bound to these W-angles
	angleXW = 90.0f * vw_basis.x;
	angleYW = 90.0f * vw_basis.y;
	angleZW = 90.0f * vw_basis.z;

	angleYZ = round(angleYZ / 90.0f) * 90.0f;

	SetCurrentRotation();
}

void Player::RotateAngle(float& axisAngle, glm::vec4& va, glm::vec4& vb, float degree)
{
	Rotate(degree, va, vb);
	AddToAngleDegree(axisAngle, degree);
}

#define RAY_NO_COLLISION 0
#define RAY_COLLIDE_BLOCK 1
#define RAY_COLLIDE_MAP_BORDER 2

void Player::SetNewPos(glm::vec4 v, float delta, int sign)
{
	float safeDist;
	Cell_t collideCell = 0;
	int res = Raycaster::FindCollision(lastPos, v*(float)sign, delta, safeDist, collideCell, noclip, field);

	//Enable out-of box walking
	//if (!noclip || res == RAY_COLLIDE_MAP_BORDER)
	if (!noclip)
	{
		safeDist = glm::min(safeDist, delta);
		pos += v*safeDist*(float)sign;
	}
	else 
	{
		pos += v*delta*(float)sign;
	}

	//glorious victory
	if (res == RAY_NO_COLLISION && (collideCell & WIN_BLOCK) != 0)
	{
		field->CreateWinRoom();
		Reset();
		pos = glm::vec4(1.01f, 6.99f, 1.01f, 3.5f);
		lastPos = pos;
		RotateXZ(45.0f);
		RotateXY(-30.0f);
		noclip = false;
	}

	lastPos = pos;
}

void Player::MoveX(float d, int sign) { SetNewPos(vx, d, sign); }
void Player::MoveY(float d, int sign) { SetNewPos(vy, d, sign); }
void Player::MoveZ(float d, int sign) { SetNewPos(vz, d, sign); }
void Player::MoveW(float d, int sign) { SetNewPos(vw, d, sign); }

#define IF_GRND if (groundRotation)

//3d-roll
void Player::RotateYZ(float a)
{
	RotateAngle(angleYZ, vy, vz, a);

	IF_GRND Rotate(-angleXY, vx, vy); //MouseY, up and down rotation
	IF_GRND Rotate(-angleXZ, vx, vz); //MouseX, rotate left and right

	IF_GRND RebaseToCurrent();
	SetCurrentRotation();
}

//MouseY
void Player::RotateXY(float a)
{
	RotateAngle(angleXY, vx, vy, a);
	SetCurrentRotation();
}

//MouseX
void Player::RotateXZ(float a)
{
	RotateAngle(angleXZ, vx, vz, a);
	SetCurrentRotation();
}

//4d-roll
void Player::RotateYW(float a)
{
	IF_GRND ResetToBasis();

	RotateAngle(angleYW, vy, vw, a);

	IF_GRND RebaseToCurrent();
	SetCurrentRotation();
}

//Shift+MouseX
void Player::RotateZW(float a)
{
	IF_GRND Rotate(-angleXY, vx, vy); //MouseY, up and down rotation

	RotateAngle(angleZW, vz, vw, a);

	IF_GRND Rotate(-angleXZ, vx, vz); //MouseX, rotate left and right
	IF_GRND RebaseToCurrent();
	SetCurrentRotation();
}

//Shift+MouseY
void Player::RotateXW(float a)
{
	IF_GRND Rotate(-angleXY, vx, vy); //MouseY, up and down rotation

	RotateAngle(angleXW, vx, vw, a);

	IF_GRND Rotate(-angleXZ, vx, vz); //MouseX, rotate left and right
	IF_GRND RebaseToCurrent();
	SetCurrentRotation();
}

//Format Float to string, FF
std::string Player::FF(float value, unsigned int decimals)
{
	std::stringstream stream;
	stream << std::fixed << std::setprecision(decimals) << value;

	return stream.str();
}

void Player::Print()
{
	Log("vx = glm::vec4(", vx.x, "f, ", vx.y, "f, ", vx.z, "f, ", vx.w, "f);");
	Log("vy = glm::vec4(", vy.x, "f, ", vy.y, "f, ", vy.z, "f, ", vy.w, "f);");
	Log("vz = glm::vec4(", vz.x, "f, ", vz.y, "f, ", vz.z, "f, ", vz.w, "f);");
	Log("vw = glm::vec4(", vw.x, "f, ", vw.y, "f, ", vw.z, "f, ", vw.w, "f);");
	Log("pos = glm::vec4(", pos.x, "f, ", pos.y, "f, ", pos.z, "f, ", pos.w, "f);");

	/*
	Log("angles: XZ(mx) =", FF(angleXZ), ", XY(my) =", FF(angleXY), ");");
	Log("angles: ZW(smX)=", FF(angleZW), ", XW(smY)=", FF(angleXW), ");");
	Log("angles: YZ(rol)=", FF(angleYZ), ", YW(rlW)=", FF(angleYW), ");");
	Log("vx = glm::vec4(", FF(vx_basis.x), ", ", FF(vx_basis.y), ", ", FF(vx_basis.z), ", ", FF(vx_basis.w), ");");
	Log("vy = glm::vec4(", FF(vy_basis.x), ", ", FF(vy_basis.y), ", ", FF(vy_basis.z), ", ", FF(vy_basis.w), ");");
	Log("vz = glm::vec4(", FF(vz_basis.x), ", ", FF(vz_basis.y), ", ", FF(vz_basis.z), ", ", FF(vz_basis.w), ");");
	Log("vw = glm::vec4(", FF(vw_basis.x), ", ", FF(vw_basis.y), ", ", FF(vw_basis.z), ", ", FF(vw_basis.w), ");");
	Log("pos = glm::vec4(", pos.x, ", ", pos.y, "f, ", pos.z, "f, ", pos.w, "f);");
	*/
};

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

glm::vec4& Player::BasisVecByNum(int i)
{
	if (i == 0)
		return vx_basis;
	if (i == 1)
		return vy_basis;
	if (i == 2)
		return vz_basis;
	if (i == 3)
		return vw_basis;
}

float& Player::BasisCoordByPoint(int vecNum, int coordNum)
{
	if (coordNum == 0)
		return BasisVecByNum(vecNum).x;
	if (coordNum == 1)
		return BasisVecByNum(vecNum).y;
	if (coordNum == 2)
		return BasisVecByNum(vecNum).z;
	if (coordNum == 3)
		return BasisVecByNum(vecNum).w;
}

int Player::MaxCoordInt(glm::vec4& vec)
{
	if (abs(vec.x) >= std::max(abs(vec.y), std::max(abs(vec.z), abs(vec.w))))
		return 0;

	if (abs(vec.y) >= std::max(abs(vec.x), std::max(abs(vec.z), abs(vec.w))))
		return 1;

	if (abs(vec.z) >= std::max(abs(vec.y), std::max(abs(vec.x), abs(vec.w))))
		return 2;

	if (abs(vec.w) >= std::max(abs(vec.y), std::max(abs(vec.z), abs(vec.x))))
		return 3;
}

float Player::RotateToZero(float &coordinate, glm::vec4 &veca, glm::vec4 &vecb, unsigned int maxSteps)
{
	unsigned int i = 0;
	float angleRotated = 0;

	if (abs(coordinate) < 0.0005f)
		return angleRotated;

	float step = 0.01f;

	float prevValue = coordinate;
	Rotate(step, veca, vecb);
	angleRotated += step;
	i++;

	if (abs(coordinate) > abs(prevValue))
	{
		step = -step;
		Rotate(step, veca, vecb);
		angleRotated += step;
		i--;
	}

	while (abs(coordinate) > 0.0005f && i < maxSteps)
	{
		float prevValue = coordinate;
		Rotate(step, veca, vecb);

		angleRotated += step;
		i++;
	}

	return angleRotated;
}

void Player::RoundBasisAngles()
{
	std::set<int> coordsDone = std::set<int>(); //coords that has been rotated to 1 in any vector
	std::set<int> vectorsDone = std::set<int>(); //vectors that has 3 coords rotated to zero
	int curVector = 3; //starting from vw_basis

	while (vectorsDone.size() < 4)
	{
		//find coordinate with maximum value to rotate it to 1/-1 
		float maxCoord = 0;
		int OrtoCoordNum = -1;
		for (int i = 0; i < 4; i++)
			if (coordsDone.find(i) == coordsDone.end())
				if (abs(BasisCoordByPoint(curVector, i)) > maxCoord)
				{
					maxCoord = abs(BasisCoordByPoint(curVector, i));
					OrtoCoordNum = i; //future winner
				}

		float &coordOrto = BasisCoordByPoint(curVector, OrtoCoordNum);
		coordsDone.insert(OrtoCoordNum); //since now skip this coordinate for rotation purposes

		bool firstTime = true;
		int vecForRotation = -1;

		int coordForRotation = -1;

		//define static vector for rotation for all 3 zero-rotated coords
		int vecForRotation2 = curVector;

		//in case we need to rotate coordinate to 1 (e.g. w)
		//But corresponding vector is already ortogonated (e.g. vw_basis)
		//we replace it by ortogonated coordinate from it (e.g. vw_basis.z)
		if (vectorsDone.find(OrtoCoordNum) != vectorsDone.end())
		{
			vecForRotation2 = OrtoCoordNum;
			while (vectorsDone.find(vecForRotation2) != vectorsDone.end()) {
				int replaceVector = *vectorsDone.find(vecForRotation2);
				vecForRotation2 = MaxCoordInt(BasisVecByNum(replaceVector));
			}
		}

		//rotate until we have OrtoVector equal to 1
		//We check that all others are zero
		//Done for presicion purpose
		float maxNonOrtoCoord = 0;
		while (abs(maxNonOrtoCoord) > 0.001 || firstTime)
		{
			//Plan:
			//1. Find coord for rotation
			//2. Find corresponding vector for rotation
			//3. Rotate
			//4. Repeat
			if (!firstTime)
				RotateToZero(BasisCoordByPoint(curVector, coordForRotation), BasisVecByNum(vecForRotation), BasisVecByNum(vecForRotation2), 1);

			firstTime = false;

			//find biggest coord that needs to be rotated to zero
			maxNonOrtoCoord = 0;
			for (int i = 0; i < 4; i++)
				if (coordsDone.find(i) == coordsDone.end())
					if (abs(BasisCoordByPoint(curVector, i)) > abs(maxNonOrtoCoord))
					{
						maxNonOrtoCoord = abs(BasisCoordByPoint(curVector, i));
						coordForRotation = i;
					}

			//find corresponding vector which needs to be rotated to get coordinate to zero best way
			vecForRotation = coordForRotation;
			if (vecForRotation2 == vecForRotation)
				vecForRotation = OrtoCoordNum;
			while (vectorsDone.find(vecForRotation) != vectorsDone.end())
			{
				int replaceVector = *vectorsDone.find(vecForRotation);
				vecForRotation = MaxCoordInt(BasisVecByNum(replaceVector));
			}
			if (vecForRotation2 == vecForRotation)
				vecForRotation = OrtoCoordNum;
		}
		//end while rotation

		//move to the next vector for ortogonation
		vectorsDone.insert(curVector);
		if (vectorsDone.size() > 3)
			break;
		else if (vectorsDone.find(OrtoCoordNum) == vectorsDone.end())
			curVector = OrtoCoordNum; //chain next vector based on previous
		else
		{//if chain loops take any non processed vector
			curVector = 3;
			while (vectorsDone.find(curVector) != vectorsDone.end())
				curVector--;
		}
	}
}