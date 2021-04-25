#pragma once
#include <string>
const int SURVIVE = 100;
const int ATTACK = 500;
const int ATTACK__HEALTH_AMMU = 490;
const int ATTACK_AND_AMMU = 470;
const int ATTACK_AND_HEALTH = 450;
const int NEED_AMMUNITION = 150;

class Player
{
private:
	int health, ammunition, team, x, y, id, lastRIndex, lastMode, lowHealth, highHealth, ammunitionThreshold;
	bool alive;
public:
	Player(int team, int x, int y, int id, int lastRoomIndex, int healthLowThreshold, int healthHighThreshold, int ammuThreshold);
	Player();
	~Player();
	void hit();
	int getId();
	bool isAmmu();
	void loadAmmu(int num);
	void loadHealth(int num);
	void gotHit(int damage);
	bool isThere(int x, int y);
	int getTeam();
	int getX();
	int getY();
	void move(int x, int y);
	bool isAlive();
	int getMode();
	void setLastRIndex(int index);
	int getLastRIndex();

};

