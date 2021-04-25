//NOY ELBAZ 315073122 
//SHELLY FAINBERG 208469429

#include "glut.h"
#include <time.h>
#include <vector>
#include "GLUT.h"
#include <math.h>
#include "Node.h"
#include "Room.h"
#include <string>
#include <iostream>
#include <queue>
#include "CompareNodes.h"
#include "CompareNodesFVal.h"
#include "Player.h"
#include <time.h>
#include <map>
#include <sstream>
#include "Bullet.h"
#include "Grenade.h"

using namespace std;

const int WIDTH = 800;
const int HEIGHT = 800;

const int NUM_ROOMS = 12;
const int PLAYERS = 2;

Node maze[MSZ][MSZ];// 0 is SPACE
double security_map[MSZ][MSZ] = { 0 };// danger map: 0 means no danger
Room rooms[NUM_ROOMS];

Bullet* pb = nullptr;
Grenade* pg = nullptr;;

bool run_bfs = false;
const string MAP_KEY_ROOM_TRG = "roomsTrg";
const string MAP_KEY_HIT_TRG = "hit";

int existRooms = 0;
int aliveTeam1 = 0;
int aliveTeam2 = 0;
int numOfPlayersAlive = 0;
int healthPoints = 10;
int ammunitionPoints = 10;

queue <Player*> playerSteps;
queue <Point2D*> health;
queue <Point2D*> ammunition;

bool defineTeams = false;
bool startPlay = false;
bool resume = false;

double calculateDistance(int x, int y, int xx, int yy);

vector <Node> gray;
Point2D* getWayTarget(Point2D* curPos, Point2D* tarPos);
vector<Point2D*> NWallNeighbors(Point2D* p);

Point2D* enemysHit(int posTeam, int x1, int y1, int x2, int y2);
vector<Point2D*> ableNeighborsWalk(Point2D* point);

string DirectionOfHits(int target1, int target2, int direction1, int direction2);
Player* setPlayerLocation(int location1, int location2);

void GenerateSecurityMap();
void InitMaze();
int calculateDamage(double length);

Point2D* source = new Point2D(0, 0);
struct PointsThatCloseToSrc
{
	bool operator ()(Point2D* point1, Point2D* point2)
	{
		return calculateDistance(point1->getCol(), point1->getRow(), source->getCol(), source->getRow()) > calculateDistance(point2->getCol(), point2->getRow(), source->getCol(), source->getRow());
	}
};

void init()
{
	glClearColor(0.7, 0.7, 0.7, 0);// color of window background
	glOrtho(-1, 1, -1, 1, -1, 1);

	srand(time(0));

	InitMaze();
	GenerateSecurityMap();

	for (int i = 0; i < MSZ; i++)
	{
		for (int j = 0; j < MSZ; j++)
		{
			maze[i][j].setDng(security_map[i][j]);
		}
	}
}

Room InitRooms(int roomIndex)
{
	int i, j;
	int w, h, row, col;
	bool overlap;
	Room* r = nullptr;

	do
	{
		overlap = false;
		w = 6 + rand() % 10;
		h = 6 + rand() % 10;

		col = w / 2 + rand() % (MSZ - w);
		row = h / 2 + rand() % (MSZ - h);

		delete r;
		r = new Room(row, col, w, h);

		Point2D leftop = r->getLeftTop();
		Point2D rightbottom = r->getRightBottom();

		if (rightbottom.getCol() == 0 || rightbottom.getRow() == 0 || rightbottom.getRow() >= MSZ - 1 || rightbottom.getCol() >= MSZ - 1 ||
			leftop.getCol() == 0 || leftop.getRow() == 0 || leftop.getRow() >= MSZ - 1 || leftop.getCol() >= MSZ - 1)
			overlap = true;
		// check if the above definitions don't make a room to overlap with any other room
		for (j = 0; j < existRooms && !overlap; j++)
		{
			if (rooms[j].IsOverlap(r))
				overlap = true;
		}
	} while (overlap);

	for (i = r->getLeftTop().getRow(); i <= r->getRightBottom().getRow(); i++) {
		for (j = r->getLeftTop().getCol(); j <= r->getRightBottom().getCol(); j++) {
			maze[i][j].SetVal(SPACE);
			maze[i][j].SetRoomIndex(roomIndex);
		}
	}
	return *r;
}

void AddNode(int row, int col, Node* pn, vector<Node>& gray, vector<Node>& black,
	priority_queue <Node*, vector<Node*>, CompareNodes>& pq)
{
	Point2D p;
	Node* pt;
	vector<Node>::iterator drawGray;
	vector<Node>::iterator drawBlack;
	double space;

	p.setRow(row);
	p.setCol(col);

	if (maze[row][col].GetVal() == WALL)
		space = 3;
	else if (maze[row][col].GetVal() == SPACE)
		space = 0.1;

	pt = new Node(p, pn->getTrg(), maze[p.getRow()][p.getCol()].GetVal(), pn->getG() + space, pn);

	drawBlack = find(black.begin(), black.end(), *pt);
	drawGray = find(gray.begin(), gray.end(), *pt);

	if (drawGray == gray.end() && drawBlack == black.end()) // it is not black and not gray!
	{
		pq.push(pt);
		gray.push_back(*pt);
	}
}

