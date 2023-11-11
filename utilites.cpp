#include "utilites.h"
#include <cstring> // Pour utiliser memset
//#include <stdio.h> // Pour utiliser getchar()
#include <sys/ioctl.h> // Pour avoir les dimensions du terminal

int yScreenSize, xScreenSize;

void Scene::fillArray(GameObject** array)
{
	array[0] = &player;
	for (int i = 0; i < NBENEMIES; i++)
		array[i + 1] = &enemies[i];
	for (int i = 0; i < NBBULLETS; i++)
		array[i + NBENEMIES + 1] = &bullets[i];
}

void Player::setBulletsPtr(Bullet* bulletsPtr)
{
	this->bulletsPtr = bulletsPtr;
}

void Player::setInputmPtr(InputManager* inputmPtr)
{
	this->inputmPtr = inputmPtr;
}

void Enemy::setBulletsPtr(Bullet* bulletsPtr)
{
	this->bulletsPtr = bulletsPtr;
}

void Enemy::setScorePtr(int* scorePtr)
{
	this->scorePtr = scorePtr;
}

void Scene::fillPointers(int * scorePtr, InputManager * inputmPtr)
{
	player.setBulletsPtr(bullets);
	for (int i = 0; i <  NBENEMIES; i++)
	{
		enemies[i].setBulletsPtr(bullets);
		enemies[i].setScorePtr(scorePtr);
	}
	player.setInputmPtr(inputmPtr);
}

void Engine::run()
{
	std::thread input (&Engine::UpdateInput,this);
	while(runGame)
	{
		update();
		draw();
		usleep(50000);
	}
	input.join();
}

void Engine::setup()
{
	srand(time(NULL));
	runGame = true;
	score = 0;

	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	yScreenSize = w.ws_row;
	xScreenSize = w.ws_col;

	scene.fillArray(objects);
	scene.fillPointers(&score,&input);

	for (int i = 0; i < NBOBJECTS; i++)
		if (objects[i] != nullptr)
			objects[i]->setup();

	input.InitInput();
	input.runGamePtr = &runGame;
}

void Engine::update()
{
	for (int i = 0; i < NBOBJECTS; i++)
		if (objects[i] != nullptr)
			objects[i]->update();
	//ReadInput(); // ÇA BLOQUE ÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆÆ
}

void Engine::draw()
{
	//printf("\e[2J");
	for (int i = (NBOBJECTS - 1); i >= 0; i--)
		if (objects[i] != nullptr)
			objects[i]->draw();
	//fflush(stdout);
}

void GameObject::setup()
{
}

void Player::setup()
{
	ypos = yScreenSize / 1.3;
	xpos = xScreenSize / 2;
}

void Enemy::setup()
{
	ypos = rand() % yScreenSize / 3 + 5;
	xpos = rand() % xScreenSize + 1;
}

void GameObject::update()
{
}

void Player::update()
{
	if (inputmPtr->LRUDS[0])
	{
		if (inputmPtr->LRUDS[2])
			this->move(-1,-1);
		else if (inputmPtr->LRUDS[3])
			this->move(1,-1);
		else
			this->move(0,-1);
	}
	else if (inputmPtr->LRUDS[1])
	{
		if (inputmPtr->LRUDS[2])
			this->move(-1,1);
		else if (inputmPtr->LRUDS[3])
			this->move(1,1);
		else
			this->move(0,1);
	}
	else if (inputmPtr->LRUDS[2])
		this->move(-1,0);
	else if (inputmPtr->LRUDS[3])
		this->move(1,0);
	if (inputmPtr->LRUDS[4])
		this->shoot(-1,rand() % 3 - 1);
}

void Enemy::update()
{
	if (!dead)
	{
		int tempY, tempX;
		for (int i = 0; i < (NBBULLETS / 2); i++)
		{
			if (bulletsPtr[i].isFired())
			{
				tempY = bulletsPtr[i].getYpos();
				tempX = bulletsPtr[i].getXpos();
				if (tempY >= ypos-1 && tempY <= ypos+1 && tempX >= xpos-2 && tempX <= xpos+2)
				{
					(*scorePtr)++;
					bulletsPtr[i].endFire();
					yLastMv = 0;
					xLastMv = 0;
					dead = true;
				}
			}
		}
	}
	if (!dead)
	{
		int varTemp = (rand() % 2 ? 1 : -1);
		if (rand() % 2)
		{
			xLastMv = 0;
			if (ypos <= 0+1)
			{
				ypos += 1;
				yLastMv = 1;
			}
			else if (ypos > yScreenSize-1)
			{
				ypos -= 1;
				yLastMv = -1;
			}
			else
			{
				ypos += varTemp;
				yLastMv = varTemp;
			}
		}
		else
		{
			yLastMv = 0;
			if (xpos <= 0+2)
			{
				xpos += 1;
				xLastMv = 1;
			}
			else if (xpos > xScreenSize-2)
			{
				xpos -= 1;
				xLastMv = -1;
			}
			else
			{
				xpos += varTemp;
				xLastMv = varTemp;
			}
		}
	}
}

