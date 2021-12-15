#include "Res.h"

//--------------------------------

void changeViewPort(int w, int h);
void render();
void update();
void customInit();
int BuildMenu();
void SelectFromMenu(int cmd);
void MouseMovement(int x, int y);
void KeyHandler(unsigned char key, int x, int y);

//--------------------------------

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	glutInitWindowSize(wWIDTH, wHEIGHT);

	glutCreateWindow("Dijkstra");

	customInit();

	glutReshapeFunc(changeViewPort);
	glutDisplayFunc(render);
	glutIdleFunc(update);
	BuildMenu();
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutKeyboardFunc(KeyHandler);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	GLenum err = glewInit();

	if (GLEW_OK != err)
	{
		fprintf(stderr, "GLEW error");
		return 1;
	}

	glutMainLoop();

	delete endPoint;
	delete startPoint;
	delete currentPoint;

	return 0;
}

void KeyHandler(unsigned char key, int x, int y)
{
	if (startFlag) return;

	y /= yStep;
	x /= xStep;

	if (x < 0 || x > orthoWIDTH || y < 0 || y > orthoHEIGHT) return;
	
	if (fieldMatrix[x][orthoHEIGHT - (unsigned int)y].state != STATES::FREE) return;

	switch (key)
	{
		case '1': 
		{
			startPoint = &fieldMatrix[x][orthoHEIGHT - (unsigned int)y];

			break;
		}

		case '2':
		{
			endPoint = &fieldMatrix[x][orthoHEIGHT - (unsigned int)y];

			break;
		}

		case '3':
		{
			if (x < 0 || x > orthoWIDTH || y < 0 || y > orthoHEIGHT) return;

			if(fieldMatrix[x][orthoHEIGHT - (unsigned int)y].state == STATES::BLOCKED) return;

			fieldMatrix[x][orthoHEIGHT - (unsigned int)y].state = STATES::BLOCKED;

			break;
		}

		case '4':
		{
			if (x < 0 || x > orthoWIDTH || y < 0 || y > orthoHEIGHT) return;

			if (fieldMatrix[x][orthoHEIGHT - (unsigned int)y].state == STATES::SWAMP) return;

			fieldMatrix[x][orthoHEIGHT - (unsigned int)y].state = STATES::SWAMP;
		}

		default: break;
	}
}

void SelectFromMenu(int cmd)
{
	switch (cmd)
	{
		case MENU_START:
		{
			startFlag = true;

			break;
		}

		case RENEW_FIELD:
		{
			renewField();

			break;
		}

		default: break;
	}

	glutPostRedisplay();
}

int BuildMenu()
{
	int menu = glutCreateMenu(SelectFromMenu);

	glutAddMenuEntry("Start", MENU_START);
	glutAddMenuEntry("Reset", RENEW_FIELD);

	return menu;
}

void changeViewPort(int w, int h)
{
	glutReshapeWindow(wWIDTH, wHEIGHT);

	glutPostRedisplay();
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBegin(GL_POINTS);

	for (unsigned int x = 0; x < orthoWIDTH; x++)
	{
		for (unsigned int y = 0; y < orthoHEIGHT; y++)
		{
			switch (fieldMatrix[x][y].state)
			{
				case STATES::BLOCKED:
				{
					glColor3ub(255, 0, 0);

					break;
				}

				case STATES::SWAMP:
				{
					glColor3ub(100, 222, 173);

					break;
				}

				default:
				{
					glColor3ub(0, 0, 0);

					break;
				}
			}

			glVertex2i(fieldMatrix[x][y].x, fieldMatrix[x][y].y);
		}
	}

	if (endPoint != nullptr)
	{
		glColor3ub(0, 255, 42);

		glVertex2i(endPoint->x, endPoint->y);
	}

	if (startPoint != nullptr)
	{
		glColor3ub(255, 255, 255);

		glVertex2i(startPoint->x, startPoint->y);
	}

	glEnd();

	if (startFlag)
	{
		if (!endPoint)
		{
			MessageBox(NULL, L"Set End Point", L"ERROR", MB_ICONERROR);

			startFlag = false;
		}
		if (!startPoint)
		{
			MessageBox(NULL, L"Set Start Point", L"ERROR", MB_ICONERROR);

			startFlag = false;
		}

		if (startFlag)
		{
			if (!isGraphCreated)
			{
				createGraph();

				dijkstra(startPoint->cellIndex);

				isGraphCreated = true;
			}

			calcPath(endPoint->cellIndex);

			if (queue.empty())
			{
				MessageBox(NULL, L"There is no way to this point.\n Reset field.", L"ERROR", MB_ICONEXCLAMATION);

				startFlag = false;
			}
			else drawPath();
		}
	}

	glutSwapBuffers();
}

void update()
{
	glutPostRedisplay();
}

void customInit()
{
	renewField();

	glPointSize(15.5f);

	gluOrtho2D(0.0, orthoWIDTH, 0.0, orthoHEIGHT);
}