void AddNeighbours(Node* pointNeighbour, vector<Node>& gray, vector<Node>& black,
	priority_queue <Node*, vector<Node*>, CompareNodes>& pq)
{
	if (pointNeighbour->getPoint().getRow() > 0)
		AddNode(pointNeighbour->getPoint().getRow() - 1, pointNeighbour->getPoint().getCol(), pointNeighbour, gray, black, pq);
	if (pointNeighbour->getPoint().getRow() < MSZ - 1)
		AddNode(pointNeighbour->getPoint().getRow() + 1, pointNeighbour->getPoint().getCol(), pointNeighbour, gray, black, pq);
	if (pointNeighbour->getPoint().getCol() < MSZ - 1)
		AddNode(pointNeighbour->getPoint().getRow(), pointNeighbour->getPoint().getCol() + 1, pointNeighbour, gray, black, pq);
	if (pointNeighbour->getPoint().getCol() > 0)
		AddNode(pointNeighbour->getPoint().getRow(), pointNeighbour->getPoint().getCol() - 1, pointNeighbour, gray, black, pq);
}

void InitPath(Point2D start, Point2D target)
{
	priority_queue <Node*, vector<Node*>, CompareNodes> pq;
	vector<Node> gray;
	vector<Node> black;
	Node* pointNeighbour;
	bool stop = false;
	vector<Node>::iterator drawGrey;
	vector<Node>::iterator drawBlack;

	double wall = 10;
	double space = 0.2;

	pointNeighbour = new Node(start, &target, maze[start.getRow()][start.getCol()].GetVal(), 0, nullptr);
	pq.push(pointNeighbour);
	gray.push_back(*pointNeighbour);

	while (!pq.empty() && !stop)
	{
		pointNeighbour = pq.top();
		pq.pop();
		if (!(pointNeighbour->getPoint() == target))
		{
			drawGrey = find(gray.begin(), gray.end(), *pointNeighbour);
			if (drawGrey != gray.end())
				gray.erase(drawGrey);
			black.push_back(*pointNeighbour);

			AddNeighbours(pointNeighbour, gray, black, pq);
		}
		else
		{
			stop = true;
			while (!(pointNeighbour->getPoint() == start))
			{
				maze[pointNeighbour->getPoint().getRow()][pointNeighbour->getPoint().getCol()].SetVal(SPACE);
				pointNeighbour = pointNeighbour->getPrnt();
			}
			return;
		}
	}
}

void DigTunnels()
{
	int i, j;

	for (i = 0; i < NUM_ROOMS; i++)
	{
		cout << "PATH FROM ROOM-> " << i << endl;
		for (j = i + 1; j < NUM_ROOMS; j++)
		{
			cout << " TO ROOM->  " << j << endl;
			InitPath(rooms[i].getCenter(), rooms[j].getCenter());
		}
	}
}

void AddHealth()
{
	int randNum, row, col;
	randNum = rand() % NUM_ROOMS;
	Room randRoom = rooms[randNum];
	Point2D leftTop = randRoom.getLeftTop();
	Point2D rightBottom = randRoom.getRightBottom();
	row = rand() % (rightBottom.getRow() - leftTop.getRow() + 1) + leftTop.getRow();
	col = rand() % (rightBottom.getCol() - leftTop.getCol() + 1) + leftTop.getCol();
	while (maze[row][col].GetVal() != SPACE)
	{
		row = rand() % (rightBottom.getRow() - leftTop.getRow() + 1) + leftTop.getRow();
		col = rand() % (rightBottom.getCol() - leftTop.getCol() + 1) + leftTop.getCol();
	}
	maze[row][col].SetVal(HEALTH);
	health.push(new Point2D(col, row));
}

void AddAmmunition()
{
	int randNum, row, col;
	randNum = rand() % NUM_ROOMS;
	Room randRoom = rooms[randNum];
	Point2D leftTop = randRoom.getLeftTop();
	Point2D rightBottom = randRoom.getRightBottom();
	row = rand() % (rightBottom.getRow() - leftTop.getRow() + 1) + leftTop.getRow();
	col = rand() % (rightBottom.getCol() - leftTop.getCol() + 1) + leftTop.getCol();
	while (maze[row][col].GetVal() != SPACE)
	{
		row = rand() % (rightBottom.getRow() - leftTop.getRow() + 1) + leftTop.getRow();
		col = rand() % (rightBottom.getCol() - leftTop.getCol() + 1) + leftTop.getCol();
	}
	maze[row][col].SetVal(AMMUNITION);
	ammunition.push(new Point2D(col, row));
}

