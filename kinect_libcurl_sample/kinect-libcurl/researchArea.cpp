#include "researchArea.hpp"
#include <string>

researchArea::researchArea(void)
{
	count = 0;
	status = 0;
}


researchArea::~researchArea(void)
{
}


researchArea::researchArea(int minRow, int maxRow, int minCol, int maxCol)
{
	this->minRow = minRow;
	this->maxRow = maxRow;
	this->minCol = minCol;
	this->maxCol = maxCol;
	this->count = 0;
}