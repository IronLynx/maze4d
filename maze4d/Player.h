#pragma once

#include <Utils.h>
#include <Field.h>
#include <Raycaster.h>

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
		vx = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
		vy = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
		vz = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
		vw = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		pos = glm::vec4(field->roomSize / 2.0f + 0.2f);
		lastPos = pos;
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

	void RotateXY(float a) { Rotate(a, vx, vy); }
	void RotateXZ(float a) { Rotate(a, vx, vz); }
	void RotateXW(float a) { Rotate(a, vx, vw); }
	void RotateYZ(float a) { Rotate(a, vy, vz); }
	void RotateYW(float a) { Rotate(a, vy, vw); }
	void RotateZW(float a) { Rotate(a, vz, vw); }

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
	bool noclip = false;

	Field* field = nullptr;
};