void InitMaze()
{
	int i, j, k;

	for (i = 0; i < MSZ; i++)
		for (j = 0; j < MSZ; j++)
			maze[i][j].SetVal(WALL);


	for (existRooms = 0; existRooms < NUM_ROOMS; existRooms++)
		rooms[existRooms] = InitRooms(existRooms);

	for (k = 0; k < 400; k++)
	{
		i = rand() % MSZ;
		j = rand() % MSZ;
		maze[i][j].SetVal(WALL);
	}

	DigTunnels();

	for (i = 0; i < MSZ; i++)
		for (j = 0; j < MSZ; j++)
			maze[i][j].setPoint(j, i);
	for (int i = 0; i < ammunitionPoints; i++)
	{
		AddAmmunition();
	}
	for (int i = 0; i < healthPoints; i++)
	{
		AddHealth();
	}
}

Point2D* UniformCost(Point2D* source, map<string, string> trgSearch, bool(*target)(Point2D* current, Point2D* source, map<string, string> args))
{
	Node* current;
	queue<Node*> q;
	Point2D* trgLocation = new Point2D(-1, -1);
	bool found = false;
	priority_queue <Node*, vector<Node*>, CompareNodesFVal> pq;
	pq.push(new Node(maze[source->getRow()][source->getCol()]));
	while (!pq.empty() && !found)
	{
		current = pq.top();
		pq.pop();
		if (maze[current->getPoint().getRow()][current->getPoint().getCol()].getSearchVal() == BLACK)
		{
			continue;
		}
		maze[current->getPoint().getRow()][current->getPoint().getCol()].setSearchVal(BLACK);
		q.push(current);
		if (target(new Point2D(current->getPoint()), source, trgSearch))
		{
			found = true;
			trgLocation = new Point2D(current->getPoint());
			while (!pq.empty())
			{
				Node* t = pq.top();
				maze[t->getPoint().getRow()][t->getPoint().getCol()].setSearchVal(SPACE);
				maze[t->getPoint().getRow()][t->getPoint().getCol()].setFVal(0);
				pq.pop();
			}
		}
		else {
			for each (Point2D * n in NWallNeighbors(&current->getPoint()))
			{
				double tmp = maze[current->getPoint().getRow()][current->getPoint().getCol()].getFVal() + 1;
				if (maze[n->getRow()][n->getCol()].getSearchVal() == BLACK)
					continue;
				if (maze[n->getRow()][n->getCol()].getSearchVal() == SPACE)
				{
					maze[n->getRow()][n->getCol()].setFVal(tmp);
				}
				else {
					if (tmp < maze[n->getRow()][n->getCol()].getFVal())
					{
						maze[n->getRow()][n->getCol()].setFVal(tmp);
					}
				}
				maze[n->getRow()][n->getCol()].setSearchVal(GRAY);
				pq.push(new Node(maze[n->getRow()][n->getCol()]));
			}
		}
	}
	while (!q.empty())
	{
		Node* t = q.front();
		maze[t->getPoint().getRow()][t->getPoint().getCol()].setSearchVal(SPACE);
		maze[t->getPoint().getRow()][t->getPoint().getCol()].setFVal(0);
		q.pop();
	}
	return trgLocation;
}

Point2D* nextStepToTrg(Point2D* source, Point2D* target, double(*costFunction) (int x, int y))
{
	int i = source->getRow();
	int j = source->getCol();
	Point2D* next = new Point2D(-1, -1);
	int count = 0;

	if (source->equals(target))
	{
		return next;
	}

	Node* current;
	priority_queue <Node*, vector<Node*>, CompareNodesFVal> pq;
	queue<Node*> q;

	pq.push(new Node(maze[i][j]));
	bool found = false;
	while (!pq.empty() && !found)
	{
		current = pq.top();
		count++;
		pq.pop();
		if (current->getPoint().equals(target))
		{
			next = getWayTarget(new Point2D(current->getPoint()), source);
			while (!pq.empty())
			{
				Node* t = pq.top();
				maze[t->getPoint().getRow()][t->getPoint().getCol()].setPrnt(NULL);
				maze[t->getPoint().getRow()][t->getPoint().getCol()].setFVal(0);
				maze[t->getPoint().getRow()][t->getPoint().getCol()].setSearchVal(SPACE);
				pq.pop();
			}
			found = true;
		}
		maze[current->getPoint().getRow()][current->getPoint().getCol()].setSearchVal(BLACK);
		q.push(current);
		if (!found) {
			for each (Point2D * neigbor in NWallNeighbors(&current->getPoint()))
			{
				if (maze[neigbor->getRow()][neigbor->getCol()].getSearchVal() == SPACE)
				{
					double distance = calculateDistance(neigbor->getCol(), neigbor->getRow(), target->getCol(), target->getRow());
					maze[neigbor->getRow()][neigbor->getCol()].setFVal(distance + costFunction(neigbor->getCol(), neigbor->getRow()));
					maze[neigbor->getRow()][neigbor->getCol()].setSearchVal(GRAY);
					maze[neigbor->getRow()][neigbor->getCol()].setPrnt(current);
					pq.push(new Node(&maze[neigbor->getRow()][neigbor->getCol()]));

				}
			}
		}
	}
	while (!q.empty())
	{
		Node* t = q.front();
		maze[t->getPoint().getRow()][t->getPoint().getCol()].setFVal(0);
		maze[t->getPoint().getRow()][t->getPoint().getCol()].setPrnt(NULL);
		maze[t->getPoint().getRow()][t->getPoint().getCol()].setSearchVal(SPACE);
		q.pop();
	}
	return next;
}

