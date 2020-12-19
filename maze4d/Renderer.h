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

	void FillPixel(glm::vec4& pos, glm::vec4& v, uint8_t* buffer, int viewWidth, int viewHeight, int index)
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

		buffer[index * 3    ] = pixel.x;
		buffer[index * 3 + 1] = pixel.y;
		buffer[index * 3 + 2] = pixel.z;
	}

	void FillPixelAtXY(uint8_t* buffer, const int viewWidth, const int viewHeight, const int i,
		const int H2, const int W2, const float ratioH2, const int skipEven)
	{
		int y = i / viewWidth;
		int x = i % viewWidth;

		if (skipPixels)
		{
			if (y % 2 == 0)
			{
				if (x % 2 == skipEven) return;
			}
			else
			{
				if (x % 2 == (skipEven == 0 ? 1 : 0)) return;
			}
		}

		glm::vec4 tmpVecY = player->vy * (float(y - H2) / ratioH2);
		glm::vec4 tmpVecZ = player->vz * (float(x - W2) / W2);
		glm::vec4 tmpVecX = player->vx + tmpVecY + tmpVecZ;
		FillPixel(player->pos, tmpVecX, buffer, viewWidth, viewHeight, i);
	}

	void ThreadedCycle(uint8_t* buffer, const int viewWidth, const int viewHeight,
		const int H2, const int W2, const float ratioH2, const int skipEven)
	{
		#pragma omp parallel for
		for (int i = 0; i < viewHeight*viewWidth; i++)
		{
			FillPixelAtXY(buffer, viewWidth, viewHeight, i, H2, W2, ratioH2, skipEven);
		}
	}

	void SimpleCycle(uint8_t* buffer, const int viewWidth, const int viewHeight,
		const int H2, const int W2, const float ratioH2, const int skipEven)
	{
		for (int i = 0; i < viewHeight*viewWidth; i++)
		{
			FillPixelAtXY(buffer, viewWidth, viewHeight, i, H2, W2, ratioH2, skipEven);
		}
	}

	void FillTexData(uint8_t* buffer, const int viewWidth, const int viewHeight)
	{
		static int skipEven = 0;

		const int W2 = viewWidth / 2;
		const int H2 = viewHeight / 2;
		const float ratioH2 = H2 / (float(viewHeight) / viewWidth);

		if (useMP)
			ThreadedCycle(buffer, viewWidth, viewHeight, H2, W2, ratioH2, skipEven);
		else
			SimpleCycle(buffer, viewWidth, viewHeight, H2, W2, ratioH2, skipEven);
		skipEven = skipEven == 0 ? 1 : 0;
	}

	bool useMP;
	bool skipPixels;

	Player* player = nullptr;
	Field* field = nullptr;
	Raycaster* raycaster = nullptr;
};