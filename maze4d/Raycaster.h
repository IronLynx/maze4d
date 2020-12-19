#pragma once

#include <Utils.h>
#include <Field.h>
#include <Cube.h>

class Raycaster
{
public:
	void Init(Field* field)
	{
		this->field = field;
	}

	// uses DDA algo (from https://lodev.org/cgtutor/raycasting.html)
	void FindPixel(glm::vec4 pos, glm::vec4 v, glm::u8vec3& pixel, float& dist)
	{
		//which box of the map we're in
		glm::ivec4 map(pos);

		//length of ray from current position to next x or y-side
		glm::vec4 sideDist;

		//length of ray from one x or y-side to next x or y-side
		glm::vec4 deltaDist(glm::abs(1.0f / v.x), glm::abs(1.0f / v.y), glm::abs(1.0f / v.z), glm::abs(1.0f / v.w));

		//what direction to step in x or y-direction (either +1 or -1)
		glm::i8vec4 step;

		int hit = 0; //was there a wall hit?
		int side;

		//calculate step and initial sideDist
		if (v.x < 0)
		{
			step.x = -1;
			sideDist.x = (pos.x - map.x) * deltaDist.x;
		}
		else
		{
			step.x = 1;
			sideDist.x = (map.x + 1.0f - pos.x) * deltaDist.x;
		}
		if (v.y < 0)
		{
			step.y = -1;
			sideDist.y = (pos.y - map.y) * deltaDist.y;
		}
		else
		{
			step.y = 1;
			sideDist.y = (map.y + 1.0f - pos.y) * deltaDist.y;
		}
		if (v.z < 0)
		{
			step.z = -1;
			sideDist.z = (pos.z - map.z) * deltaDist.z;
		}
		else
		{
			step.z = 1;
			sideDist.z = (map.z + 1.0f - pos.z) * deltaDist.z;
		}
		if (v.w < 0)
		{
			step.w = -1;
			sideDist.w = (pos.w - map.w) * deltaDist.w;
		}
		else
		{
			step.w = 1;
			sideDist.w = (map.w + 1.0f - pos.w) * deltaDist.w;
		}

		Cell_t cell = 0;
		Light_t light = 0;
		int index = 0;

		//perform DDA
		while (true)
		{
			//jump to next map square, OR in x-direction, OR in y-direction
			if (sideDist.x <= sideDist.y && sideDist.x <= sideDist.z && sideDist.x <= sideDist.w)
			{
				sideDist.x += deltaDist.x;
				map.x += step.x;
				side = 0;
			}
			else if (sideDist.y <= sideDist.x && sideDist.y <= sideDist.z && sideDist.y <= sideDist.w)
			{
				sideDist.y += deltaDist.y;
				map.y += step.y;
				side = 1;
			}
			else if (sideDist.z <= sideDist.x && sideDist.z <= sideDist.y && sideDist.z <= sideDist.w)
			{
				sideDist.z += deltaDist.z;
				map.z += step.z;
				side = 2;
			}
			else if (sideDist.w < sideDist.x && sideDist.w <= sideDist.y && sideDist.w <= sideDist.z)
			{
				sideDist.w += deltaDist.w;
				map.w += step.w;
				side = 3;
			}

			if (!field->IsCubeIndexValid(map.x, map.y, map.z, map.w))
				return;

			//Check if ray has hit a wall
			index = field->GetIndex(map.x, map.y, map.z, map.w);
			cell = field->curMap[index];
			if ((cell & WALL_BLOCK) != 0)
				break;
		}

		//Calculate distance projected on camera direction (Euclidean distance will give fisheye effect!)
		if (side == 0)
		{
			dist = (map.x - pos.x + (1 - step.x) / 2.0f) / v.x;
			glm::vec4 texPoint = pos + v*dist;
			Cube::GetPixel(pos.x < map.x ? NEG_X : POS_X, pixel, glm::vec3(texPoint.y, texPoint.z, texPoint.w),
				map.x, map.y, map.z, map.w, cell, field->curLightMap[index]);
		}
		else if (side == 1)
		{
			dist = (map.y - pos.y + (1 - step.y) / 2.0f) / v.y;
			glm::vec4 texPoint = pos + v*dist;
			Cube::GetPixel(pos.y < map.y ? NEG_Y : POS_Y, pixel, glm::vec3(texPoint.x, texPoint.z, texPoint.w),
				map.x, map.y, map.z, map.w, cell, field->curLightMap[index]);
		}
		else if (side == 2)
		{
			dist = (map.z - pos.z + (1 - step.z) / 2.0f) / v.z;
			glm::vec4 texPoint = pos + v*dist;
			Cube::GetPixel(pos.z < map.z ? NEG_Z : POS_Z, pixel, glm::vec3(texPoint.x, texPoint.y, texPoint.w),
				map.x, map.y, map.z, map.w, cell, field->curLightMap[index]);
		}
		else if (side == 3)
		{
			dist = (map.w - pos.w + (1 - step.w) / 2.0f) / v.w;
			glm::vec4 texPoint = pos + v*dist;
			Cube::GetPixel(pos.w < map.w ? NEG_W : POS_W, pixel, glm::vec3(texPoint.x, texPoint.y, texPoint.z),
				map.x, map.y, map.z, map.w, cell, field->curLightMap[index]);
		}
	}

	static int FindCollision(glm::vec4 pos, glm::vec4 v, float targetDist, float& safeDist, Cell_t& collideCell, bool noclip, Field* field)
	{
		glm::vec4 tmp;
		glm::ivec4 map(tmp);
		int index;
		Cell_t cell;
		float step = 0.01f;
		for (float i = 0.0f; ; i += step)
		{

			tmp = pos + v*i;
			map = tmp;

			if (!field->IsCubeIndexValid(map.x, map.y, map.z, map.w))
			{
				glm::vec4 tmp2 = glm::vec4(pos + v*(i - step));
				safeDist = i - step;
				return 2;
			}

			if (!noclip)
			{
				// Check if ray has hit a wall
				index = field->GetIndex(map.x, map.y, map.z, map.w);
				cell = field->curMap[index];
				collideCell = cell;
				if ((cell & WALL_BLOCK) != 0 && (cell & LIGHT_BLOCK) == 0 && (cell & WIN_BLOCK) == 0)
				{
					safeDist = i - step;
					return 1;
				}
			}

			if (i > targetDist)
			{
				safeDist = targetDist;
				return 0;
			}
		}
	}

private:
	Field* field = nullptr;
};
