// Snake.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <windows.h>
#include <time.h>

using namespace std;

const int WIDTH = 70;
const int HEIGHT =50;
pair<int, int> dir[4] = { make_pair(0,1),make_pair(0,-1),make_pair(1,0),make_pair(-1,0) };
static int path[WIDTH][HEIGHT] = { -1 };


bool move(vector<pair<int, int>>&, pair<int, int>, pair<int, int>);
pair<int, int> getDirection(vector<pair<int, int>>, pair<int,int>);
void draw(vector<pair<int, int>>, pair<int, int>, sf::RenderWindow&);
pair<int, int> newCherry(vector<pair<int, int>>);
bool isDead(vector<pair<int, int>>);


int main(){
	sf::RenderWindow window(sf::VideoMode(WIDTH*20, HEIGHT*20), "SFML works!");
	vector<pair<int, int>> snake;
	for (int i = 5; i < 11; i++) {
		snake.push_back(pair<int, int>(10, i));
	}
	pair<int, int> cherryLoc = newCherry(snake);

	int time = 0;
	int moveCount= 0;
	while (window.isOpen()){
		sf::Event event;
		while (window.pollEvent(event)){
			if (event.type == sf::Event::Closed)
				window.close();
		}
		if (!isDead(snake)) {
			clock_t start=clock();
			if (move(snake, getDirection(snake, cherryLoc), cherryLoc))
			{
				cherryLoc = newCherry(snake);
				cout << snake.size() << "   " << time*1000/moveCount<<endl;
				time = 0;
				moveCount = 0;

			}
			time += clock() - start;
			moveCount++;
			if(false)
				draw(snake, cherryLoc, window);
		}
	}

	return 0;
}


bool move(vector<pair<int, int>> &snake, pair<int, int> move, pair<int, int> cherryLoc) {
	snake.push_back(make_pair(snake.back().first + move.first, snake.back().second + move.second));
	if (snake.back() == cherryLoc)
		return true;
	snake.erase(snake.begin());
	return false;
}


void initializePath(int path[WIDTH][HEIGHT]) {
	int x = 0,y=0;
	for (int i = 0; i < WIDTH*HEIGHT; i++)
	{
		path[WIDTH][HEIGHT] = i;
		if (y == 0) {
			if (x == 0)
				y++;
			else
				x--;
		}
		else if (x % 2 == 0) {
			if (y == HEIGHT - 1)
				x++;
			else
				y++;
		}
		else if (y == 1)
			if (x == WIDTH - 1)
				y--;
			else
				x++;
		else
			y--;
		path[x][y] = i;
	}

}


int getVal(int x, int y, int path[WIDTH][HEIGHT]){
	if (x >= 0 && x < WIDTH&&y >= 0 && y < HEIGHT)
		return path[x][y];
	return -2;
}


pair<int, int> findDir(pair<int, int> pos, int nextPos) {
	for (int i = 0; i < 4; i++){
		pair<int,int> toCheck = make_pair(pos.first+dir[i].first, pos.second +dir[i].second);
		if (toCheck.first >= 0 && toCheck.first < WIDTH && toCheck.second >= 0 && toCheck.second < HEIGHT)
			if (path[toCheck.first][toCheck.second] == nextPos)
				return dir[i];
	}
	return make_pair(-1, -1);
}


bool isBetween(int min, int mid, int max){
	int i = 0;
	if (min < mid)
		++i;
	if (mid < max)
		++i;
	if (max < min)
		++i;
	return i == 2;
}


bool absModOne(int x) {
	return x == 1 || x == -1 || x == WIDTH * HEIGHT - 1 || -1 * x == WIDTH * HEIGHT - 1;
}


