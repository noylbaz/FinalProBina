#pragma once
#include "Node.h"
const int MSZ = 100;
const double teta = 0.0001;

class Bullet
{
private:
	double x, y;
	double dx, dy;
	bool isMoving;
public:
	Bullet();
	Bullet(double x, double y);
	~Bullet();
	void DrawMe();
	void SetIsMoving(bool move);
	bool GetIsMoving();
	void move(Node maze[MSZ][MSZ]);
	double getX();
	double getY();
	void SetDirection(double angle);
	void SimulateMot(double map[MSZ][MSZ], Node maze[MSZ][MSZ]);


};

