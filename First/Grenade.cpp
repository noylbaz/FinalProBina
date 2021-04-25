#include "Grenade.h"
#include <iostream>
using namespace std;

const double PI = 3.14;



Grenade::Grenade()
{
}

Grenade::Grenade(double x, double y)
{
	int i;
	double alpha, delta = 2 * PI / NUM_BULLETS;
	this->x = x;
	this->y = y;
	for (i = 0, alpha = 0; i < NUM_BULLETS; i++, alpha += delta)
	{
		bullets[i] = new Bullet(x, y);
		bullets[i]->SetDirection(alpha);
	}

}


Grenade::~Grenade()
{
}

void Grenade::Explode()
{
	for (int i = 0; i < NUM_BULLETS; i++) {
		if (this != NULL) {
			bullets[i]->SetIsMoving(true);
		}
		else {
			cout << "OOP'S -> NO BULLET\n";
			return;
		}
	}
}

void Grenade::DrawMe()
{
	for (int i = 0; i < NUM_BULLETS; i++)
		bullets[i]->DrawMe();
}

void Grenade::moveBullets(Node maze[MSZ][MSZ])
{
	for (int i = 0; i < NUM_BULLETS; i++)
		bullets[i]->move(maze);

}

void Grenade::UpdateSecurityMap(double map[MSZ][MSZ], Node maze[MSZ][MSZ])
{
	for (int i = 0; i < NUM_BULLETS; i++)
	{
		bullets[i]->SetIsMoving(true);
		bullets[i]->SimulateMot(map, maze);
	}

}