bool swapLoop(int stX, int stY, int endX, int endY, int cherVal, int snakeEnd)
{
	int curX = stX;
	int curY = stY;
	int nextX=-5, nextY=-5;
	bool changed = false;
	for (int i = path[stX][stY]; i != path[endX][endY];){
		int nextI = (i + 1) % (WIDTH*HEIGHT);
		if (path[curX][curY] != i)
			throw 5;
		for (int j = 0; j < 4; j++) {
			if (curX == 0 && curY == 0)
				int ik = 7;
			if (getVal(curX + dir[j].first, curY + dir[j].second, path) == nextI) {
				nextX = curX + dir[j].first;
				nextY = curY + dir[j].second;
			}
		}
		if ((nextX == curX) == (nextY == curY))
			throw 6;
		if (nextX == -5)
			throw 9;
		for (int j = 0; j < 4; j++)
			if (isBetween(cherVal, getVal(curX + dir[j].first, curY + dir[j].second, path), snakeEnd))
				if (absModOne(getVal(curX + dir[j].first, curY + dir[j].second, path) - getVal(nextX + dir[j].first, nextY + dir[j].second, path)))
					if (!absModOne(getVal(curX + dir[j].first, curY + dir[j].second, path) - path[curX][curY])) {
						if (getVal(curX + dir[j].first, curY + dir[j].second, path) - getVal(nextX + dir[j].first, nextY + dir[j].second, path) == -1)
							throw 7;
						changed = true;
						/*The path can be divided into 4 parts: Snake, Cherry, out, and in (where out and in form the loop).
						The current order is snake, out, back, cherry.
						It will be swapped to snake, cherry, back, out.
						Snake will be held constant.*/
						int cherryMod = path[stX][stY] - path[endX][endY] - 1;
						int cherryMin = path[endX][endY] + 1;
						int cherryMax = getVal(nextX + dir[j].first, nextY + dir[j].second, path);
						int outMod = cherryMax - path[curX][curY];
						int outMin = path[stX][stY];
						int outMax = path[curX][curY];
						int backMod = outMin + outMod - cherryMin;
						int backMin = (outMax + 1) % (WIDTH*HEIGHT);
						int backMax = (cherryMin - 1) % (WIDTH*HEIGHT);
						for (int k = 0; k < WIDTH; k++)
							for (int l = 0; l < HEIGHT; l++) {
								if (isBetween(cherryMin - 1, path[k][l], cherryMax + 1)) {
									path[k][l] += cherryMod+ (WIDTH*HEIGHT);
									path[k][l] %= (WIDTH*HEIGHT);
								}
								else if (isBetween(outMin - 1, path[k][l], outMax + 1)) {
									path[k][l] += outMod + (WIDTH*HEIGHT);
									path[k][l] %= (WIDTH*HEIGHT);
								}
								else if (isBetween(backMin - 1, path[k][l], backMax + 1)) {
									path[k][l] += backMod + (WIDTH*HEIGHT);
									path[k][l] %= (WIDTH*HEIGHT);
								}
							}
						j = 4;
						nextI = path[endX][endY];

					}
		curY = nextY;
		curX = nextX;
		i = nextI;
	}
	return changed;
}


bool isIn(int x, int y, vector<pair<int, int>> &snake)
{
	for (int i = 0; i < snake.size(); i++)
		if (snake.at(i).first == x && snake.at(i).second == y)
			return true;
	return false;
}


bool blocked(int x, int y, vector<pair<int, int>> &snake)
{
	if (x == 0 || isIn(x - 1, y, snake))
		if (x == WIDTH - 1 || isIn(x + 1, y, snake))
			return true;
	if (y == 0 || isIn(x, y - 1, snake))
		if (y == HEIGHT - 1 || isIn(x, y + 1, snake))
			return true;
	return false;
}