double calculateDistance(int x, int y, int xx, int yy)
{
	return sqrt(pow(xx - x, 2) + pow(yy - y, 2));
}

vector<Point2D*> NWallNeighbors(Point2D* p)
{
	int i = p->getRow();
	int j = p->getCol();
	vector<Point2D*> neigbor;

	if (maze[i][j + 1].GetVal() != WALL)
		neigbor.push_back(new Point2D(j + 1, i));
	if (maze[i][j - 1].GetVal() != WALL)
		neigbor.push_back(new Point2D(j - 1, i));
	if (maze[i + 1][j].GetVal() != WALL)
		neigbor.push_back(new Point2D(j, i + 1));
	if (maze[i - 1][j].GetVal() != WALL)
		neigbor.push_back(new Point2D(j, i - 1));

	return neigbor;
}

vector<Point2D*> ableNeighborsWalk(Point2D* p)
{
	int i = p->getRow();
	int j = p->getCol();
	vector<Point2D*> neigbor;

	if (maze[i][j + 1].walkable())
		neigbor.push_back(new Point2D(j + 1, i));
	if (maze[i][j - 1].walkable())
		neigbor.push_back(new Point2D(j - 1, i));
	if (maze[i + 1][j].walkable())
		neigbor.push_back(new Point2D(j, i + 1));
	if (maze[i - 1][j].walkable())
		neigbor.push_back(new Point2D(j, i - 1));

	return neigbor;
}

void deleteAPoint(Point2D* point)
{
	int i = point->getRow();
	int j = point->getCol();
	bool temp = false;
	queue <Point2D*> tempV;
	while (!ammunition.empty())
	{
		if (!ammunition.front()->equals(point))
			tempV.push(ammunition.front());
		ammunition.pop();
	}
	while (!tempV.empty())
	{
		ammunition.push(tempV.front());
		tempV.pop();
	}
	maze[i][j].SetVal(SPACE);
} // health

void deleteHPoint(Point2D* point)
{
	int i = point->getRow();
	int j = point->getCol();
	bool temp = false;
	queue <Point2D*> tempV;
	while (!health.empty())
	{
		if (!health.front()->equals(point))
			tempV.push(health.front());
		health.pop();
	}

	while (!tempV.empty())
	{
		health.push(tempV.front());
		tempV.pop();
	}
	maze[i][j].SetVal(SPACE);
}//annunition

void AddPlayer(int room, int TValue)
{
	Room temp = rooms[room];
	int i = temp.getRightBottom().getCol();
	int j = temp.getRightBottom().getRow();
	while (maze[j][i].GetVal() != SPACE)
	{
		i--;
		j--;
	}
	maze[j][i].SetVal(TValue);
	Player* p = nullptr;
	int ammu = rand() % 5 + 1;
	int highHealth = rand() % 50 + 30;
	int lowHealth = highHealth - (rand() % 25 + 5);
	p = new Player(TValue, i, j, numOfPlayersAlive, maze[j][i].GetRoomIndex(), lowHealth, highHealth, ammu);
	numOfPlayersAlive++;
	playerSteps.push(p);

}

Point2D* getWayTarget(Point2D* current, Point2D* source)
{
	if (current == source)
		return new Point2D(-1, -1);
	Point2D* position = new Point2D(current);
	Point2D* LPosition = new Point2D(position);
	bool found = false;
	while (!found)
	{
		LPosition = new Point2D(position);
		position = new Point2D(maze[position->getRow()][position->getCol()].getPrnt()->getPoint());

		if (position->equals(source))
			found = true;
	}
	return LPosition;
}

void DrawMaze()
{
	int i, j;
	double sz;
	double x, y;

	for (i = 0; i < MSZ; i++)
		for (j = 0; j < MSZ; j++)
		{
			switch (maze[i][j].GetVal()) 
			{
			case WALL:
				glColor3d(0.4, 0.0, 0.2); // dark red
				break;
			case START:
				glColor3d(0, 1, 1);   // cyan
				break;
			case TARGET:
				glColor3d(1, 0, 0); // red
				break;
			case PLAYER_TEAM1:
				glColor3d(1, 0, 0); // red
				break;
			case PLAYER_TEAM2:
				glColor3d(0.7, 0.0, 0.5); //pink
				break;
			case HEALTH:
				glColor3d(0.0, 0.0, 0.9); // blue
				break;
			case AMMUNITION:
				glColor3d(0.0, 0.7, 0.2); //green
				break;
			case SPACE:
				glColor3d(1, 1, 1); // white
				break;
			}

			sz = 2.0 / MSZ; //rectangle
			x = j * sz - 1;
			y = i * sz - 1;

			// draw square maze[i][j]
			x = 2 * (j / (double)MSZ) - 1;
			y = 2 * (i / (double)MSZ) - 1;

			glBegin(GL_POLYGON); // fill up
			glVertex2d(x, y);
			glVertex2d(x + sz, y);
			glVertex2d(x + sz, y + sz);
			glVertex2d(x, y + sz);

			glEnd();
		}
}

