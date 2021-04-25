#include "glut.h"
#include <time.h>
#include <vector>
#include "Cell.h"
#include <iostream>

using namespace std;

const int MSZ = 100;

const int SPACE = 0;
const int WALL = 1;
const int START = 2;
const int TARGET = 3;
const int BLACK = 4;
const int GRAY = 5;
const int PATH = 6;

int maze[MSZ][MSZ] = { 0 }; // 0 is SPACE
bool bfs_is_on = false;
bool dfs_is_on = false;

vector <Cell*> grays;


void InitMaze();

void init()
{
	glClearColor(0.8, 0.7, 0.5, 0);// color of window background
	glOrtho(-1, 1, -1, 1, 1, -1);

	srand(time(0));

	InitMaze();
}

void InitMaze()
{
	int i, j;

	// set margins
	for (i = 0; i < MSZ; i++)
	{
		maze[0][i] = WALL;
		maze[MSZ - 1][i] = WALL;
		maze[i][0] = WALL;
		maze[i][MSZ - 1] = WALL;
	}

	for (i = 1; i < MSZ - 1; i++)
		for (j = 1; j < MSZ - 1; j++)
		{
			if (i % 2 == 1)  // this is mostly SPACE
			{
				if (rand() % 100 < 10) // 10% WALLs
					maze[i][j] = WALL;
				else
					maze[i][j] = SPACE;
			}
			else // this is mostly WALL
			{
				if (rand() % 100 < 40) // 40% SPACEs
					maze[i][j] = SPACE;
				else
					maze[i][j] = WALL;
			}
		}

	maze[MSZ / 2][MSZ / 2] = START;
	maze[rand() % MSZ][rand() % MSZ] = TARGET;

	Cell* pc = new Cell(MSZ / 2, MSZ / 2, nullptr);
	grays.push_back(pc); // add first cell to grays
}

void DrawMaze()
{
	int i, j;
	double sx, sy; // cell size
	double x, y;

	sx = 2.0 / MSZ;
	sy = 2.0 / MSZ;

	for (i = 0; i < MSZ; i++)
		for (j = 0; j < MSZ; j++)
		{
			switch (maze[i][j])
			{
			case SPACE:
				glColor3d(1, 1, 1);   // white
				break;
			case WALL:
				glColor3d(0.7, 0.4, 0);   // dark-orange
				break;
			case START:
				glColor3d(0, 1, 1);   // cyan
				break;
			case TARGET:
				glColor3d(1, 0, 0);   // red
				break;
			case GRAY:
				glColor3d(1, 1, 0);   // yellow
				break;
			case BLACK:
				glColor3d(0.8, 1, 0.8);   // green
				break;
			case PATH:
				glColor3d(0.8, 0.4, 1);   // magenta
				break;
			}
			// draw square maze[i][j]
			x = 2 * (j / (double)MSZ) - 1;
			y = 2 * (i / (double)MSZ) - 1;

			glBegin(GL_POLYGON);  // fill up
			glVertex2d(x, y);
			glVertex2d(x, y + sy);
			glVertex2d(x + sx, y + sy);
			glVertex2d(x + sx, y);
			glEnd();
			glColor3d(0, 0, 0);
			/*			glBegin(GL_LINE_LOOP);  // boundaries
						glVertex2d(x, y);
						glVertex2d(x, y + sy);
						glVertex2d(x + sx, y + sy);
						glVertex2d(x + sx, y);
						glEnd();
			*/
		}
}

void RestorePath(Cell* current)
{
	while (current->GetParent() != nullptr)
	{
		maze[current->GetRow()][current->GetCol()] = PATH;
		current = current->GetParent();
	}
}

void CheckNeighbor(Cell* current, int row, int col)
{
	Cell* next;
	if (maze[row][col] == TARGET)
	{
		bfs_is_on = false;
		RestorePath(current);
	}
	else
		if (maze[row][col] == SPACE) // paint it gray
		{
			maze[row][col] = GRAY;
			next = new Cell(row, col, current);
			grays.push_back(next);
		}
}

void BFSIteration()
{
	Cell* current;
	int curr_row, curr_col;


	if (grays.empty()) // no more grays....no solution
	{
		cout << "no more grays....no solution\n";
		bfs_is_on = false;
	}
	else
	{
		// take the first element, paint it black and add to grays its white neighbors
		current = grays.front();
		grays.erase(grays.begin()); // remove the front element
		curr_row = current->GetRow();
		curr_col = current->GetCol();
		// paint it black
		if (maze[curr_row][curr_col] != START)
			maze[curr_row][curr_col] = BLACK;
		// look for white neighbors
		// up
		CheckNeighbor(current, curr_row + 1, curr_col);
		if (bfs_is_on)		// down
			CheckNeighbor(current, curr_row - 1, curr_col);
		if (bfs_is_on)		// left
			CheckNeighbor(current, curr_row, curr_col - 1);
		if (bfs_is_on)		// right
			CheckNeighbor(current, curr_row, curr_col + 1);
	}
}
void DFSIteration()
{
	//to be cotinued...
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer

	DrawMaze();

	glutSwapBuffers(); // show all
}

void idle()
{

	if (bfs_is_on)
		BFSIteration();

	glutPostRedisplay(); // indirect call to display
}
void menu(int choice)
{
	switch (choice)
	{
	case 1: // BFS
		bfs_is_on = true;
		break;
	case 2: // DFS

		break;
	}
}
void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("First Example");

	glutDisplayFunc(display);
	glutIdleFunc(idle);
	// menu
	glutCreateMenu(menu);
	glutAddMenuEntry("BFS", 1);
	glutAddMenuEntry("DFS", 2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);


	init();

	glutMainLoop();
}