#pragma once
#include <iostream> // Pour utiliser std::cin et autres
#include <unistd.h> // Pour utiliser read() et usleep() 
#include <time.h>
#include <stdlib.h>
#include <thread> // Pour utiliser std::thread
#include "InputRawReader.h"

#define NBENEMIES 16
#define NBBULLETS 32
#define NBOBJECTS 64

class GameObject
{
	protected:
		int ypos,xpos;
	public:
		virtual void setup();
		virtual void update();
		virtual void draw();
};
class Bullet : public GameObject
{
		int yMv,xMv;
		bool fired = false;
		bool clearSprite = false;
	public:
		void update();
		void draw();
		void setPos(int,int);
		int getYpos();
		int getXpos();
		bool isFired();
		void endFire();
		void beginFire(int,int);
};
class Enemy : public GameObject
{
		int yLastMv,xLastMv;
		Bullet * bulletsPtr;
		int * scorePtr;
		bool dead = false;
	public:
		void setup();
		void update();
		void draw();
		void setBulletsPtr(Bullet*);
		void setScorePtr(int*);
};
class InputManager : public RawReader
{
	protected:
		void OnInput(int key, bool pressed)
		{
			if (key == 105) // flèche gauche
				LRUDS[0] = pressed;
			else if (key == 106) // flèche droite
				LRUDS[1] = pressed;
			else if (key == 103) // flèche haute
				LRUDS[2] = pressed;
			else if (key == 108) // flèche bas
				LRUDS[3] = pressed;
			else if (key == 44) // w
				LRUDS[4] = pressed;
			else if (key == 1) // Échap
				(*runGamePtr) = false;
		}
	public:
		bool LRUDS[5] = { false };
		bool * runGamePtr;
};
class Player : public GameObject
{
	private:
		int yLastMv,xLastMv;
		Bullet * bulletsPtr;
		bool dead = false;
		InputManager * inputmPtr;
	public:
		char * buffer;
		void setup();
		void update();
		void draw();
		void move(int,int);
		void shoot(int,int);
		void setBulletsPtr(Bullet*);
		void setInputmPtr(InputManager*);
};
struct Scene
{
	Player player;
	Enemy enemies[NBENEMIES];
	Bullet bullets[NBBULLETS];
	void fillArray(GameObject**);
	void fillPointers(int*,InputManager*);
};
class Engine
{
		bool runGame;
		Scene scene;
		GameObject * objects[NBOBJECTS] = {nullptr};
		InputManager input;
	public:
		char buffer[8];
		int score;
		void setup();
		void update();
		void draw();
		void run();
		void UpdateInput()
		{
			while (runGame)
				input.UpdateInput();
		}
};
void ReadInput();
void DrawRect(int,int,int,int,int);
extern int yScreenSize, xScreenSize;
