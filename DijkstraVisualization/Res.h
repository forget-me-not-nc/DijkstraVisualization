#pragma once

#define MENU_START 1000
#define RENEW_FIELD 1001

#include <Windows.h>
#include <GL\glew.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <GL\freeglut.h>
#include <iostream>
#include "MatrixElement.h"
#include <vector>
#include <queue>

const unsigned int wHEIGHT = 800;
const unsigned int wWIDTH = 800;
const unsigned int orthoHEIGHT = wHEIGHT - 750;
const unsigned int orthoWIDTH = wWIDTH - 750;
const unsigned int yStep = wHEIGHT / orthoHEIGHT;
const unsigned int xStep = wWIDTH / orthoWIDTH;

#define V (orthoHEIGHT * orthoWIDTH)

bool startFlag = false;
bool isGraphCreated = false;
bool firstFlag = false;
std::vector<int> path(V, -1);
std::queue<MatrixElement> queue{};

MatrixElement fieldMatrix[orthoWIDTH][orthoHEIGHT]{};
int graph[V][V]{0};
MatrixElement* startPoint;
MatrixElement* endPoint;
MatrixElement* currentPoint;

MatrixElement* findOnField(int cellIndex)
{
    for (unsigned int y = 0; y < orthoHEIGHT; y++)
    {
        for (unsigned int x = 0; x < orthoWIDTH; x++)
        {
            if (fieldMatrix[x][y].cellIndex == cellIndex)
            {
                return &fieldMatrix[x][y];
            }
        }
    }
}

void renewField()
{
    for (unsigned int y = 0; y < orthoHEIGHT; y++)
    {
        for (unsigned int x = 0; x < orthoWIDTH; x++)
        {
            fieldMatrix[x][y] = MatrixElement(x, y, STATES::FREE, y * orthoWIDTH + x);
        }
    }

    startFlag = false;
    isGraphCreated = false;

    path.clear();
    path.insert(path.begin(), V, -1);

    startPoint = nullptr;
    endPoint = nullptr;
}

void createGraph()
{
    int x = 0;
    int y = 0;

    for (unsigned int i = 0; i < V; i++)
    {
        if (x - 1 >= 0) graph[i][fieldMatrix[x - 1][y].cellIndex] = static_cast<int>(fieldMatrix[x - 1][y].state);
        if (x + 1 < orthoWIDTH) graph[i][fieldMatrix[x + 1][y].cellIndex] = static_cast<int>(fieldMatrix[x + 1][y].state);
        if (y - 1 >= 0) graph[i][fieldMatrix[x][y - 1].cellIndex] = static_cast<int>(fieldMatrix[x][y - 1].state);
        if (y + 1 < orthoHEIGHT) graph[i][fieldMatrix[x][y + 1].cellIndex] = static_cast<int>(fieldMatrix[x][y + 1].state);
        if (x - 1 >= 0 && y + 1 < orthoHEIGHT) graph[i][fieldMatrix[x - 1][y + 1].cellIndex] = static_cast<int>(fieldMatrix[x - 1][y + 1].state);
        if (x - 1 >= 0 && y - 1 >= 0) graph[i][fieldMatrix[x - 1][y - 1].cellIndex] = static_cast<int>(fieldMatrix[x - 1][y - 1].state);
        if (x + 1 < orthoWIDTH && y + 1 < orthoHEIGHT) graph[i][fieldMatrix[x + 1][y + 1].cellIndex] = static_cast<int>(fieldMatrix[x + 1][y + 1].state);
        if (x + 1 < orthoWIDTH && y - 1 >= 0) graph[i][fieldMatrix[x + 1][y - 1].cellIndex] = static_cast<int>(fieldMatrix[x + 1][y - 1].state);

        if (x + 1 < orthoWIDTH && y + 1 < orthoHEIGHT)
        {
            if (fieldMatrix[x + 1][y].state == STATES::BLOCKED && fieldMatrix[x][y + 1].state == STATES::BLOCKED)
            {
                graph[i][fieldMatrix[x + 1][y + 1].cellIndex] = 0;
            }        
        }

        if (x + 1 < orthoWIDTH && y - 1 < orthoHEIGHT)
        {
            if (fieldMatrix[x + 1][y].state == STATES::BLOCKED && fieldMatrix[x][y - 1].state == STATES::BLOCKED)
            {
                graph[i][fieldMatrix[x + 1][y - 1].cellIndex] = 0;
            }
        }

        if (x - 1 < orthoWIDTH && y + 1 < orthoHEIGHT)
        {
            if (fieldMatrix[x - 1][y].state == STATES::BLOCKED && fieldMatrix[x][y + 1].state == STATES::BLOCKED)
            {
                graph[i][fieldMatrix[x - 1][y + 1].cellIndex] = 0;
            }
        }

        if (x - 1 < orthoWIDTH && y - 1 < orthoHEIGHT)
        {
            if (fieldMatrix[x - 1][y].state == STATES::BLOCKED && fieldMatrix[x][y - 1].state == STATES::BLOCKED)
            {
                graph[i][fieldMatrix[x - 1][y - 1].cellIndex] = 0;
            }
        }

        x++;

        if (x == orthoWIDTH)
        {
            y++;
        }
        
        x %= orthoWIDTH;
    }
}

int minDistance(int dist[], bool sptSet[])
{

    int min = INT_MAX, min_index;


    for (int v = 0; v < V; v++)
    {
        if (sptSet[v] == false && dist[v] <= min)
        {
            min = dist[v], min_index = v;
        }
    }

    return min_index;
}

void dijkstra(int src)
{
    int dist[V];

    bool sptSet[V];

    for (int i = 0; i < V; i++)
    {
        dist[i] = INT_MAX;
        sptSet[i] = false;
    }

    dist[src] = 0;

    for (int count = 0; count < V - 1; count++)
    {
        int u = minDistance(dist, sptSet);

        sptSet[u] = true;

        for (int v = 0; v < V; v++)
        {
            if (!sptSet[v] && graph[u][v] && dist[u] != INT_MAX && dist[u] + graph[u][v] < dist[v])
            {
                dist[v] = dist[u] + graph[u][v];

                path[v] = u;
            }
        }
    }
}

void calcPath(int index)
{
    if (path[index] == -1) return;

    calcPath(path[index]);

    queue.push(*findOnField(path[index]));
}

void drawPath()
{
    queue.pop();

    glBegin(GL_POINTS);

    glColor3ub(6, 0, 176);

    while (!queue.empty())
    {
        currentPoint = &queue.front();

        glVertex2i(currentPoint->x, currentPoint->y);

        queue.pop();
    }
    
    glEnd();
}