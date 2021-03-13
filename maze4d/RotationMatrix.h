#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <math.h>
#include <set>

#include <Utils.h>

class RotationMatrix
{
public:

	glm::vec4 vx = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f); // always points at center of screen
	glm::vec4 vy = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	glm::vec4 vz = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
	glm::vec4 vw = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	void Reset()
	{
		vx = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
		vy = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
		vz = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
		vw = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	glm::vec4& VecByNum(int i)
	{
		if (i == 0)
			return vx;
		if (i == 1)
			return vy;
		if (i == 2)
			return vz;
		if (i == 3)
			return vw;

		return vx;
	}

	float& CoordByPoint(int vecNum, int coordNum)
	{
		if (coordNum == 0)
			return VecByNum(vecNum).x;
		if (coordNum == 1)
			return VecByNum(vecNum).y;
		if (coordNum == 2)
			return VecByNum(vecNum).z;
		if (coordNum == 3)
			return VecByNum(vecNum).w;

		return VecByNum(vecNum).x;
	}

	void RotateYZ(float a) { Rotate(a, vy, vz); } //3d-roll	
	void RotateXY(float a) { Rotate(a, vx, vy); }  //MouseY	
	void RotateXZ(float a) { Rotate(a, vx, vz); } //MouseX

	void RotateYW(float a) { Rotate(a, vy, vw); } //4d-roll	
	void RotateZW(float a) { Rotate(a, vz, vw); } //Shift+MouseX	
	void RotateXW(float a) { Rotate(a, vx, vw); } //Shift+MouseY

	


	void RoundAngles()
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
					if (abs(CoordByPoint(curVector, i)) > maxCoord)
					{
						maxCoord = abs(CoordByPoint(curVector, i));
						OrtoCoordNum = i; //future winner
					}

			float &coordOrto = CoordByPoint(curVector, OrtoCoordNum);
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
					vecForRotation2 = MaxCoordInt(VecByNum(replaceVector));
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
					RotateToZero(CoordByPoint(curVector, coordForRotation), VecByNum(vecForRotation), VecByNum(vecForRotation2), 1);

				firstTime = false;

				//find biggest coord that needs to be rotated to zero
				maxNonOrtoCoord = 0;
				for (int i = 0; i < 4; i++)
					if (coordsDone.find(i) == coordsDone.end())
						if (abs(CoordByPoint(curVector, i)) > abs(maxNonOrtoCoord))
						{
							maxNonOrtoCoord = abs(CoordByPoint(curVector, i));
							coordForRotation = i;
						}

				//find corresponding vector which needs to be rotated to get coordinate to zero best way
				vecForRotation = coordForRotation;
				if (vecForRotation2 == vecForRotation)
					vecForRotation = OrtoCoordNum;
				while (vectorsDone.find(vecForRotation) != vectorsDone.end())
				{
					int replaceVector = *vectorsDone.find(vecForRotation);
					vecForRotation = MaxCoordInt(VecByNum(replaceVector));
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

private:
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

	int MaxCoordInt(glm::vec4& vec)
	{
		if (abs(vec.x) >= std::max(abs(vec.y), std::max(abs(vec.z), abs(vec.w))))
			return 0;

		if (abs(vec.y) >= std::max(abs(vec.x), std::max(abs(vec.z), abs(vec.w))))
			return 1;

		if (abs(vec.z) >= std::max(abs(vec.y), std::max(abs(vec.x), abs(vec.w))))
			return 2;

		if (abs(vec.w) >= std::max(abs(vec.y), std::max(abs(vec.z), abs(vec.x))))
			return 3;

		return -1;
	}

	float RotateToZero(float &coordinate, glm::vec4 &veca, glm::vec4 &vecb, unsigned int maxSteps)
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
};
