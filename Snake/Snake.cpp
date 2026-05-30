// Snake.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <conio.h>
#include <thread>
bool GameOver;
const int width = 20;
const int height = 20;
int x, y, fx, fy, score;
enum Dir
{
	Stop, Up, Down, Left, Right
};
char tailChar[]
{
	' ',
	'R',
	'E',
	'G',
	'G',
	'I',
	'N',
};
int tailsx[100];
int tailsy[100];
int tailcount = 1;
Dir direction;
using namespace std;
void Setup() {
	GameOver = false;
	direction = Dir::Stop;
	x = width / 2;
	y = height / 2;
	fx = rand() % width;
	fy = rand() % height;
	score = 0;
}
void Draw() {
	system("cls");
	for (int i = 0; i < width; i++) {
		cout << "#";
	}
	cout << endl;

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {

			if (j == 0)
				cout << "#";
			else if (j == x && i == y)
				cout << "0";
			else if (fx == j && fy == i)
				cout << "F";
			else if (j == width - 1)
				cout << "#";
			else {
				bool draw = false;
				for (int t = 1; t < tailcount; t++) {
					if (tailsx[t] == j && tailsy[t] == i) {
						if(t < sizeof(tailChar))
							cout << tailChar[t];
						else
							cout << "0";
						draw = true;
					}
				}
				if (!draw)
					cout << " ";
			}
		}
		cout << endl;
	}


	for (int i = 0; i < width; i++) {
		cout << "#";
	}
	cout << endl;
}
void Input() {
	if (_kbhit()) {
		switch (_getch()) {
		case 'a':
			direction = Left;
			break;
		case 'd':
			direction = Right;
			break;
		case 'w':
			direction = Up;
			break;
		case 's':
			direction = Down;
			break;
		case'x':
			GameOver = true;
			break;
		}
	}
}
void Logic() {
	int pervx = tailsx[0];
	int pervy = tailsy[0];
	int pervx2, pervy2;
	tailsx[0] = x;
	tailsy[0] = y;
	for (int i = 0; i < tailcount; i++) {
		pervx2 = tailsx[i];
		pervy2 = tailsy[i];
		tailsx[i] = pervx;
		tailsy[i] = pervy;
		pervx = pervx2;
		pervy = pervy2;
	}
	switch (direction)
	{
	case Up:
		y--;
		break;
	case Down:
		y++;
		break;
	case Left:
		x--;
		break;
	case Right:
		x++;
		break;
	default:
		break;
	}
	for (int t = 0; t < tailcount; t++) {
		if (tailsx[t] == x && tailsy[t] == y) {
			Draw();
			GameOver = true;
		}
	}
	if (x > width || x < 0 || y > height || y < 0)
		GameOver = true;
	if (fx == x && fy == y) {
		score += 10;
		fx = rand() % width;
		fy = rand() % height;
		tailcount++;
	}
	cout << "Score : " << score << endl;
}

int main()
{
	Setup();
	while (!GameOver) {
		Draw();
		Input();
		Logic();

		this_thread::sleep_for(chrono::milliseconds(100));
	}
	std::cout << "Hello World!\n";
}