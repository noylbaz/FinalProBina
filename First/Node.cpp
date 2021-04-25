#include "Node.h"
#include <math.h>


Node::Node()
{
	val = SPACE;
	this->searchVal = SPACE;
	parent = nullptr;
	g = 0;
	this->roomIndex = -1;
}


Node::~Node()
{
}

Node::Node(Point2D &pt, Point2D* t, int v, double g, Node* pr) {
	point = pt;
	trg = t;
	val = v;
	parent = pr;
	this->g = g;
	h = ComputeH();
	this->gVal = 0;
}

Node::Node(Node * n)
{
	this->point = n->getPoint();
	if (n->getPrnt() == nullptr)
		parent = nullptr;
	else
		parent = n->getPrnt();
	this->val = n->GetVal();
	this->h = n->h;
	this->g = n->g;
	this->fVal = n->fVal;
	this->gVal = n->gVal;
	this->searchVal = n->searchVal;
}

void Node::SetRoomIndex(int index)
{
	this->roomIndex = index;
}

int Node::GetRoomIndex()
{
	return this->roomIndex;
}

void Node::SetVal(int value)
{
	this->val = value;
}

int Node::GetVal()
{
	return val;
}

void Node::setFVal(double val)
{
	this->fVal = val;
}

double Node::getFVal()
{
	return this->fVal;
}

double Node::getG()
{
	return g;
}

double Node::getGVal()
{
	return this->gVal;
}

void Node::setGVal(double val)
{
	this->gVal = val;
}



double Node::ComputeH()
{
	return sqrt(pow(point.getRow() - trg->getRow(), 2) +
		pow(point.getCol() - trg->getCol(), 2));
}

double Node::getF()
{
	return g + h;
}

void Node::setDng(double danger)
{
	this->denger = danger;
}

double Node::getDng()
{
	return this->denger;
}

int Node::getSearchVal()
{
	return this->searchVal;
}

void Node::setSearchVal(int value)
{
	this->searchVal = value;
}

void Node::setPoint(int x, int y)
{
	this->point = Point2D(x, y);
}

Point2D Node::getPoint()
{
	return point;
}

Node * Node::getPrnt()
{
	return parent;
}

void Node::setPrnt(Node * parent)
{
	this->parent = parent;
}

Point2D * Node::getTrg()
{
	return trg;
}

bool Node::Equals(Node * other)
{
	return this->point.getRow() == other->point.getRow() && this->point.getCol() == other->point.getCol();
}

bool Node::walkable()
{
	return this->val != WALL && this->val != PLAYER_TEAM1 && this->val != PLAYER_TEAM2;
}