void DrawMap()
{
	int i, j;
	double  sx, sy, sz;

	for (i = 0; i < MSZ; i++)
		for (j = 0; j < MSZ; j++)
		{
			if (maze[i][j].GetVal() == SPACE)
			{
				double c;
				c = 1 - security_map[i][j];// 1(white) is very safe, 0(black) is very dangerous
				glColor3d(c, c, c);

				sz = 2.0 / MSZ; //rectangle
				sx = (j * sz - 1);
				sy = i * sz - 1;

				glBegin(GL_POLYGON); // fill up
				glVertex2d(sx, sy);
				glVertex2d(sx + sz, sy);
				glVertex2d(sx + sz, sy + sz);
				glVertex2d(sx, sy + sz);

				glEnd();
			}
		}
}

void GenerateSecurityMap()
{
	int num_tries = 5000;
	int counter;
	int x, y;
	double xx, yy, sz;
	sz = 2 / MSZ;
	Grenade* pg1;

	for (counter = 1; counter <= num_tries; counter++)
	{
		do
		{
			x = rand() % MSZ; // x and y are random pixels
			y = rand() % MSZ;
		} while (maze[y][x].GetVal() != SPACE);
		xx = x * sz - 1;
		yy = y * sz - 1; // now xx,yy are in range [-1,1]
		pg1 = new Grenade(xx, yy);
		pg1->UpdateSecurityMap(security_map, maze);
	}
}

void display()
{
	int i;
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer

	DrawMaze();

	//	if (pb != nullptr)
	//	pb->showMe();
	if (pg)
		pg->DrawMe();


	glutSwapBuffers();// show all
}

void displayMap()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer

	DrawMaze();
	DrawMap();

	glutSwapBuffers();// show all
}

bool findSpaces(double x, double y)
{
	int row, col;
	row = MSZ * (y + 1) / 2;
	col = MSZ * (x + 1) / 2;
	return  maze[row][col].GetVal() == SPACE;
}

double functionAttack(int x, int y)
{
	return (security_map[y][x] + 3);
}

double functionH(int x, int y)
{
	int temp;
	temp = (maze[y][x].GetVal() == HEALTH) ? 1 : 0;
	return (security_map[y][x] + 1) + temp * (-1.5);
}

double functionA(int x, int y)
{
	int temp;
	temp = (maze[y][x].GetVal() == AMMUNITION) ? 1 : 0;
	return (security_map[y][x] + 1) + temp * (-1.5);
}

double functionAHA(int x, int y)
{
	int tempA;
	int tempH;
	tempA = (maze[y][x].GetVal() == AMMUNITION) ? 1 : 0;
	tempH = (maze[y][x].GetVal() == HEALTH) ? 1 : 0;
	return (security_map[y][x] + 3) + tempH * (-1.5) + tempA * (-1.5);
}

std::vector<std::string> split(std::string strToSplit, char delimeter)
{
	std::stringstream ss(strToSplit);
	std::string item;
	std::vector<std::string> str;
	while (std::getline(ss, item, delimeter))
	{
		str.push_back(item);
	}
	return str;
}

string getPlayerHitSpace(Point2D* p)
{
	string space = "";
	int i = p->getRow();
	int j = p->getCol();
	for (int row = -1; row <= 1; row++)
	{
		for (int col = -1; col <= 1; col++)
		{
			if (row == 0 && col == 0)
				continue;
			space += DirectionOfHits(j, i, row, col);
		}
	}
	return space;
}

string DirectionOfHits(int tX, int tY, int dX, int dY)
{
	string space = "";
	int x = tX + dX;
	int y = tY + dY;
	while (maze[y][x].GetRoomIndex() == maze[tY][tX].GetRoomIndex() && maze[y][x].GetVal() == SPACE)
	{
		space += maze[y][x].getPoint().toString() + ",";
		x += dX;
		y += dY;
	}
	return space;
}

bool shootPosRoom(Point2D* current, Point2D* source, map<string, string> args)
{
	string space = args[MAP_KEY_HIT_TRG];
	size_t found = space.find(current->toString());
	return found != string::npos;
}

bool shootPosRoomHealtAmm(Point2D* current, Point2D* source, map<string, string> args)
{
	int i = current->getRow();
	int j = current->getCol();
	if (maze[i][j].GetVal() == AMMUNITION || maze[i][j].GetVal() == HEALTH)
		return true;
	string spots = args[MAP_KEY_HIT_TRG];
	size_t found = spots.find(current->toString());
	return found != string::npos;
}

bool shootPosRoomAmmu(Point2D* current, Point2D* source, map<string, string> args)
{
	int i = current->getRow();
	int j = current->getCol();
	if (maze[i][j].GetVal() == AMMUNITION)
		return true;
	string space = args[MAP_KEY_HIT_TRG];
	size_t found = space.find(current->toString());
	return found != string::npos;
}

