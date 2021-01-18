#pragma once

#include <Field.h>
#include <Raycaster.h>

class Player
{
public:
	Player() {}

	void Init(Field* field, bool groundRotation = false);

	void Reset(); //start from the very begining
	void ResetBasis();
	void ResetToBasis();
	void RebaseToCurrent();
	
	void SetCurrentRotation(); //works only for groundRotation
	void AlignRotation();

	static void Rotate(float a, glm::vec4& va, glm::vec4& vb);

	void SetNewPos(glm::vec4 v, float delta, int sign);

	void MoveX(float d, int sign);
	void MoveY(float d, int sign);
	void MoveZ(float d, int sign);
	void MoveW(float d, int sign);
	
	void RotateYZ(float a); //3d-roll	
	void RotateXY(float a); //MouseY	
	void RotateXZ(float a); //MouseX
	
	void RotateYW(float a); //4d-roll	
	void RotateZW(float a); //Shift+MouseX	
	void RotateXW(float a); //Shift+MouseY

	//Format Float to string, FF
	std::string FF(float value, unsigned int decimals = 2);
	void Print();

	glm::vec4 vx; // always points at center of screen
	glm::vec4 vy;
	glm::vec4 vz;
	glm::vec4 vw;
	
	glm::vec4 vx_basis; 
	glm::vec4 vy_basis;
	glm::vec4 vz_basis;
	glm::vec4 vw_basis;

	glm::vec4 pos;
	glm::vec4 lastPos;

	//inside 3d-slice rotation angles	
	float angleXY; //MouseY
	float angleXZ; //MouseX	
	float angleYZ; //3d roll

	//3d slice definition rotation angles
	float angleXW; //Shift+MouseY
	float angleYW; //4d roll
	float angleZW; //Shift+MouseX

	bool noclip = false;
	bool groundRotation = false;

	Field* field = nullptr;

private:

	void RotateAngle(float& axisAngle, glm::vec4& va, glm::vec4& vb, float degree);
	void AddToAngleDegree(float& axisAngle, float degree); //Does not make any matrix rotations

	glm::vec4& BasisVecByNum(int i);
	float& BasisCoordByPoint(int vecNum, int coordNum);
	int MaxCoordInt(glm::vec4& vec);

	float RotateToZero(float &coordinate, glm::vec4 &veca, glm::vec4 &vecb, unsigned int maxSteps = 1);
	void RoundBasisAngles();
};