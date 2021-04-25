#pragma once
#include "Bullet.h"
#include "Node.h"

const int NUM_BULLETS =20;

class Grenade
{
private:
	double x, y;
	Bullet* bullets[NUM_BULLETS];
public:
	Grenade();
	Grenade(double x, double y);
	~Grenade();
	void Explode();
	void DrawMe();
	void moveBullets(Node maze[MSZ][MSZ]);
	void UpdateSecurityMap(double map[MSZ][MSZ], Node maze[MSZ][MSZ]);


};