bool shootPosRoomHealth(Point2D* current, Point2D* source, map<string, string> args)
{
	int i = current->getRow();
	int j = current->getCol();
	if (maze[i][j].GetVal() == HEALTH)
		return true;
	string spots = args[MAP_KEY_HIT_TRG];
	size_t found = spots.find(current->toString());
	return found != string::npos;
}

bool attackTrgRoom(Point2D* current, Point2D* source, map<string, string> args)
{
	string temp;
	vector <string> vec;
	int i = current->getRow();
	int j = current->getCol();
	vec = split(args[MAP_KEY_ROOM_TRG], ',');
	for each (string v in vec)
	{
		temp = std::to_string(maze[i][j].GetRoomIndex());
		if (temp.compare(v) == 0) {
			return true;
		}
	}
	return false;
}

bool attackTrgRoomHealtAmmunition(Point2D* current, Point2D* source, map<string, string> args)
{
	string temp;
	vector <string> vec;
	int i = current->getRow();
	int j = current->getCol();

	if (maze[i][j].GetVal() == HEALTH || maze[i][j].GetVal() == AMMUNITION)
		return true;
	vec = split(args[MAP_KEY_ROOM_TRG], ',');
	for each (string v in vec)
	{
		temp = std::to_string(maze[i][j].GetRoomIndex());
		if (temp.compare(v) == 0) {
			return true;
		}
	}
	return false;
}

bool attackTrgRoomAmmunition(Point2D* current, Point2D* source, map<string, string> args)
{
	string temp;
	vector <string> vec;
	int i = current->getRow();
	int j = current->getCol();

	if (maze[i][j].GetVal() == AMMUNITION)
		return true;
	vec = split(args[MAP_KEY_ROOM_TRG], ',');
	for each (string v in vec)
	{
		temp = std::to_string(maze[i][j].GetRoomIndex());
		if (temp.compare(v) == 0) {
			return true;
		}
	}
	return false;
}

bool attackTrgRoomHealth(Point2D* current, Point2D* source, map<string, string> args)
{
	string temp;
	vector <string> tokens;
	int i = current->getRow();
	int j = current->getCol();
	if (maze[i][j].GetVal() == HEALTH)
		return true;
	tokens = split(args[MAP_KEY_ROOM_TRG], ',');
	for each (string token in tokens)
	{
		temp = std::to_string(maze[i][j].GetRoomIndex());
		if (temp.compare(token) == 0) {
			return true;
		}
	}
	return false;
}

bool survivorInSameRoom(Point2D* current, Point2D* source, map<string, string> args)
{
	if (maze[current->getRow()][current->getCol()].GetVal() == HEALTH)
		return true;
	if (maze[current->getRow()][current->getCol()].GetRoomIndex() == maze[source->getRow()][source->getCol()].GetRoomIndex())
		return false;
	return false;
}

bool survivorNotInSameRoom(Point2D* current, Point2D* source, map<string, string> args)
{
	if (maze[current->getRow()][current->getCol()].GetVal() == HEALTH)
		return true;
	return false;
}

vector<Player*> getEnemies(Player* p)
{
	vector<Player*> e;
	for (int i = 0; i < playerSteps.size(); i++)
	{
		Player* pTemp = playerSteps.front();
		if (pTemp->isAlive() && pTemp->getTeam() != p->getTeam()) {
			e.push_back(pTemp);
		}
		playerSteps.pop();
		playerSteps.push(pTemp);
	}
	return e;
}

bool needAmmunition(Point2D* current, Point2D* source, map<string, string> args)
{
	if (maze[current->getRow()][current->getCol()].GetVal() == AMMUNITION)
		return true;
	return false;
}

bool isSameRoom(Player* p)
{
	for each (auto enemy in getEnemies(p))
	{
		int ex1 = enemy->getX();
		int ex2 = p->getX();
		int ey1 = enemy->getY();
		int ey2 = p->getY();
		if (maze[ey1][ex1].GetRoomIndex() == maze[ey2][ex2].GetRoomIndex())
			return true;
	}
	return false;
}

queue<Point2D*> enimiesInSameRoom(Player* p)
{
	source->~Point2D();
	int ex2 = p->getX();
	int ey2 = p->getY();
	source = new Point2D(ex2, ey2);
	priority_queue <Point2D*, vector<Point2D*>, PointsThatCloseToSrc> pq;
	queue<Point2D*> result;

	for each (Player * enemy in getEnemies(p))
	{
		int ex1 = enemy->getX();
		int ey1 = enemy->getY();

		if (maze[ey1][ex1].GetRoomIndex() == maze[ey2][ex2].GetRoomIndex())
		{
			pq.push(new Point2D(ex1, ey1));
		}
	}
	while (!pq.empty())
	{
		result.push(pq.top());
		pq.pop();
	}
	return result;
}