void Bullet::update()
{
	if (fired)
	{
		ypos += yMv;
		xpos += xMv;
		if (ypos <= 0 || ypos >= yScreenSize || xpos <= 0 || xpos >= xScreenSize)
		{
			this->endFire();
		}
	}
}

void GameObject::draw()
{
}

void Player::draw()
{
	DrawRect(ypos,xpos,4,2,37);

	if (yLastMv != 0)
		for (int i = xpos-5;i < xpos+6;i++)
			printf("\e[%d;%dH ",yLastMv > 0 ? ypos-3 : ypos+3,i);
	if (xLastMv != 0)
		for (int i = ypos-3;i < ypos+4;i++)
			printf("\e[%d;%dH ",i,xLastMv > 0 ? xpos-5 : xpos+5);
}

void Enemy::draw()
{
	DrawRect(ypos,xpos,2,1,36);

	if (yLastMv != 0)
		for (int i = xpos-3;i < xpos+4;i++)
			printf("\e[%d;%dH ",yLastMv > 0 ? ypos-2 : ypos+2,i);
	if (xLastMv != 0)
		for (int i = ypos-1;i < ypos+2;i++)
			printf("\e[%d;%dH ",i,xLastMv > 0 ? xpos-3 : xpos+3);
}

void Bullet::draw()
{
	if (fired)
	{
		printf("\e[0;31m");
		printf("\e[%d;%dH*",ypos,xpos);
		printf("\e[0;37m");
		printf("\e[%d;%dH*",ypos-yMv,xpos-xMv);
		printf("\e[%d;%dH ",ypos-(2*yMv),xpos-(2*xMv));
	}
	else if (clearSprite) 	
	{
		printf("\e[%d;%dH ",ypos,xpos);
		printf("\e[%d;%dH ",ypos-yMv,xpos-xMv);
		printf("\e[%d;%dH ",ypos-(2*yMv),xpos-(2*xMv));
		clearSprite = false;
	}
}

void Bullet::beginFire(int y, int x)
{
	yMv = y;
	xMv = x;
	fired = true;
}

void Bullet::endFire()
{
	fired = false;
	clearSprite = true;
}

bool Bullet::isFired()
{
	return fired;
}

int Bullet::getXpos()
{
	return xpos;
}

int Bullet::getYpos()
{
	return ypos;
}

void Bullet::setPos(int y, int x)
{
	ypos = y;
	xpos = x;
}

void Player::shoot(int y, int x)
{
	int i;
	for (i = 0; i < (NBBULLETS / 2); i++)
	{
		if (!bulletsPtr[i].isFired())
			break;
	}
	if (i != (NBBULLETS / 2))
	{
		bulletsPtr[i].beginFire(y,x);
		bulletsPtr[i].setPos(ypos-3,xpos);
	}
}

void Player::move(int y, int x)
{
	ypos += y;
	yLastMv = y;
	xpos += x;
	xLastMv = x;
}

void ReadInput()
{
	char buffer[8];
	int count = read(0,buffer,8);
	memset(buffer+count,0,8-count);
	/*
	if (buffer[0] == '\e')
	{
		switch (count)
		{
			case 1:
				*gameStatus = false;
				repeat = false;
			case 3:
				if (buffer[1] != '[') break;
				switch (buffer[2])
				{
					case 'A':
						//player->move(-1,0);
						//player->printSprite(-1,0);
						break;
					case 'B':
						//player->move(1,0);
						//player->printSprite(1,0);
						break;
					case 'C':
						//player->move(0,1);
						//player->printSprite(0,1);
						break;
					case 'D':
						//player->move(0,-1);
						//player->printSprite(0,-1);
						break;

				}
				break;
		}
	}
	if (buffer[0] == 'w')
	{
		//player->shoot(bullet,bulletAvail);
		//player->printSprite(0,0);
	}
	if (buffer[0] == 'x')
	{
		//player->printSprite(0,0);
	}
	*/
}

void DrawRect(int y, int x, int w, int l,int color)
{
	for (int i = x-w;i <= x+w;i++)
	{
		for (int j = y-l;j <= y+l;j++)
		{
			printf("\e[0;%dm",color);
			printf("\e[%d;%dH%%",j,i);
		}
	}	
}
