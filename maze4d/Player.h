#pragma once

#include <Utils.h>
#include <Field.h>
#include <Raycaster.h>
#include <fstream>
#include <math.h>

class Player
{
public:
	Player() {}

	void Init(Field* field, bool groundRotation = false)
	{
		this->field = field;
		Reset();
		this->groundRotation = groundRotation;

		vx = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
		vy = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
		vz = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
		vw = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		vx_basis = vx;
		vy_basis = vy;
		vz_basis = vz;
		vw_basis = vw;
	}

	void Reset()
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
	}

	void ResetBasis()
	{
		vx_basis = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
		vy_basis = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
		vz_basis = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
		vw_basis = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		vx = vx_basis;
		vy = vy_basis;
		vz = vz_basis;
		vw = vw_basis;

		angleYZ = 0; //cancel Roll

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

	void RotateBasisW(float& axisAngle, glm::vec4& va, float degree)
	{
		if (!groundRotation)
			Rotate(degree, va, vw);
		else
		{
			axisAngle += degree;
			vx = vx_basis;
			vy = vy_basis;
			vz = vz_basis;
			vw = vw_basis;

			Rotate(degree, va, vw);

			vx_basis = vx;
			vy_basis = vy;
			vz_basis = vz;
			vw_basis = vw;

			SetCurrentRotation();
		}
	}

	void SetCurrentRotation()
	{
		if (groundRotation)
		{
			vx = vx_basis;
			vy = vy_basis;
			vz = vz_basis;
			vw = vw_basis;

			//Prioritized rotation inside current 3d-slice
			Rotate(angleYZ, vy, vz); //roll
			Rotate(angleXZ, vx, vz); //MouseX, rotate left and right		
			Rotate(angleXY, vx, vy); //MouseY, up and down rotation
		}
	}

	float RotateToZero(float &coordinate, glm::vec4 &veca, glm::vec4 &vecb, unsigned int maxSteps = 1)
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

	//Align 3d-slice to specific degree multiplier (called rounder)
	void RoundAngleW()
	{
		//Idea is to make sequence rotations to align vw_basis vector to 90 degrees
		//Ignore y axis because it should not be part of basis, because it is not bound to shift+Mouse
		//Sequence:	
		//Step 1. get by_basis vector equal to (0,1,0,0)
		//Step 2. get vw_basis vector ortogonal, 3 options: vw_x=1, vw_z=1, vw_w=1 
		//Step 3. get vx_basis vector ortogonal, 3 options: vx_x=1, vx_z=1, vx_w=1
		//Total of 6 combinations excluding same coordinate, e.g vw_x/vx_x 

		//w coordinate is major one inside vector, it's 0..1 value depends only on angle(w)
		//x,z secondary coordinates, their 0..1 values depends on sin/cos of angle(w)

		//TO DO: refactoring to create true rotation matrix so can iterate through same coordinate (e.g x) across different vectors

		if (!groundRotation)
		{
			//Make this work for not groundRotation
			vx_basis = vx;
			vy_basis = vy;
			vz_basis = vz;
			vw_basis = vw;
		}

		if (groundRotation)
		{//Step 1. get vy_basis=(0,1,0,0)
			//do not use vector.coord > 0.999 because of required precision vector.coord > 0.9999999 to get other coords < 0.001
			while (abs(vy_basis.x) > 0.001 || abs(vy_basis.z) > 0.001 || abs(vy_basis.w) > 0.001)
			{
				if (abs(vy_basis.w) > std::max(abs(vy_basis.x), abs(vy_basis.z)))
					RotateToZero(vy_basis.w, vy_basis, vw_basis, 1);
				else if (abs(vy_basis.x) > abs(vy_basis.z))
					RotateToZero(vy_basis.x, vy_basis, vx_basis, 1);
				else
					RotateToZero(vy_basis.z, vy_basis, vz_basis, 1);
			}

			if (vy_basis.y < 0)
				Rotate(180, vx_basis, vy_basis);
		}


		if (vw_basis.w < -0.5 || vw_basis.w > 0.5)
		{//primary vw_w aligment to 1/-1
			while (abs(vw_basis.x) > 0.001 || abs(vw_basis.z) > 0.001 || abs(vw_basis.y) > 0.001)
			{
				if (abs(vw_basis.y) > std::max(abs(vw_basis.x), abs(vw_basis.z)))
					RotateToZero(vw_basis.y, vy_basis, vw_basis, 1);
				else if (abs(vw_basis.x) > abs(vw_basis.z))
					RotateToZero(vw_basis.x, vx_basis, vw_basis, 1);
				else
					RotateToZero(vw_basis.z, vz_basis, vw_basis, 1);
			}

			if (groundRotation)
				if (abs(vx_basis.x) > abs(vx_basis.z))
					RotateToZero(vx_basis.z, vx_basis, vz_basis, -1);
				else
					RotateToZero(vx_basis.x, vx_basis, vz_basis, -1);
		}
		else if (abs(vw_basis.y) > std::max(abs(vw_basis.x), abs(vw_basis.z)))
		{//secondary vw_y aligment to 1/-1
			while ((abs(vw_basis.w) > 0.001 || abs(vw_basis.z) > 0.001) || abs(vw_basis.x) > 0.001)
			{
				if (abs(vw_basis.x) > std::max(abs(vw_basis.w), abs(vw_basis.z)))
					RotateToZero(vw_basis.x, vx_basis, vw_basis, 1);
				else if (abs(vw_basis.w) > abs(vw_basis.z))
					RotateToZero(vw_basis.w, vy_basis, vw_basis, 1);
				else
					RotateToZero(vw_basis.z, vz_basis, vw_basis, 1);
			}
			//do not align xz rotation for groundRotation, because we already did it in Step1
		}
		else if (abs(vw_basis.x) > abs(vw_basis.z))
		{//secondary vw_x aligment to 1/-1
			while ((abs(vw_basis.w) > 0.001 || abs(vw_basis.z) > 0.001) || abs(vw_basis.y) > 0.001)
			{
				if (abs(vw_basis.y) > std::max(abs(vw_basis.w), abs(vw_basis.z)))
					RotateToZero(vw_basis.y, vy_basis, vw_basis, 1);
				else if (abs(vw_basis.w) > abs(vw_basis.z))
					RotateToZero(vw_basis.w, vx_basis, vw_basis, 1);
				else
					RotateToZero(vw_basis.z, vz_basis, vw_basis, 1);
			}

			if (groundRotation)
				if (abs(vx_basis.w) > abs(vx_basis.z))
					RotateToZero(vx_basis.z, vx_basis, vz_basis, -1);
				else
					RotateToZero(vx_basis.w, vx_basis, vz_basis, -1);
		}
		else 
		{//vw_z aligment to 1/-1
			while (abs(vw_basis.w) > 0.001 || abs(vw_basis.x) > 0.001 || abs(vw_basis.y) > 0.001)
			{
				if (abs(vw_basis.y) > std::max(abs(vw_basis.w), abs(vw_basis.x)))
					RotateToZero(vw_basis.y, vy_basis, vw_basis, 1);
				else if (abs(vw_basis.w) < abs(vw_basis.x))
					RotateToZero(vw_basis.x, vz_basis, vw_basis, 1);
				else
					RotateToZero(vw_basis.w, vx_basis, vw_basis, 1);
			}

			if (groundRotation)
				if (abs(vx_basis.x) < abs(vx_basis.w))
					RotateToZero(vx_basis.x, vx_basis, vz_basis, -1);
				else
					RotateToZero(vx_basis.w, vx_basis, vz_basis, -1);
		}

		if (!groundRotation)
		{
			vx = vx_basis;
			vy = vy_basis;
			vz = vz_basis;
			vw = vw_basis;
		}


		//Just for interface visualization
		//No logic bound to these W-angles
		angleXW = -90.0f * vw_basis.x;
		angleYW = -90.0f * vw_basis.y;
		angleZW = -90.0f * vw_basis.z;;

		SetCurrentRotation();
	}

	void RotateAngle(float& axisAngle, glm::vec4& va, glm::vec4& vb, float degree)
	{	
		if (!groundRotation)
			Rotate(degree, va, vb);
		else {
			axisAngle += degree;

			//All angles allowed to be -180 to 180 degrees going robin-round
			while (axisAngle > 180)
				axisAngle -= 360;
			while (axisAngle < -180)
				axisAngle += 360;

			//pitch angle restriction
			angleXY = std::max(-90.0f, angleXY);
			angleXY = std::min( 90.0f, angleXY);

			SetCurrentRotation();
		}
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
			RotateXZ(45.0f);
			RotateXY(-30.0f);
			noclip = false;
		}

		lastPos = pos;
	}

	void MoveX(float d, int sign) { SetNewPos(vx, d, sign); }
	void MoveY(float d, int sign) { SetNewPos(vy, d, sign); }
	void MoveZ(float d, int sign) { SetNewPos(vz, d, sign); }
	void MoveW(float d, int sign) { SetNewPos(vw, d, sign); }

	void RotateYZ(float a) { RotateAngle(angleYZ, vy, vz, a); } //3d-roll
	void RotateXY(float a) { RotateAngle(angleXY, vx, vy, a); } //MouseY
	void RotateXZ(float a) { RotateAngle(angleXZ, vx, vz, a); } //MouseX


	void RotateYW(float a) { RotateBasisW(angleYW, vy, a); } //4d-roll
	void RotateZW(float a) { RotateBasisW(angleZW, vz, a); } //Shift+MouseX
	void RotateXW(float a) { RotateBasisW(angleXW, vx, a); } //Shift+MouseY

	//Format Float to string, FF
	std::string FF(float value,unsigned int decimals = 2)
	{
		std::stringstream stream;
		stream << std::fixed << std::setprecision(decimals) << value;

		return stream.str();
	}

	void Print()
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

	glm::vec4 vx; // always points at center of screen
	glm::vec4 vy;
	glm::vec4 vz;
	glm::vec4 vw;


	//for W-rotation only
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
};