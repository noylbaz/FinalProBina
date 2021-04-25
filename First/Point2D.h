#pragma once
#include <string>

class Point2D
{
public:
	Point2D();
	Point2D(Point2D* p);
	Point2D(int xVal, int yVal);
	~Point2D();
	int getRow();
	int getCol();
	void setRow(int r);
	void setCol(int c);
	std::string toString();
	bool operator == (const Point2D &other)
	{
		return row == other.row && col == other.col;
	}
	bool equals(Point2D* p);
private:
	int row, col;
};

