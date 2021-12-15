#pragma once

#include "STATES.h"

struct MatrixElement
{
public:

	int x;
	int y;
	STATES state;
	int cellIndex;

	MatrixElement() = default;

	MatrixElement(unsigned int x, unsigned int y, STATES state, int cellIndex)
	{
		this->x = x;
		this->y = y;
		this->state = state;
		this->cellIndex = cellIndex;
	};
};