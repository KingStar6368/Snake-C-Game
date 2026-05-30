// TankiGame.cpp : Snake game implementation for SFML 2.6.2
//

#include <iostream>
#include <SFML/Graphics.hpp>
#include <random>
#include <thread>
#include <chrono>
#include <sstream>
#include <cmath>

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

using namespace std;
using namespace sf;

void Draws(RenderWindow* window) {
    Vector2u windowsize = window->getSize();

    // Draw walls
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

    // Draw snake head
    CircleShape HSnake;
    HSnake.setFillColor(Color(0, 255, 0));
    HSnake.setPosition(Vector2f(x, y));
    HSnake.setRadius(10);

    window->draw(HSnake);

    // Draw snake tail
    for (int t = 0; t < tailcount; t++) {
        CircleShape tail;
        tail.setFillColor(Color(0, 255, 0));
        tail.setPosition(Vector2f(tailsx[t], tailsy[t]));
        tail.setRadius(8);
        window->draw(tail);
    }

    // Draw fruit
    CircleShape Fruit;
    Fruit.setFillColor(Color(255, 0, 0));
    Fruit.setPosition(Vector2f(fx, fy));
    Fruit.setRadius(8);
    window->draw(Fruit);

    // Draw score
    Font font;
    if (font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf")) {
        Text scoreText;
        scoreText.setFont(font);
        scoreText.setString("Score: " + to_string(score));
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(Color::White);
        scoreText.setPosition(Vector2f(10, 10));
        window->draw(scoreText);
    }

    // Draw Game Over
    if (GameOver) {
        if (font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf")) {
            Text txt;
            txt.setFont(font);
            txt.setString("Game Over! Score: " + to_string(score));
            txt.setCharacterSize(36);
            txt.setFillColor(Color(255, 0, 0));
            txt.setPosition(Vector2f(windowsize.x / 2 - 100, windowsize.y / 2));
            window->draw(txt);
        }
    }
}

void Inputs(RenderWindow* window) {
    Event event;
    while (window->pollEvent(event))
    {
        if (event.type == Event::Closed) {
            GameOver = true;
            window->close();
        }
        else if (event.type == Event::KeyPressed) {
            if (event.key.scancode == Keyboard::Scancode::Escape || event.key.scancode == Keyboard::Scancode::X)
            {
                GameOver = true;
                window->close();
            }
            else if (event.key.scancode == Keyboard::Scancode::W && Direction != Dir::Down) {
                Direction = Dir::Up;
            }
            else if (event.key.scancode == Keyboard::Scancode::S && Direction != Dir::Up) {
                Direction = Dir::Down;
            }
            else if (event.key.scancode == Keyboard::Scancode::A && Direction != Dir::Right) {
                Direction = Dir::Left;
            }
            else if (event.key.scancode == Keyboard::Scancode::D && Direction != Dir::Left) {
                Direction = Dir::Right;
            }
        }
    }
}

void GameLogic(RenderWindow* window) {
    if (GameOver) return;

    Vector2u windowsize = window->getSize();

    // Update tail positions
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

    // Move snake based on direction
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
    case Stop:
        break;
    }

    // Check collision with self
    if (tailcount > 1) {
        for (int t = 0; t < tailcount; t++) {
            double dis = sqrt((pow(x - tailsx[t], 2) + pow(y - tailsy[t], 2)));
            if (dis < 10) {
                GameOver = true;
            }
        }
    }

    // Check collision with fruit
    double dis = sqrt(pow((x - fx), 2) + pow((y - fy), 2));
    if (dis < 15) {
        score += 10;
        // Generate new fruit position
        do {
            fx = rand() % (windowsize.x - 50) + 20;
            fy = rand() % (windowsize.y - 50) + 20;
        } while (fx < 10 || fx > windowsize.x - 20 || fy < 10 || fy > windowsize.y - 20);
        tailcount++;
    }

    // Check collision with walls
    if (x < 10 || x > windowsize.x - 20 || y < 10 || y > windowsize.y - 20) {
        GameOver = true;
    }
}

void InitWindow() {
    RenderWindow window(VideoMode(800, 600), "Snake Game");
    window.setActive(true);
    window.setFramerateLimit(15);

    // Initialize random seed
    srand(time(nullptr));

    // Initialize snake position
    x = window.getSize().x / 2;
    y = window.getSize().y / 2;

    // Initialize fruit position
    fx = rand() % (window.getSize().x - 50) + 20;
    fy = rand() % (window.getSize().y - 50) + 20;

    // Initialize tail arrays
    for (int i = 0; i < 100; i++) {
        tailsx[i] = 0;
        tailsy[i] = 0;
    }

    Direction = Dir::Stop;

    while (window.isOpen()) {
        window.clear(Color::Black);
        Inputs(&window);
        GameLogic(&window);
        Draws(&window);
        window.display();

        // Small delay to control game speed
        if (GameOver) {
            sf::sleep(sf::seconds(3));
            window.close();
        }
    }
}

int main()
{
    cout << "Snake Game Started!" << endl;
    InitWindow();
    cout << "Final Score: " << score << endl;
    cout << "Game Over!" << endl;
    return 0;
}