pair<int,int> getDirection(vector<pair<int, int>> snake, pair<int,int> cherry) {
	/*clock_t time_end;
	time_end = clock() + 50 * CLOCKS_PER_SEC / 1000;
	while (clock() < time_end)
	{
	}*/
	static int  moves = 0;
	moves++;

	if (path[0][0] == -1)
		initializePath(path);
	int cherryVal = path[cherry.first][cherry.second];

	int curX = snake.back().first;
	int curY = snake.back().second;
	int nextX =-5, nextY = -5;
	clock_t start = clock();
	for (int i = path[curX][curY]; i != cherryVal&&clock()-start<2;)
	{
		int nextI = (i + 1) % (WIDTH*HEIGHT);
		if (path[curX][curY] != i)
			throw 5;
		for (int j = 0; j < 4; j++)
			if (getVal(curX + dir[j].first, curY + dir[j].second, path) == nextI) {
				nextX = curX + dir[j].first;
				nextY = curY + dir[j].second;
			}
		if ((nextX == curX) == (nextY == curY))
			cout << path[0][0];
		if (nextX == -5)
			throw 9;
		for (int j = 0; j < 4; j++)
			if (isBetween(i + 1, getVal(curX + dir[j].first, curY + dir[j].second, path), cherryVal + 1))
				if (getVal(curX + dir[j].first, curY + dir[j].second, path) == getVal(nextX + dir[j].first, nextY + dir[j].second, path) + 1)
					if (swapLoop(nextX, nextY, nextX + dir[j].first, nextY + dir[j].second, cherryVal, path[snake.at(0).first][snake.at(0).second])) {
						nextI = i;
						nextY = curY;
						nextX = curX;
						j = 4;
						cherryVal = path[cherry.first][cherry.second];
						start = clock();
					}
		i = nextI;
		curX = nextX;
		curY = nextY;
	}
	return findDir(snake.back(), (path[snake.back().first][snake.back().second] + 1)%(WIDTH*HEIGHT));
}


bool isDead(vector<pair<int, int>> snake) {
	if (snake.back().first < 0 || snake.back().first >= WIDTH || snake.back().second < 0 || snake.back().second >= HEIGHT)
		return true;
	for (int i = 0; i < snake.size() - 1; i++)
		if (snake.back() == snake.at(i))
			return true;
	return false;
}


void draw(vector<pair<int, int>> snake, pair<int, int> cherry, sf::RenderWindow& window)
{
	window.clear();

	sf::RectangleShape vert(sf::Vector2f(16, 36));
	vert.setFillColor(sf::Color::Green);

	sf::RectangleShape horiz(sf::Vector2f(36, 16));
	horiz.setFillColor(sf::Color::Green);

	sf::RectangleShape cher(sf::Vector2f(16, 16));
	cher.setFillColor(sf::Color::Red);

	for (int i = 0; i < snake.size() - 1; i++){
		if (snake.at(i).first == snake.at(i + 1).first){
			vert.setPosition(snake.at(i).first * 20 + 2,min(snake.at(i).second,snake.at(i+1).second)*20+2);
			window.draw(vert);
		}
		else{
			horiz.setPosition(min(snake.at(i).first, snake.at(i + 1).first) * 20 + 2, snake.at(i).second * 20 + 2);
			window.draw(horiz);
		}
	}

	cher.setPosition(cherry.first * 20 + 2, cherry.second * 20 + 2);
	window.draw(cher);

	sf::RectangleShape vertline(sf::Vector2f(2, 22));
	vertline.setFillColor(sf::Color::Blue);

	sf::RectangleShape horizline(sf::Vector2f(22, 2));
	horizline.setFillColor(sf::Color::Blue);

	for (int i = 0; i < WIDTH - 1; i++)
		for (int j = 0; j < HEIGHT; j++)
			if (absModOne(path[i][j] - path[i + 1][j])) {
				horizline.setPosition(i * 20 + 9, j * 20 + 9);
				window.draw(horizline);
			}
	for (int i = 0; i < WIDTH; i++)
		for (int j = 0; j < HEIGHT-1; j++)
			if (absModOne(path[i][j] - path[i][j+1])) {
				vertline.setPosition(i * 20 + 9, j * 20 + 9);
				window.draw(vertline);
			}


	window.display();
}


pair<int, int> newCherry(vector<pair<int, int>> snake){
	bool overlap = true;
	pair<int, int> newPos;
	while (overlap){
		newPos = make_pair(rand() % WIDTH, rand() % HEIGHT);
		overlap = false;
		for (int i = 0; i < snake.size(); i++)
			overlap = overlap || snake.at(i) == newPos;
	}
	return newPos;
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