bool hitEnemy(Player* p, int& teamAlive, double thresholdLength)
{
	int row, col;
	priority_queue <Point2D*, vector<Point2D*>, PointsThatCloseToSrc> pq;
	source->~Point2D();
	source = new Point2D(p->getX(), p->getY());
	for (row = -1; row <= 1; row++)
	{
		for (col = -1; col <= 1; col++)
		{
			Point2D* el = enemysHit(p->getTeam(), p->getX(), p->getY(), row, col);
			if (el->getRow() != -1) {
				pq.push(el);
			}
		}
	}
	if (pq.size() == 0) {
		return false;
	}
	double length = calculateDistance(pq.top()->getCol(), pq.top()->getRow(), p->getX(), p->getY());
	if (length <= thresholdLength)
	{
		Player* enemy = setPlayerLocation(pq.top()->getCol(), pq.top()->getRow());
		enemy->gotHit(calculateDamage(length));
		p->hit();
		cout << "PLAYER --> " << p->getId() << " HITED ENEMY --> " << enemy->getId() << "\n";

		if (!enemy->isAlive()) {
			maze[enemy->getY()][enemy->getX()].SetVal(SPACE);
			(teamAlive)--;
			cout << "PLAYER --> " << enemy->getId() << " DEAD!\n";
			cout << "____________________________GAME STATUS____________________________" << "\n";
			cout << "TEAM --> " << PLAYER_TEAM1 << " PLAYER ALIVE --> " << aliveTeam1 << "\n";
			cout << "TEAM --> " << PLAYER_TEAM2 << " PLAYER ALIVE --> " << aliveTeam2 << "\n";
			cout << "___________________________________________________________________" << "\n";
			if (teamAlive == 0)
			{
				cout << "GOOD JOB! --> TEAM " << p->getTeam() << " WIN!\n";
				startPlay = false;
			}
		}
		return true;
	}
	return false;
}

Player* setPlayerLocation(int x, int y)
{
	Player* p = new Player();
	for (int i = 0; i < playerSteps.size(); i++)
	{
		Player* temp = playerSteps.front();
		if (temp->getX() == x && temp->getY() == y)
			p = temp;
		playerSteps.pop();
		playerSteps.push(temp);
	}
	return p;
}

Point2D* enemysHit(int posTeam, int x1, int y1, int x2, int y2)
{
	Point2D* locationOfEnemy = new Point2D(-1, -1);
	x1 += x2;
	y1 += y2;
	while (maze[y1][x1].GetVal() == SPACE)
	{
		x1 += x2;
		y1 += y2;
	}
	if (maze[y1][x1].GetVal() == PLAYER_TEAM1 + PLAYER_TEAM2 - posTeam)
	{
		locationOfEnemy->setCol(x1);
		locationOfEnemy->setRow(y1);
	}
	return locationOfEnemy;
}

