#include "Player.h"
#include <iostream>
using namespace std;


Player::Player(int team, int x, int y, int id, int lastRoomIndex, int healthLowThreshold, int healthHighThreshold, int ammuThreshold)
{
	health = 20;
	ammunition = 5;
	alive = true;
	this->x = x;
	this->y = y;
	this->team = team;
	this->id = id;
	this->lastRIndex = lastRoomIndex;
	this->lastMode = 0;
	this->lowHealth = healthLowThreshold;
	this->highHealth = healthHighThreshold;
	this->ammunitionThreshold = ammuThreshold;
	cout << "_________________________________ADDED PLAYER_______________________" << "\n";
	cout << "player -> " << this->id << " IN TEAM NUM: " << team << " AMMUNITION:" << ammunition << " HEALTH: " << health << "\n";
	cout << "HEALTH LOW THRESHOLD: " << healthLowThreshold << " HEALTH HIGH THRESHOLD: " << healthHighThreshold << " AMMUNITION THRESHOLD: " << ammuThreshold << "\n";
	cout << "_____________________________________________________________________" << "\n";

}

Player::Player()
{
}

Player::~Player()
{
}

void Player::hit()
{
	if (ammunition > 0)
	{
		ammunition--;
		cout << "PLAYER-> " << this->id << " CURRENT AMMUNITION ->  " << ammunition << " \n";
	}
	else
	{
		cout << "PLAYER-> " << this->id << " SHOOT ERROR! -> NO EMMUNITION  \n";
	}
}

int Player::getId()
{
	return this->id;
}

bool Player::isAmmu()
{
	return (ammunition > 0);
}

void Player::loadAmmu(int num)
{
	ammunition += num;
	if (ammunition >= 10)
		ammunition = 10;
	cout << "PLAYER -> " << this->id << " LOAD AMMUNITION ->" << ammunition << "\n";
}

void Player::loadHealth(int num)
{
	this->health += num;
	if (this->health >= 100)
		this->health = 100;
	cout << "PLAYER -> " << this->id << " LOAD HEALTH ->" << health << "\n";
}

void Player::gotHit(int damage)
{
	health -= damage;
	cout << "PLAYER -> " << this->id << " GOT HIT-> " << damage << " HEALTH DAMAGE TO -> " << health << "\n";
	if (health <= 0)
		alive = false;
}

bool Player::isThere(int x, int y)
{
	return (x == this->x && y == this->y);
}

int Player::getTeam()
{
	return this->team;
}

int Player::getX()
{
	return this->x;
}

int Player::getY()
{
	return this->y;
}

void Player::move(int x, int y)
{
	this->x = x;
	this->y = y;
}

bool Player::isAlive()
{
	return this->alive;
}

int Player::getMode()
{
	int mode;
	string tmp;
	if (health < this->lowHealth) {
		mode = SURVIVE;
		tmp = "SURVIVE";
	}
	else if (health <= this->highHealth && ammunition <= this->ammunitionThreshold) {
		mode = ATTACK__HEALTH_AMMU;
		tmp = "ATTACK AND AMMU AND HEALTH";
	}
	else if (ammunition == 0) {
		mode = NEED_AMMUNITION;
		tmp = "NEED AMMU";
	}
	else if (ammunition <= this->ammunitionThreshold) {
		mode = ATTACK_AND_AMMU;
		tmp = "ATTACK AND AMMU";
	}
	else if (health <= this->highHealth) {
		mode = ATTACK_AND_HEALTH;
		tmp = "ATTACK AND HEALTH";
	}
	else {
		mode = ATTACK;
		tmp = "ATTACK";
	}
	if (mode != lastMode)
	{
		cout << "_______________________________________MODE_______________________" << "\n";
		cout << "PLAYER -> " << this->id << " CHANGED MODE TO -> " << tmp << "\n";
	}
	lastMode = mode;
	return mode;
}

void Player::setLastRIndex(int index)
{
	this->lastRIndex = index;
}

int Player::getLastRIndex()
{
	return this->lastRIndex;
}



