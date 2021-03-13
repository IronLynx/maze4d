#pragma once

#include <WinField.h>

const glm::ivec4 WinField::winMapSize = glm::ivec4(9, 9, 9, 9);

Field* WinField::CreateWinRoom(Field* const field)
{
	Field* winField = field;
	//field = winField;

	winField->NewField(winMapSize, 2);

	// Generate walls
	for (int x = 0; x < winMapSize.x; x++)
	{
		for (int y = 0; y < winMapSize.y; y++)
		{
			for (int z = 0; z < winMapSize.z; z++)
			{
				for (int w = 0; w < winMapSize.w; w++)
				{
					int index = winField->GetIndex(x, y, z, w);

					if (!(x > 0 && x < 8 &&
						y > 0 && y < 8 &&
						z > 0 && z < 8 &&
						w > 0 && w < 8))
					{
						//winMap[index] = WALL_BLOCK;
						//curMap[index].cellType = 1;
						winField->CreateCube(x, y, z, w);
					}
					else
					{
						winField->CreateCube(x, y, z, w, Cell());
						//winMap[index] = 0;
					}

					//winField->curLightMap[index] = 0;
				}
			}
		}
	}

	static const glm::ivec4 letters[] = {
		// W
		glm::ivec4(7,6,2,0), glm::ivec4(7,5,2,0), glm::ivec4(7,4,2,0), glm::ivec4(7,3,2,0),
		glm::ivec4(7,2,3,0), glm::ivec4(7,4,4,0), glm::ivec4(7,3,4,0), glm::ivec4(7,2,5,0),
		glm::ivec4(7,6,6,0), glm::ivec4(7,5,6,0), glm::ivec4(7,4,6,0), glm::ivec4(7,3,6,0),
		// I
		glm::ivec4(5,2,7,0), glm::ivec4(4,2,7,0), glm::ivec4(3,2,7,0), glm::ivec4(4,3,7,0),
		glm::ivec4(4,4,7,0), glm::ivec4(4,5,7,0), glm::ivec4(5,6,7,0), glm::ivec4(4,6,7,0),
		glm::ivec4(3,6,7,0),
		// N
		glm::ivec4(6,1,6,0), glm::ivec4(6,1,5,0), glm::ivec4(6,1,4,0), glm::ivec4(6,1,3,0),
		glm::ivec4(6,1,2,0), glm::ivec4(5,1,5,0), glm::ivec4(4,1,4,0), glm::ivec4(3,1,3,0),
		glm::ivec4(2,1,6,0), glm::ivec4(2,1,5,0), glm::ivec4(2,1,4,0), glm::ivec4(2,1,3,0),
		glm::ivec4(2,1,2,0)
	};
	for (int w = 1; w < winMapSize.w - 1; w++)
	{
		for (int i = 0; i < sizeof(letters) / sizeof(letters[0]); i++)
		{
			winField->CreateCube(letters[i].x, letters[i].y, letters[i].z, w, Cell(LIGHT_BLOCK, 0, false));
		}
	}

	winField->LoadFieldToGL();
	return winField;
}