#pragma once

#include <Utils.h>
#include <Player.h>
#include <Raycaster.h>

class Renderer
{
public:
	Renderer(Player* player, Field* field, Raycaster* raycaster, bool useMP, bool skipPixels)
		: player(player), field(field), raycaster(raycaster), useMP(useMP), skipPixels(skipPixels)
	{}

	void FillPixel(glm::vec4& pos, glm::vec4& v, uint8_t* buffer, int index)
	{
		glm::u8vec3 pixel(250, 250, 250);
		float dist;

		raycaster->FindPixel(pos, v, pixel, dist);

		//// distance fog
		//static const int FOG_RANGE = 30;
		//static const int FOG_STRENGH = 120; // 0 - 255
		//int t = std::min(int(dist * FOG_RANGE), FOG_STRENGH);
		//pixel.x = std::min(t + pixel.x, 255);
		//pixel.y = std::min(t + pixel.y, 255);
		//pixel.z = std::min(t + pixel.z, 255);

		buffer[index * 4    ] = pixel.x;
		buffer[index * 4 + 1] = pixel.y;
		buffer[index * 4 + 2] = pixel.z;
		buffer[index * 4 + 3] = 0;
	}


	void FillPixelAtXY(uint8_t* buffer, const int x, const int y, 
		const int viewWidth, const int viewHeight, const int skipEven)
	{
		int index = y*viewWidth + x;

		int W2 = viewWidth / 2;
		int H2 = viewHeight / 2;


		if (skipPixels)
			if (y % 2 == 0)
				if (x % 2 == skipEven) return;
			else
				if (x % 2 == (skipEven == 0 ? 1 : 0)) return;

		float dY = (float(y - H2) / W2);
		float dX = (float(x - W2) / W2);

		glm::vec4 rayDy = player->vy * dY;
		glm::vec4 rayDx = player->vz * dX;
		glm::vec4 raycastVec = player->vx + rayDy + rayDx;
		FillPixel(player->pos, raycastVec, buffer, index);
	}

	void ThreadedCycle(uint8_t* buffer, const int viewWidth, const int viewHeight, const int skipEven)
	{
		#pragma omp parallel for
		for (int x = 0; x < viewWidth; x++)
			for (int y = 0; y < viewHeight; y++)
			{
				FillPixelAtXY(buffer, x, y, viewWidth, viewHeight, skipEven);
			}
	}

	void SimpleCycle(uint8_t* buffer, const int viewWidth, const int viewHeight, const int skipEven)
	{
		for (int x = 0; x < viewWidth; x++)
			for (int y = 0; y < viewHeight; y++)
			{
				FillPixelAtXY(buffer, x, y, viewWidth, viewHeight, skipEven);
			}
	}

	void FillTexData(uint8_t* buffer, const int viewWidth, const int viewHeight)
	{
		static int skipEven = 0;

		if (useMP)
			ThreadedCycle(buffer, viewWidth, viewHeight, skipEven);
		else
			SimpleCycle(buffer, viewWidth, viewHeight, skipEven);
		skipEven = skipEven == 0 ? 1 : 0;
	}

	bool useMP;
	bool skipPixels;

	Player* player = nullptr;
	Field* field = nullptr;
	Raycaster* raycaster = nullptr;
};