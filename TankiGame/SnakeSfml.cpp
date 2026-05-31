// TankiGame.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <thread>
#include <chrono>
#include <filesystem>
#include <sstream>

struct SnakePlayer
{
	int score = 0;
	double x, y;
	double tailsx[100];
	double tailsy[100];
	int tailcount = 1;
};
SnakePlayer player;
int score = 0;
double x, y, fx, fy;
enum Dir
{
	Stop,
	Up,
	Down,
	Left,
	Right,
};
Dir Direction;
bool GameOver = false;

double tailsx[100];
double tailsy[100];
int tailcount = 1;

sf::TcpSocket socketPlayer;
bool Send = false;

using namespace std;
using namespace sf;
void Draws(RenderWindow* window, Font& font) {
	Vector2u windowsize = window->getSize();

	RectangleShape wall;
	RectangleShape wall1;
	RectangleShape wall2;
	RectangleShape wall3;

	wall.setFillColor(Color(155, 155, 255));
	wall1.setFillColor(Color(155, 155, 255));
	wall2.setFillColor(Color(155, 155, 255));
	wall3.setFillColor(Color(155, 155, 255));

	wall.setSize(Vector2f(10, windowsize.y));
	wall1.setSize(Vector2f(10, windowsize.y));
	wall2.setSize(Vector2f(windowsize.x, 10));
	wall3.setSize(Vector2f(windowsize.x, 10));

	wall.setPosition(Vector2f(0, 0));
	wall1.setPosition(Vector2f(windowsize.x - 10, 0));
	wall2.setPosition(Vector2f(0, 0));
	wall3.setPosition(Vector2f(0, windowsize.y - 10));

	window->draw(wall);
	window->draw(wall1);
	window->draw(wall2);
	window->draw(wall3);

	//Walls End

	CircleShape HSnake;
	HSnake.setFillColor(Color(0, 255, 0));
	HSnake.setPosition(Vector2f(x, y));
	HSnake.setRadius(10);

	window->draw(HSnake);

	for (int t = 0; t < tailcount; t++) {
		CircleShape tail;
		tail.setFillColor(Color(0, 255, 0));
		tail.setPosition(Vector2f(tailsx[t], tailsy[t]));
		tail.setRadius(8);
		window->draw(tail);
	}
	//End Snake

	CircleShape Furit;
	Furit.setFillColor(Color(255, 0, 0));
	Furit.setPosition(Vector2f(fx, fy));
	Furit.setRadius(8);

	window->draw(Furit);

	//score
	Text scoretxt(font);
	scoretxt.setString("Your Score :" + to_string(score));
	scoretxt.setFillColor(Color(0, 255, 0));
	scoretxt.setPosition(Vector2f(10, 10));

	window->draw(scoretxt);


	//End Furit
	if (GameOver) {
		sf::Text txt(font);
		txt.setString("Game Over");
		txt.setFillColor(Color(255, 0, 0));
		txt.setPosition(Vector2f(windowsize.x / 2, windowsize.y / 2));
		window->draw(txt);
	}
}
void Inputs(RenderWindow* window) {
	while (const optional event = window->pollEvent())
	{
		if (event->is < Event::Closed>()) {
			GameOver = true;
		}
		else if (const auto* keyp = event->getIf<Event::KeyPressed>()) {
			if (keyp->scancode == Keyboard::Scancode::Escape || keyp->scancode == Keyboard::Scancode::X)
			{
				GameOver = true;
			}
			else if (keyp->scancode == Keyboard::Scancode::W) {
				Direction = Dir::Up;
			}
			else if (keyp->scancode == Keyboard::Scancode::S) {
				Direction = Dir::Down;
			}
			else if (keyp->scancode == Keyboard::Scancode::A) {
				Direction = Dir::Left;
			}
			else if (keyp->scancode == Keyboard::Scancode::D) {
				Direction = Dir::Right;
			}
		}
	}
}
void GameLogic(RenderWindow* window) {
	double pervx = tailsx[0];
	double pervy = tailsy[0];
	double pervx2, pervy2;
	tailsx[0] = x;
	tailsy[0] = y;
	for (int t = 1; t < tailcount; t++) {
		pervx2 = tailsx[t];
		pervy2 = tailsy[t];
		tailsx[t] = pervx;
		tailsy[t] = pervy;
		pervx = pervx2;
		pervy = pervy2;
	}
	//Direction
	switch (Direction)
	{
	case Up:
		y -= 10;
		break;
	case Down:
		y += 10;
		break;
	case Left:
		x -= 10;
		break;
	case Right:
		x += 10;
		break;
	}
	if (tailcount > 1) {
		for (int t = 0; t < tailcount; t++) {
			double dis = sqrt((pow(x - tailsx[t], 2) + pow(y - tailsy[t], 2)));
			if (dis < 10) {
				GameOver = true;
				window->close();
			}
		}
	}
	//Score
	double dis = sqrt(pow((x - fx), 2) + pow((y - fy), 2));
	if (dis < 10) {
		score += 10;
		fx = rand() % window->getSize().x;
		fy = rand() % window->getSize().y;
		tailcount++;
	}
	//WallCheck
	if (x < 10 || x > window->getSize().x || y < 10 || y > window->getSize().y) {
		GameOver = true;
	}
}
void SecendPlayer(RenderWindow* window,Font& font) {
	if (Send)
	{
		sf::Packet sendpacket;
		sendpacket << score << x << y << tailcount;
		for (int t = 0; t < tailcount; t++) {
			sendpacket << tailsx[t];
			sendpacket << tailsy[t];
		}
		if (socketPlayer.send(sendpacket) != Socket::Status::Done) {
			//error
		}
		sf::Packet recivepacket;
		if (socketPlayer.receive(recivepacket) == Socket::Status::Done) {
			recivepacket >> player.score >> player.x >> player.y >> player.tailcount;
			for (int t = 0; t < player.tailcount; t++) {
				recivepacket >> player.tailsx[t];
				recivepacket >> player.tailsy[t];
			}
			//head
			CircleShape shape;
			shape.setRadius(10);
			shape.setFillColor(Color(255, 255, 0));
			shape.setPosition(Vector2f(player.x, player.y));
			window->draw(shape);
			//tail
			shape.setRadius(8);
			for (int t = 0; t < player.tailcount; t++) {
				shape.setPosition(Vector2f(player.tailsx[t], player.tailsy[t]));
				window->draw(shape);
			}
			//score
			Text score(font);
			score.setString("Player1 Score : " + player.score);
			score.setFillColor(Color(255, 255, 0));
		}
	}
}
void InitWindow() {
	RenderWindow window(VideoMode({ 800,600 }), "Tanki");
	WindowHandle handle = window.getNativeHandle();
	window.setActive(true);
	window.setFramerateLimit(20);
	sf::Font font;
	bool loaded = font.openFromFile("Fonts/Minecraft.ttf");
	x = window.getSize().x / 2;
	y = window.getSize().y / 2;
	fx = rand() % window.getSize().x;
	fy = rand() % window.getSize().y;
	while (window.isOpen()) {
		window.clear();
		Draws(&window, font);
		if (!GameOver) {
			Inputs(&window);
			GameLogic(&window);
			if (Send)
				SecendPlayer(&window,font);
			window.display();
		}
		else {
			window.display();
			this_thread::sleep_for(chrono::seconds(5));
		}
	}
}

int main()
{
	std::cout << "Did you want play Mulity ? " << endl;
	std::cout << "1 = Host" << endl;
	std::cout << "2 = Client" << endl;
	std::cout << "other = offline" << endl;
	char result = std::cin.get();
	if (result == '1') {
		//host
		TcpListener socket;
		if (socket.listen(8060) != Socket::Status::Done) {
			//error
		}
		if (socket.accept(socketPlayer) != Socket::Status::Done) {
			//error
		}
		Send = true;
	}
	else if (result == '2') {
		//client
		Socket::Status Status = socketPlayer.connect({ 127,0,0,1 }, 8060);
		if (Status != Socket::Status::Done) {
			//error
		}
		Send = true;
	}
	InitWindow();
	std::cout << "Hello World!\n";
}