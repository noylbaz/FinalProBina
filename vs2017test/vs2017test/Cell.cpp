#include "Cell.h"

Cell::Cell(int r, int c, Cell* pc)
{
	row = r;
	col = c;
	parent = pc;
}

int Cell::GetRow()
{
	return row;
}

int Cell::GetCol()
{
	return col;
}

Cell* Cell::GetParent()
{
	return parent;
}
