#include "Bullet.h"
#include "GLUT.H"
#include <math.h>

Bullet::Bullet()
{
}

Bullet::Bullet(double x, double y)
{
	double len;
	this->x = x;
	this->y = y;
	dx = (rand() % 101) - 50;
	dy = (rand() % 101) - 50;
	len = sqrt(dx*dx + dy * dy);
	dx /= len;
	dy /= len;
	isMoving = false;
}


Bullet::~Bullet()
{
}

void Bullet::DrawMe()
{
	glColor3d(0, 0, 0);
	glBegin(GL_POLYGON);
	glVertex2d(x - 0.01, y);
	glVertex2d(x, y + 0.01);
	glVertex2d(x + 0.01, y);
	glVertex2d(x, y - 0.01);
	glEnd();
}

void Bullet::SetIsMoving(bool move)
{
	isMoving = move;
}

bool Bullet::GetIsMoving()
{
	return isMoving;
}

void Bullet::move(Node maze[MSZ][MSZ])
{
	int i, j;
	i = MSZ * (y + 1) / 2;
	j = MSZ * (x + 1) / 2;

	if (isMoving && maze[i][j].GetVal() == SPACE)
	{
		x += 0.001*dx;
		y += 0.001*dy;
	}
}

double Bullet::getX()
{
	return x;
}

double Bullet::getY()
{
	return y;
}

void Bullet::SetDirection(double angle)
{
	dx = cos(angle);
	dy = sin(angle);
}

void Bullet::SimulateMot(double map[MSZ][MSZ], Node maze[MSZ][MSZ])
{
	int i, j;
	i = MSZ * (y + 1) / 2;
	j = MSZ * (x + 1) / 2;

	while (maze[i][j].GetVal() == SPACE)
	{
		map[i][j] += teta;
		x += 0.001*dx;
		y += 0.001*dy;
		i = MSZ * (y + 1) / 2;
		j = MSZ * (x + 1) / 2;
	}

}
