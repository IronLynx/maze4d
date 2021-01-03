#pragma once

#include <Utils.h>
#include <Field.h>
#include <Raycaster.h>
#include <fstream>

class Player
{
public:
	Player() {}

	void Init(Field* field)
	{
		this->field = field;
		Reset();
	}

	void Reset()
	{
		ResetAngle();
		pos = glm::vec4(field->roomSize / 2.0f + 0.2f);
		lastPos = pos;
	}

	void ResetAngle()
	{
		//player orientation
		angleXY = 0.0f; //-90f - 90f acceptable values, this is up-down rotation angle
		angleXZ = 0.0f; //free for all rotation left-right
		angleYZ = 0.0f; //roll
		angleXW = 0.0f; 
		angleZW = 0.0f; 
		angleYW = 0.0f; 

		SetCurrentRotation();
	}

	void Rotate(float a, glm::vec4& va, glm::vec4& vb)
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

	void SetCurrentRotation()
	{
		vx = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
		vy = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
		vz = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
		vw = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		
		//Set default-constant pitch angle
		//Doesn't affect MouseY input
		Rotate(90.0f, vx, vy); 
		
		//Prioritized W-rotation to define 3D-slice
		Rotate(angleYW, vy, vw); //Roll 4th dimenison journey
		Rotate(angleZW, vz, vw); //Shift + MouseX, 4th dimenison journey
		Rotate(angleXW, vx, vw); //Shift + MouseY, 4th dimenison journey

		//Prioritized rotation inside current 3d-slice
		Rotate(angleYZ, vy, vz); //roll
		Rotate(angleXZ, vx, vz); //MouseX, rotate left and right
		Rotate(angleXY, vx, vy); //MouseY, up and down rotation
	}

	//Align 3d-slice to specific degree multiplier (called rounder)
	void RoundAngleW(int rounder = 90)
	{
		angleXW = round(angleXW / rounder) * (float)rounder;
		angleZW = round(angleZW / rounder) * (float)rounder;
		angleYW = round(angleYW / rounder) * (float)rounder;

		SetCurrentRotation();
	}

	void RotateAngle(float& axisAngle, glm::vec4& va, glm::vec4& vb, float degree)
	{	
		axisAngle += degree;

		//All angles allowed to be -180 to 180 degrees going robin-round
		while (axisAngle > 180)
			axisAngle -= 360;
		while (axisAngle < -180)
			axisAngle += 360;

		//pitch angle restiction to avoid MouseY backflip rotation
		angleXY = std::max(angleXY, -90.0f);
		angleXY = std::min(angleXY, +90.0f);

		SetCurrentRotation();
	}

	void SetNewPos(glm::vec4 v, float delta, int sign)
	{
		float safeDist;
		Cell_t collideCell = 0;
		int res = Raycaster::FindCollision(lastPos, v*(float)sign, delta, safeDist, collideCell, noclip, field);

		if (!noclip || res == 2)
		{
			safeDist = glm::min(safeDist, delta);
			pos += v*safeDist*(float)sign;
		}
		else
		{
			pos += v*delta*(float)sign;
		}

		if (res == 0 && (collideCell & WIN_BLOCK) != 0)
		{
			field->CreateWinRoom();
			Reset();
			pos = glm::vec4(1.01f, 6.99f, 1.01f, 3.5f);
			lastPos = pos;
			noclip = false;
		}

		lastPos = pos;
	}

	void MoveX(float d, int sign) { SetNewPos(vx, d, sign); }
	void MoveY(float d, int sign) { SetNewPos(vy, d, sign); }
	void MoveZ(float d, int sign) { SetNewPos(vz, d, sign); }
	void MoveW(float d, int sign) { SetNewPos(vw, d, sign); }

	void RotateXY(float a) { RotateAngle(angleXY, vx, vy, a); }
	void RotateXZ(float a) { RotateAngle(angleXZ, vx, vz, a); }
	void RotateXW(float a) { RotateAngle(angleXW, vx, vw, a); }
	void RotateYZ(float a) { RotateAngle(angleYZ, vy, vz, a); }
	void RotateYW(float a) { RotateAngle(angleYW, vy, vw, a); }
	void RotateZW(float a) { RotateAngle(angleZW, vz, vw, a); }

	void Print()
	{
		Log("vx = glm::vec4(", vx.x, "f, ", vx.y, "f, ", vx.z, "f, ", vx.w, "f);");
		Log("vy = glm::vec4(", vy.x, "f, ", vy.y, "f, ", vy.z, "f, ", vy.w, "f);");
		Log("vz = glm::vec4(", vz.x, "f, ", vz.y, "f, ", vz.z, "f, ", vz.w, "f);");
		Log("vw = glm::vec4(", vw.x, "f, ", vw.y, "f, ", vw.z, "f, ", vw.w, "f);");
		Log("pos = glm::vec4(", pos.x, "f, ", pos.y, "f, ", pos.z, "f, ", pos.w, "f);");
	};

	glm::vec4 vx; // always points at center of screen
	glm::vec4 vy;
	glm::vec4 vz;
	glm::vec4 vw;
	glm::vec4 pos;
	glm::vec4 lastPos;

	//inside 3d-slice rotation angles
	float angleXY; 
	float angleXZ; 
	float angleYZ;

	//3d slice definition rotation angles
	float angleXW;
	float angleYW;
	float angleZW; 

	bool noclip = false;

	Field* field = nullptr;
};