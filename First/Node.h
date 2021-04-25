#pragma once
#include "Point2D.h"
#include <vector>

const int SPACE = 0;
const int WALL = 1;
const int START = 2;
const int TARGET = 3;
const int PATH = 4; // belongs to the path to target
const int GRAY = 5; // Fringe
const int BLACK = 6; // VISITED
const int HEALTH = 10; // VISITED
const int AMMUNITION = 11; // VISITED
const int PLAYER_TEAM1 = 12;
const int PLAYER_TEAM2 = 13;


class Node
{
public:
	Node();
	~Node();

	Node(Point2D & pt, Point2D * t, int v, double g, Node * pr);
	Node(Node* n);
private:
	int val, roomIndex, searchVal;
	double h, g;
	double denger, fVal, gVal;
	Node* parent;
	Point2D point;
	Point2D* trg;


public:
	void SetRoomIndex(int index);
	int GetRoomIndex();
	void SetVal(int value);
	int GetVal();
	void setFVal(double val);
	double getFVal();
	double getG();
	double getGVal();
	void setGVal(double val);
	double ComputeH();
	double getF();
	void setDng(double danger);
	double getDng();
	int getSearchVal();
	void setSearchVal(int value);
	void setPoint(int x, int y);
	Point2D getPoint();
	Node* getPrnt();
	void setPrnt(Node* parent);
	Point2D* getTrg();
	bool operator == (const Node &other) {
		return point == other.point;
	}
	bool Equals(Node* other);
	bool walkable();

};

