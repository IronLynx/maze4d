#pragma once


class Cell
{
public:
	Cell() {}
	Cell(uint8_t cellType, uint8_t alphaValue, bool isWinBlock)
	{
		this->cellType = cellType;
		this->alphaValue = alphaValue;
		this->isWinBlock = isWinBlock;
	}

	uint8_t cellType = 0; //0 - empty, 1 - regular or 2 - light
	uint8_t alphaValue = 255; //0-255 transparency. 0 - fully transparent, 255 fully solid
	bool isWinBlock = false;
};