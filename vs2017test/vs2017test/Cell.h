#pragma once
class Cell
{
private:
	int row, col;
	Cell* parent;
public:
	Cell(int r, int c, Cell* pc);
	int GetRow();
	int GetCol();
	Cell* GetParent();
};

