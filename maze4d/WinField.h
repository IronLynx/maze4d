#pragma once

#include <Field.h>

class Field;

class WinField
{
public:
	static const glm::ivec4 winMapSize;
	static Field* CreateWinRoom(Field* const field);
};