void myIteration()
{
	Player* p = playerSteps.front();
	bool(*trgOfEnemyNSameRoom)(Point2D * current, Point2D * source, map<string, string> args);
	bool(*trgOfEnemySameRoom)(Point2D * current, Point2D * src, map<string, string> args);
	double(*functionOfPath)(int x, int y);
	if (p->getMode() == ATTACK) {
		trgOfEnemyNSameRoom = attackTrgRoom;
		trgOfEnemySameRoom = shootPosRoom;
		functionOfPath = functionAttack; //Attack 
	}
	else if (p->getMode() == ATTACK__HEALTH_AMMU) {
		trgOfEnemySameRoom = shootPosRoomHealtAmm;
		trgOfEnemyNSameRoom = attackTrgRoomHealtAmmunition;
		functionOfPath = functionAHA; //Attack & Health & Ammou
	}
	else if (p->getMode() == ATTACK_AND_HEALTH) {
		trgOfEnemySameRoom = shootPosRoomHealth;
		trgOfEnemyNSameRoom = attackTrgRoomHealth;
		functionOfPath = functionAHA;//Attack & Health & Ammou
	}
	else if (p->getMode() == ATTACK_AND_AMMU) {
		trgOfEnemySameRoom = shootPosRoomAmmu;
		trgOfEnemyNSameRoom = attackTrgRoomAmmunition;
		functionOfPath = functionAHA;//Attack & Health & Ammou
	}
	else if (p->getMode() == NEED_AMMUNITION) {
		trgOfEnemySameRoom = needAmmunition;
		trgOfEnemyNSameRoom = needAmmunition;;
		functionOfPath = functionA;//Ammou
	}
	else {
		trgOfEnemySameRoom = survivorInSameRoom;
		trgOfEnemyNSameRoom = survivorNotInSameRoom;
		functionOfPath = functionH;
	}
	bool isEnemyInRoom = false;
	playerSteps.pop();
	if (!p->isAlive()) {
		return;
	}
	bool hit = false;
	isEnemyInRoom = isSameRoom(p);
	if (p->isAmmu() && isEnemyInRoom)
	{
		hit = hitEnemy(p, (p->getTeam() == PLAYER_TEAM1) ? aliveTeam2 : aliveTeam1, rand() % (30 - 5 + 1) + 5);
	}
	if (hit)
	{
		playerSteps.push(p);
		return;
	}
	map<string, string> trgParams;
	Point2D* trg;
	trgParams[MAP_KEY_ROOM_TRG] = "";
	if (isEnemyInRoom) {
		queue<Point2D*> e = enimiesInSameRoom(p);//enemies
		Point2D* ep; //enemy point
		trgParams[MAP_KEY_HIT_TRG] = "";
		string space;
		while (!e.empty())
		{
			ep = e.front();
			e.pop();
			space = getPlayerHitSpace(ep);
			trgParams[MAP_KEY_HIT_TRG] += space;
		}
		trg = UniformCost(new Point2D(p->getX(), p->getY()), trgParams, trgOfEnemySameRoom);
	}
	else {
		vector<Player*> e = getEnemies(p);
		for each (Player * enemy in e)
		{
			int trgRoomIndex = maze[enemy->getY()][enemy->getX()].GetRoomIndex();
			if (trgRoomIndex == -1) {
				trgParams[MAP_KEY_ROOM_TRG] += std::to_string(enemy->getLastRIndex()) + ",";
			}
			else {
				trgParams[MAP_KEY_ROOM_TRG] += std::to_string(trgRoomIndex) + ",";
			}
		}
		trg = UniformCost(new Point2D(p->getX(), p->getY()), trgParams, trgOfEnemyNSameRoom);
	}
	Point2D* nextStep = nextStepToTrg(new Point2D(p->getX(), p->getY()), new Point2D(trg), functionOfPath);
	if (nextStep->getRow() != -1)
	{
		if (maze[nextStep->getRow()][nextStep->getCol()].GetVal() == PLAYER_TEAM1 || maze[nextStep->getRow()][nextStep->getCol()].GetVal() == PLAYER_TEAM2)
		{
			vector<Point2D*> np = ableNeighborsWalk(new Point2D(p->getX(), p->getY()));
			if (np.size() > 0)
			{
				priority_queue <Node*, vector<Node*>, CompareNodesFVal> pq;
				for each (Point2D * step in np)
				{
					maze[step->getRow()][step->getCol()].setFVal(maze[step->getRow()][step->getCol()].getDng());
					pq.push(new Node(maze[step->getRow()][step->getCol()]));
				}
				nextStep = new Point2D(pq.top()->getPoint());
				while (!pq.empty())
				{
					maze[pq.top()->getPoint().getRow()][pq.top()->getPoint().getCol()].setFVal(0);
					pq.pop();
				}
			}
		}
		if (maze[nextStep->getRow()][nextStep->getCol()].GetVal() == AMMUNITION)
		{
			AddAmmunition();
			deleteAPoint(new Point2D(nextStep->getCol(), nextStep->getRow()));
			p->loadAmmu(rand() % 3 + 1);
		}
		if (maze[nextStep->getRow()][nextStep->getCol()].GetVal() == HEALTH)
		{
			AddHealth();
			deleteHPoint(new Point2D(nextStep->getCol(), nextStep->getRow()));
			p->loadHealth(rand() % 100 + 1);
		}
		if (maze[nextStep->getRow()][nextStep->getCol()].GetVal() == SPACE)
		{
			if (maze[p->getY()][p->getX()].GetRoomIndex() != -1)
			{
				p->setLastRIndex(maze[p->getY()][p->getX()].GetRoomIndex());
			}
			maze[p->getY()][p->getX()].SetVal(SPACE);
			p->move(nextStep->getCol(), nextStep->getRow());
			maze[nextStep->getRow()][nextStep->getCol()].SetVal(p->getTeam());
		}
	}
	playerSteps.push(p);
}

int calculateDamage(double length)
{
	return (30 / length) * (rand() % 10);
}

void idle()
{
	if (resume && pg != NULL)
	{
		pg->moveBullets(maze);
	}
	if (startPlay) {
		myIteration();
	}

	glutPostRedisplay();// calls indirectly to display
}

void menu(int choice)
{
	switch (choice)
	{
	case 1:
		resume = false;
		GenerateSecurityMap();
		glutDisplayFunc(displayMap);
		break;
	case 2:
		glutDisplayFunc(display);
		pg->Explode();
		resume = true;
		break;
	case 3:
		if (!defineTeams) {
			for (int i = 0; i < PLAYERS; i++)
			{
				AddPlayer(0, PLAYER_TEAM1);
				aliveTeam1++;
			}
			for (int i = 0; i < PLAYERS; i++)
			{
				AddPlayer(1, PLAYER_TEAM2);
				aliveTeam2++;
			}
			defineTeams = true;
		}
		break;
	case 4:
		if (defineTeams && !startPlay) {
			startPlay = true;
		}
		break;
	}
}

void mouse(int button, int state, int x, int y)
{
	double xx, yy;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		xx = 2 * x / (double)WIDTH - 1;
		yy = (2 * (HEIGHT - y) / (double)HEIGHT) - 1;
		//	pb = new Bullet(xx, yy);
		pg = new Grenade(xx, yy);
	}
}

void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition(400, 100);
	glutCreateWindow("First Example");

	glutDisplayFunc(display);
	glutIdleFunc(idle);

	glutMouseFunc(mouse);

	// menu
	glutCreateMenu(menu);
	glutAddMenuEntry("Generate map", 1);
	glutAddMenuEntry("Explode", 2);
	glutAddMenuEntry("Setup Teams", 3);
	glutAddMenuEntry("play", 4);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	init();

	glutMainLoop();
}