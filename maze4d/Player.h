#pragma once


#include <Field.h>
#include <RotationMatrix.h>
#include <MazeField.h>
#include <WinField.h>
#include <Utils.h>

#include <fstream>
#include <sstream>
#include <iomanip> 
#include <math.h>
#include <set>



class Field;

class Player
{
public:
	Player() {}

	RotationMatrix basisMatrix;
	RotationMatrix rotationMatrix;

	void Init(Field* field, bool groundRotation = false);

	void Reset(); //start from the very begining
	void ResetBasis();
	void ResetToBasis();
	void RebaseToCurrent();
	
	void SetCurrentRotation(); //works only for groundRotation
	void AlignRotation();

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

	glm::vec4 pos;
	glm::vec4 defaultPos = glm::vec4(1.2f);

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

	#define RAY_NO_COLLISION 0
	#define RAY_COLLIDE_BLOCK 1
	#define RAY_COLLIDE_MAP_BORDER 2
	#define RAY_COLLIDE_WIN_BLOCK 3

	void MoveAndHandleEvent(glm::vec4 v, float delta, int sign);
	int FindCollision(glm::vec4 v, float targetDist, float& safeDist);
	void AddToAngleDegree(float& axisAngle, float degree); //Does not make any matrix rotations

};