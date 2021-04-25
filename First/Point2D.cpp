#include "Point2D.h"
#include <string>


Point2D::Point2D()
{
}

Point2D::Point2D(Point2D * p)
{
	this->col = p->col;
	this->row = p->row;
}

Point2D::Point2D(int xVal, int yVal)
{
	this->col = xVal;
	this->row = yVal;
}


Point2D::~Point2D()
{
}

int Point2D::getRow()
{
	return row;
}
int Point2D::getCol() {
	return col;
}
void Point2D::setRow(int r) {
	row = r;
}
void Point2D::setCol(int c)
{
	col = c;
}

std::string Point2D::toString()
{
	std::string str;
	str.append("(");
	str.append(std::to_string(this->col));
	str.append(";");
	str.append(std::to_string(this->row));
	str.append(")");
	return str;
}


bool Point2D::equals(Point2D * p)
{
	return p->col == this->col && this->row == p->row;
}
