#include <stdio.h>
#include <iostream>
#include <tchar.h>
#include <windows.h>
#include <conio.h>
#include <ctime>
#include <math.h>
#include <vector>
#include "SystemMsg.h"
using namespace std;

#ifdef _DEBUG
#include <crtdbg.h>
#define _CRTDBG_MAP_ALLOC
#endif

const short enemycount = 4, enemyprojectiles = 30, playerprojectiles = 10, walls = 4,wallsize=10;
const int XMinGround = 300, XMaxGround = 600, YMinGround = 10, YMaxGround = 400;
int Score = 0, MaxLife=50;
BOOL pressed[10], collision[4];

class Object 
{
protected:
	int x, y, size;
	//char image[];
public:
	Object(int x, int y, int size) 
	{
		this->x = x;
		this->y = y;
		this->size = size;
	}
	virtual void draw(HDC hdc) = 0;
	virtual void erase(HDC hdc) = 0;
	int getx()
	{
		return x;
	}
	int gety() 
	{
		return y;
	}
	int getsize() 
	{
		return size;
	}
	void setx(int x) 
	{
		this->x = x;
	}
	void sety(int y) 
	{
		this->y = y;
	}
	void setsize(int size) 
	{
		this->x = size;
	}
};


class SolidObject : public Object 
{
protected:
	//char hitbox[];
public:
	SolidObject(int x, int y, int size) : Object(x, y, size) {};
	//void CheckCollision();
	virtual void init() {};
	virtual void Collision(int xc1, int xc2, int yc1, int yc2) {};
};


class Wall : public SolidObject 
{
protected:
	int size2 ;
private:
	int xc1 = 0, xc2 = 0, yc1 = 0, yc2 = 0;
public:
	Wall(int x=0, int y=0, int size=0, int size2=0) : SolidObject(x, y, size) 
	{
		this->size2 = size2;
	}
	void draw(HDC hdc) 
	{
		HPEN hPen = CreatePen(PS_SOLID, 0, RGB(255, 255, 255));
		HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
		Rectangle(hdc, x - size / 2, y - size2 / 2, x + size / 2, y + size2 / 2);
		DeleteObject(hPen);
	}
	int getcorner(const char cord[2]) 
	{
		switch (cord[0]) {
		case 'x':
			if (cord[1] == '1')
				return xc1;
			else
				if (cord[1] == '2')
					return xc2;
			break;
		case 'y':
			if (cord[1] == '1')
				return yc1;
			else
				if (cord[1] == '2')
					return yc2;
			break;
		}
		return -1;
	}

	void init(int x, int y, int size, int size2)
	{
		this->x = x;
		this->y = y;
		this->size = size;
		this->size2 = size2;
		yc1 = y - size2 / 2; yc2 = y + size2 / 2;
		xc1 = x - size / 2, xc2 = x + size / 2;
	}
	void erase(HDC hdc) 
	{
		HPEN hPen = CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
		HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
		Rectangle(hdc, x - size / 2, y - size2 / 2, x + size / 2, y + size2 / 2);
		DeleteObject(hPen);
	}
};


class GhostObject : public Object 
{
protected:
	int vx, vy;
	//char hitbox[]
public:
	GhostObject(int x, int y, int size, int vx, int vy) : Object(x, y, size)
	{
		this->vx = vx;
		this->vy = vy;
	}
	virtual void ObjectMove(HDC hdc, message* msg)=0;

	int getvx() 
	{
		return vx;
	}
	int getvy() 
	{
		return vy;
	}
	void setvx(int vx) 
	{
		this->vx = vx;
	}
	void setvy(int vy) 
	{
		this->vy = vy;
	}
};



class Projectile : public GhostObject 
{
protected:
	int damage;
public:
	Projectile(int x = 0, int y = 0, int size = 0, int vx = 0, int vy = 0, int damage = 0) : GhostObject(x, y, size, vx, vy) 
	{

		this->damage = damage;
	}

	//~Projectile() { cout << "PROJ_R" << std::endl; }

	virtual void draw(HDC hdc) 
	{

		HPEN hPen = CreatePen(PS_INSIDEFRAME, size / 4, RGB(255, 150, 100));
		HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
		if(vx==0)
			Rectangle(hdc, x - size / 4, y + size / 2, x + size / 4, y - size / 2);
		else
			Rectangle(hdc, x - size / 2, y + size / 4, x + size / 2, y - size / 4);
		DeleteObject(hPen);
	}


	virtual void erase(HDC hdc) 
	{

		HPEN hPen = CreatePen(PS_INSIDEFRAME, size / 4, COLOR_BACKGROUND);
		HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
		MoveToEx(hdc, x, y, NULL);
		if(vx==0)
			Rectangle(hdc, x - size / 4, y + size / 2, x + size / 4, y - size / 2);
		else
			Rectangle(hdc, x - size / 2, y + size / 4, x + size / 2, y - size / 4);
		DeleteObject(hPen);
	}


	void ObjectMove(HDC hdc,message *msg)
	{
		erase(hdc);
		if (x > XMinGround + wallsize / 2 && x<XMaxGround - wallsize / 2 && y>YMinGround + wallsize / 2 && y < YMaxGround - wallsize / 2)
		{
				y += vy;
				x += vx;
			draw(hdc);
		}
		else 
			msg->code= PROJ_DELETE;
	}


	void init(int x, int y, int size, int vx, int vy, int damage) 
	{
		this->x = x;
		this->y = y;
		this->size = size;
		this->vx = vx;
		this->vy = vy;
		this->damage = damage;

	}


	int getdamage() 
	{
		return damage;
	}


	virtual void WhoAmI(message* msg)
	{
		msg->code = I_AM_PROJ_STD;
		msg->values[2] = RECT;
	}
};

class ProjectileR : public Projectile 
{
public:
	ProjectileR(int x = 0, int y = 0, int size = 0, int vx = 0, int vy = 0, int damage = 0) : Projectile(x, y, size, vx, vy, damage) {};

	//~ProjectileR(){ cout << "PROJ_R" << std::endl; }

	void draw(HDC hdc)
	{

		HPEN hPen = CreatePen(PS_INSIDEFRAME, size, RGB(255, 100, 100));
		HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
		MoveToEx(hdc, x + size / 2, y, NULL);
		AngleArc(hdc, x, y, size / 2, 0, 360);
		DeleteObject(hPen);
		DeleteObject(hOldPen);
	}


	void erase(HDC hdc) 
	{

		HPEN hPen = CreatePen(PS_INSIDEFRAME, size, COLOR_BACKGROUND);
		HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
		MoveToEx(hdc, x + size / 2, y, NULL);
		AngleArc(hdc, x, y, size / 2, 0, 360);
		DeleteObject(hPen);
		DeleteObject(hOldPen);
	}

	virtual void WhoAmI(message* msg)
	{
		msg->code = I_AM_PROJ_RND;
		msg->values[2] = CIRC;
	}
};


class Enemy : public GhostObject 
{
protected:
	int health, score;
public:
	Enemy(int x=0, int y=0, int size=0, int vx=0, int vy=0, int health=0, int score=0) : GhostObject(x, y, size, vx, vy) 
	{
		this->health = health;
		this->score = score;
	}


	int gethealth() 
	{
		return health;
	}
	int getscore() {
		return score;
	}


	void sethealth(int health) 
	{
		this->health = health;
	}


	void init(int x, int y, int size, int vx, int vy, int health, int score)
	{
		this->x = x;
		this->y = y;
		this->size = size;
		this->vx = vx;
		this->vy = vy;
		this->health = health;
		this->score = score;
	}
	virtual void Fire(HDC hdc, vector<Projectile*>* EnemyPr, message* msg) {}
	
	virtual void SetRandomSeed(int pop, int seed) = 0;
	
	virtual void ObjectMove(HDC hdc, message* msg) = 0;
	
	virtual void WhoAmI(message* msg) = 0;
	
	bool CheckArea(HDC hdc, message* msg, vector<Projectile>::iterator it);
};

bool Enemy::CheckArea(HDC hdc, message* msg, vector<Projectile>::iterator it)
{
	if (x + size / 2 - it->getx() - it->getsize() / 4 <= size &&
		-x + size / 2 + it->getx() + it->getsize() / 4 <= size &&
		it->gety() + it->getsize() / 2 - y + size / 2 <= it->getsize() + size &&
		y + size / 2 - it->gety() + it->getsize() / 2 <= size + it->getsize())
	{
		health -= it->getdamage();
		it->erase(hdc);
		if (health <= 0)
		{
			Score += score;
			msg->code = ENEM_DELETE;
		}
		else
			draw(hdc);
		return true;
	}
	else
	{
		msg->code = NULL;
		return false;
	}



};


class EnemyBiter : public Enemy
{
private:
	short tick = 0;
	int bvx = 0, bvy = 0;
public:
	EnemyBiter(int x = 0, int y = 0, int size = 0, int vx = 0, int vy = 0, int health = 0, int score = 0) : Enemy(x, y, size, vx, vy, health, score) {};


	void SetRandomSeed(int pop, int seed)
	{
		this->size = 15 + seed / 4 * 4;
		this->vx = 18 / ((size) * 1.5 / 2);
		this->vy = 18 / ((size) * 1.5 / 2);
		this->health = size + pop % 2 + seed / 4;
		this->score = health + vx * 2 - size / 4;
		this->x = XMinGround + wallsize / 2 + size / 2 + (rand() + pop * 10) % (XMaxGround - XMinGround - size / 2);
		this->y = YMinGround + wallsize / 2 + size / 2 + (rand() + pop * 7) % 50;
		this->bvx = vx;
		this->bvy = vy;
	}


	void draw(HDC hdc)
	{
		HPEN hPenl = CreatePen(PS_INSIDEFRAME, size, RGB(100, 155, 255));
		HPEN hOldPenl = (HPEN)SelectObject(hdc, hPenl);
		MoveToEx(hdc, x + size / 8, y, NULL);
		AngleArc(hdc, x, y, size / 8, 0, 360);
		HPEN hPend = CreatePen(PS_INSIDEFRAME, size / 3, RGB(255, 100, 100));
		HPEN hOldPend = (HPEN)SelectObject(hdc, hPend);
		MoveToEx(hdc, x - size / 2, y, NULL);
		LineTo(hdc, x + size / 2, y);
		MoveToEx(hdc, x, y - size / 2, NULL);
		LineTo(hdc, x, y + size / 2);
		DeleteObject(hPend);
		DeleteObject(hPenl);
	}


	void erase(HDC hdc)
	{

		HPEN hPenl = CreatePen(PS_INSIDEFRAME, size, COLOR_BACKGROUND);
		HPEN hOldPenl = (HPEN)SelectObject(hdc, hPenl);
		MoveToEx(hdc, x + size / 8, y, NULL);
		AngleArc(hdc, x, y, size / 8, 0, 360);
		HPEN hPend = CreatePen(PS_INSIDEFRAME, size / 3, COLOR_BACKGROUND);
		HPEN hOldPend = (HPEN)SelectObject(hdc, hPend);
		MoveToEx(hdc, x - size / 2, y, NULL);
		LineTo(hdc, x + size / 2, y);
		MoveToEx(hdc, x, y - size / 2, NULL);
		LineTo(hdc, x, y + size / 2);
		DeleteObject(hPend);
		DeleteObject(hPenl);
	}


	virtual void ObjectMove(HDC hdc, message* msg)
	{
		if (tick % 2)
		{
			if (abs(x - msg->values[0]) > 4)
			{
				if (x - msg->values[0] < 0)
				{
					erase(hdc);
					if (x + size / 2 < XMaxGround - wallsize / 2)
						vx = abs(bvx);
				}
				else
				{
					erase(hdc);
					if (x - size / 2 > XMinGround + wallsize / 2)
						vx = -abs(bvx);
				}

			}
			else
				vx = 0;

			if (abs(y - msg->values[1]) > 4)
			{
				if (y - msg->values[1] < 0)
				{
					erase(hdc);
					if (y + size / 2 < YMaxGround - wallsize / 2)
						vy = abs(bvy);
				}
				else
				{
					erase(hdc);
					if (y - size / 2 > YMinGround + wallsize / 2)
						vy = -abs(bvy);
				}

			}
			else
				vy = 0;


			x += vx;
			y += vy;

			draw(hdc);
		}
	}

	void Fire(HDC hdc, vector<Projectile*>* EnemyPr, message* msg)
	{
		tick++;
	}

	virtual void WhoAmI(message* msg)
	{
		msg->code = I_AM_ENEM_BITER;
		msg->values[2] = CIRC;
	}
};


class EnemyPersuer : public Enemy 
{
private:
	short tick = 0;
	int bvx = 0;

public:
	EnemyPersuer(int x = 0, int y = 0, int size = 0, int vx = 0, int vy = 0, int health = 0, int score = 0) : Enemy(x, y, size, vx, vy, health, score) {};
	
	
	void draw(HDC hdc) 
	{

		HPEN hPen = CreatePen(PS_INSIDEFRAME, size/2, RGB(100, 100, 255));
		HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
		Rectangle(hdc, x - size / 2, y + size / 2, x + size / 2, y - size / 2);
		DeleteObject(hPen);
		DeleteObject(hOldPen);
	}


	void erase(HDC hdc) 
	{

		HPEN hPen = CreatePen(PS_INSIDEFRAME, size/2, COLOR_BACKGROUND);
		HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
		Rectangle(hdc, x - size / 2, y + size / 2, x + size / 2, y - size / 2);
		DeleteObject(hPen);
		DeleteObject(hOldPen);
	}


	virtual void ObjectMove(HDC hdc, message* msg) 
	{
		if ( abs(x - msg->values[0]) > 9)
		{
			if (x - msg->values[0] < 0)
			{
				erase(hdc);
				if (x + size / 2 < XMaxGround - wallsize / 2)
					vx = abs(bvx);
			}
			else
			{
				erase(hdc);
				if (x - size / 2 > XMinGround + wallsize / 2)
					vx = -abs(bvx);
			}
			
		}
		else
			vx = 0;
		x += vx;
		draw(hdc);
	}


	void SetRandomSeed(int pop, int seed)
	{
		this->size = 15 + seed/4*4;
		this->vx = 18 / ((size)*1.5/2);
		this->vy = 0;
		this->health = size + pop%2 + seed / 4;
		this->score = health + vx * 2 - size/4;
		this->x = XMinGround + wallsize / 2 + size / 2 + (rand()+pop*10) % (XMaxGround - XMinGround - size / 2);
		this->y = YMinGround + wallsize / 2 + size / 2 + (rand()+pop*7) % 50;
		this->bvx = vx;
	}


	void Fire(HDC hdc, vector<Projectile*>* EnemyPr, message* msg)
	{
		if (tick)
			tick--;
		if (tick == 0) {
			EnemyPr->push_back(new ProjectileR);
			vector<Projectile*>::iterator it;
			it = EnemyPr->end()-1;
			if(msg->values[1]>y)
				(*it)->init(x, y + size / 2, size / 3, vx - vx / 3, 4 - size / 7, size - health / 5);
			else
				(*it)->init(x, y + size / 2, size / 3, vx - vx / 3, -(4 - size / 7), size - health / 5);
			tick = size * 5 + health * 2;
		}
	}


	virtual void WhoAmI(message* msg)
	{
		msg->code = I_AM_ENEM_PERS;
		msg->values[2] = RECT;
	}
};


class EnemyCharger : public Enemy
{
private:
	int tick = 0;
	int trig = 0;
public:
	EnemyCharger(int x = 0, int y = 0, int size = 0, int vx = 0, int vy = 0, int health = 0, int score = 0) : Enemy(x, y, size, vx, vy, health, score) {};


	void draw(HDC hdc) 
	{

		HPEN hPen = CreatePen(PS_INSIDEFRAME, size / 2, RGB(100, 200, 200));
		HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
		MoveToEx(hdc, x - size / 2, y - size / 2, NULL);
		LineTo(hdc, x + size / 2, y + size / 2);
		MoveToEx(hdc, x + size / 2, y - size / 2, NULL);
		LineTo(hdc, x - size / 2, y + size / 2);
		hPen = CreatePen(PS_INSIDEFRAME, size, RGB(100, 255, 255));
		hOldPen = (HPEN)SelectObject(hdc, hPen);
		MoveToEx(hdc, x + size / 8, y, NULL);
		AngleArc(hdc, x, y, size / 8, 0, 360);
		DeleteObject(hPen);
	}


	void erase(HDC hdc) 
	{

		HPEN hPen = CreatePen(PS_INSIDEFRAME, size / 2, COLOR_BACKGROUND);
		HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
		MoveToEx(hdc, x - size / 2, y - size / 2, NULL);
		LineTo(hdc, x + size / 2, y + size / 2);
		MoveToEx(hdc, x + size / 2, y - size / 2, NULL);
		LineTo(hdc, x - size / 2, y + size / 2);
		hPen = CreatePen(PS_INSIDEFRAME, size, COLOR_BACKGROUND);
		hOldPen = (HPEN)SelectObject(hdc, hPen);
		MoveToEx(hdc, x + size / 8, y, NULL);
		AngleArc(hdc, x, y, size / 8, 0, 360);
		DeleteObject(hPen);
	}


	void SetRandomSeed(int pop, int seed)
	{
		this->size = seed+12 + pop%2;
		this->vx = 50/size/2;
		this->vy = 26 / ((size)/2);
		this->health = size*1.5/2 + pop % 2 + (10-seed) / 2;
		this->score = (health + vx + vy)*2-size/4;
		this->x = XMinGround + wallsize / 2 + size / 2 + (rand() + pop * 10) % (XMaxGround - XMinGround - size);
		this->y = YMinGround + wallsize / 2 + size / 2;
	}
	

	virtual void ObjectMove(HDC hdc, message* msg) 
	{
		if (tick % 2) 
		{
			
			if (tick % (3 * (size * 5 + health * 2)) < size * 5 + health * 2)
			{
				erase(hdc);
				y += vy;
			}
			else
				if (tick % (3 * (size * 5 + health * 2)) < (size * 5 + health * 2) * 2)
				{
				}
				else
				{
					erase(hdc);
					trig = (rand() + size - health) % 2;
					if (x + size / 2 < XMaxGround - wallsize / 2 && trig == 0)
					{
						x += vx;
					}
					else
						if (x - size / 2 > XMinGround + wallsize / 2 && trig == 1)
							x -= vx;
				}
			draw(hdc);
			if (y >= YMaxGround - wallsize)
				msg->code = ENEM_DELETE;
		}
	}


	void Fire(HDC hdc, vector<Projectile*>* EnemyPr, message* msg)
	{
		tick++;
		int trig = 0;
		int j = 0;
		if (tick % (3 * (size * 5 + health * 2)) == (size * 5 + health * 2))
		{
			for (int i = 0; i < 6; i++) 
			{
				EnemyPr->push_back(new ProjectileR);
				vector<Projectile*>::iterator it;
				it = EnemyPr->end() - 1;
				switch (i) {
				case 0:
					(*it)->init(x, y + size / 2, size / 3, 0, 3 - size / 8, size - health / 5);
					break;
				case 1:
					(*it)->init(x + size / 2, y + size / 2, size / 3, 3 - size / 8, 3 - size / 8, size - health / 5);
					break;
				case 2:
					(*it)->init(x - size / 2, y + size / 2, size / 3, -(3 - size / 8), 3 - size / 8, size - health / 5);
					break;
				case 3:
					(*it)->init(x, y - size / 2, size / 3, 0, -(3 - size / 8), size - health / 5);
					break;
				case 4:
					(*it)->init(x + size / 2, y - size / 2, size / 3, 3 - size / 8, -(3 - size / 8), size - health / 5);
					break;
				case 5:
					(*it)->init(x - size / 2, y - size / 2, size / 3, -(3 - size / 8), -(3 - size / 8), size - health / 5);
					break;
				}
			}
		}
	}


	virtual void WhoAmI(message* msg)
	{
		msg->code = I_AM_ENEM_CHAR;
		msg->values[2] = RECT;
	}
};


class PlayerShip : public SolidObject 
{
protected:
	int vx, vy, health;
private:
	short trig = 0;
public:
	PlayerShip(int x = 0, int y = 0, int size = 0, int vx = 0, int vy = 0, int health = 0) : SolidObject(x, y, size) 
	{
		this->vx = vx;
		this->vy = vy;
		this->health = health;
	}

	void draw(HDC hdc, COLORREF Color)
	{
		HPEN hPen = CreatePen(PS_INSIDEFRAME, size/2,Color);
		HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
		MoveToEx(hdc, x + size/4, y, NULL);
		AngleArc(hdc, x, y, size/ 4, 0, 360);
		DeleteObject(hPen);
		DeleteObject(hOldPen);

	}

	void draw(HDC hdc) 
	{
		HPEN hPen = CreatePen(PS_SOLID, 2, RGB(100, 255, 100));
		HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
		MoveToEx(hdc, x + size/2, y, NULL);
		AngleArc(hdc, x, y, size/2, 0, 360);
		DeleteObject(hPen);
		DeleteObject(hOldPen);
		
	}


	void erase(HDC hdc) 
	{

		HPEN hPen = CreatePen(PS_SOLID, size/3, RGB(0,0,0));
		HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
		MoveToEx(hdc, x + size/2, y, NULL);
		AngleArc(hdc, x, y, size/2, 0, 360);
		DeleteObject(hPen);
	}


	void ObjectMove(HDC hdc)
	{
		HANDLE HandleInput = GetStdHandle(STD_INPUT_HANDLE);
		DWORD NumInputs = 0;

		GetNumberOfConsoleInputEvents(HandleInput, &NumInputs);
		if (NumInputs != 0)
		{
			DWORD InputsRead = 0;

			INPUT_RECORD RecordInput[3];
			ReadConsoleInput(HandleInput, RecordInput, NumInputs, &InputsRead);
			for (DWORD i = 0; i < InputsRead; i++)

			{


				switch (RecordInput[i].Event.KeyEvent.wVirtualKeyCode)
				{
				case 0x41:
					//x -= vx;
					pressed[0] = RecordInput[i].Event.KeyEvent.bKeyDown;	//L
					break;
				case 0x57:
					//y -= vy;
					pressed[1] = RecordInput[i].Event.KeyEvent.bKeyDown;	//U
					break;
				case 0x44:
					//x += vx;
					pressed[2] = RecordInput[i].Event.KeyEvent.bKeyDown;	//R
					break;
				case 0x53:
					//y += vy;
					pressed[3] = RecordInput[i].Event.KeyEvent.bKeyDown;	//D
					break;
				case VK_LEFT:
					//x -= vx;
					pressed[4] = RecordInput[i].Event.KeyEvent.bKeyDown;	//S_L
					break;
				case VK_UP:
					//y -= vy;
					pressed[5] = RecordInput[i].Event.KeyEvent.bKeyDown;	//S_U
					break;
				case VK_RIGHT:
					//x += vx;
					pressed[6] = RecordInput[i].Event.KeyEvent.bKeyDown;	//S_R
					break;			
				case VK_DOWN:
					//y += vy;
					pressed[7] = RecordInput[i].Event.KeyEvent.bKeyDown;	//S_D
					break;
				case VK_SPACE:
					pressed[8] = RecordInput[i].Event.KeyEvent.bKeyDown;	//A_S
					break;
				case 0x45:
				case VK_NUMPAD0:
					pressed[9] = RecordInput[i].Event.KeyEvent.bKeyDown;
					break;

				}
			}
		}

		erase(hdc);
		if (pressed[0] && collision[0] == false)
			x -= vx;
		if (pressed[1] && collision[1] == false)
			y -= vy;
		if (pressed[2] && collision[2] == false)
			x += vx;
		if (pressed[3] && collision[3] == false)
			y += vy;

		draw(hdc);
	}


	void init(int x, int y, int size, int vx, int vy, int health) 
	{
		this->x = x;
		this->y = y;
		this->size = size;
		this->vx = vx;
		this->vy = vy;
		this->health = health;
	}


	void Collision(int xc1, int xc2, int yc1, int yc2) 
	{
		if ((x - vx) + size/2 - xc1 <= xc2 - xc1 + size && xc2 - (x - vx) + size/2 <= xc2 - xc1 + size && yc2 - y + size/2 <= yc2 - yc1 + size && y + size / 2 - yc1 <= yc2 - yc1 + size) //left
			collision[0] = true;
		if (x + size / 2 - xc1 <= xc2 - xc1 + size && xc2 - x + size/2 <= xc2 - xc1 + size && yc2 - (y - vy) + size / 2 <= yc2 - yc1 + size && (y - vy) + size / 2 - yc1 <= yc2 - yc1 + size) //up
			collision[1] = true;
		if ((x + vx) + size/2 - xc1 <= xc2 - xc1 + size && xc2 - (x + vx) + size/2 <= xc2 - xc1 + size && yc2 - y + size/2 <= yc2 - yc1 + size && y + size / 2 - yc1 <= yc2 - yc1 + size) //right
			collision[2] = true;
		if (x + size / 2 - xc1 <= xc2 - xc1 + size && xc2 - x + size / 2 <= xc2 - xc1 + size && yc2 - (y + vy) + size / 2 <= yc2 - yc1 + size && (y + vy) + size / 2 - yc1 <= yc2 - yc1 + size) //down
			collision[3] = true;
	}


	int gethealth() {
		return health;
	}


	void Fire(HDC hdc, vector<Projectile>* PlayerPr);


	bool CheckArea(vector<Enemy*>::iterator it, message *msg);

	bool CheckArea(vector<Projectile*>::iterator it, message* msg);
};


void PlayerShip::Fire(HDC hdc, vector<Projectile>* PlayerPr) 
{

	if (trig)
		trig--;
	if (pressed[4] && trig == 0) 
	{
		{
			PlayerPr->push_back(Projectile());
			vector<Projectile>::iterator it;
			it =PlayerPr->end() - 1;
			it->init(x - size / 2, y, size / 2, -4, 0, 5);
			trig = 35;
		}
	}
	if ((pressed[5] || pressed[8]) && trig == 0)
	{
		{
			PlayerPr->push_back(Projectile());
			vector<Projectile>::iterator it;
			it = PlayerPr->end() - 1;
			it->init(x, y - size / 2, size / 2, 0, -4, 5);
			trig = 35;
		}
	}
	if (pressed[6] && trig == 0)
	{
		{
			PlayerPr->push_back(Projectile());
			vector<Projectile>::iterator it;
			it = PlayerPr->end() - 1;
			it->init(x + size / 2, y, size / 2, 4, 0, 5);
			trig = 35;
		}
	}
	if (pressed[7] && trig == 0)
	{
		{
			PlayerPr->push_back(Projectile());
			vector<Projectile>::iterator it;
			it = PlayerPr->end() - 1;
			it->init(x, y + size / 2, size / 2, 0, 4, 5);
			trig = 35;
		}
	}

}


bool PlayerShip::CheckArea(vector<Enemy*>::iterator it, message* msg)
{
	(*it)->WhoAmI(msg);
	if (msg->values[2] == CIRC)
	{
		if (sqrt((x - (*it)->getx()) * (x - (*it)->getx()) + (y - (*it)->gety()) * (y - (*it)->gety())) < (size + (*it)->getsize()) / 2)
		{
			if(msg->values[3] <= 0)
			health -= (*it)->getsize();
			return true;
		}
		else
			return false;
	}
	else
	{
		if (sqrt((x - ((*it)->getx() - (*it)->getsize() / 2)) * (x - ((*it)->getx() - (*it)->getsize() / 2)) + (y - ((*it)->gety() - (*it)->getsize() / 2)) * (y - ((*it)->gety() - (*it)->getsize() / 2))) < size ||
			sqrt((x - ((*it)->getx() + (*it)->getsize() / 2)) * (x - ((*it)->getx() + (*it)->getsize() / 2)) + (y - ((*it)->gety() + (*it)->getsize() / 2)) * (y - ((*it)->gety() + (*it)->getsize() / 2))) < size ||
			sqrt((x - ((*it)->getx() + (*it)->getsize() / 2)) * (x - ((*it)->getx() + (*it)->getsize() / 2)) + (y - ((*it)->gety() - (*it)->getsize() / 2)) * (y - ((*it)->gety() - (*it)->getsize() / 2))) < size ||
			sqrt((x - ((*it)->getx() - (*it)->getsize() / 2)) * (x - ((*it)->getx() - (*it)->getsize() / 2)) + (y - ((*it)->gety() + (*it)->getsize() / 2)) * (y - ((*it)->gety() + (*it)->getsize() / 2))) < size)
		{
			if (msg->values[3] <= 0)
			health -= (*it)->getsize();
			return true;
		}
		else
			return false;
	}
}


bool PlayerShip::CheckArea(vector<Projectile*>::iterator it, message* msg)
{
	(*it)->WhoAmI(msg);
	if (msg->values[2] == CIRC)
	{
		if (sqrt((x - (*it)->getx()) * (x - (*it)->getx()) + (y - (*it)->gety()) * (y - (*it)->gety())) < (size + (*it)->getsize()) / 2)
		{
			if (msg->values[3] <= 0)
			health -= (*it)->getdamage();
			return true;
		}
		else
			return false;
	}
	else
	{
		if (sqrt((x - ((*it)->getx() - (*it)->getsize() / 2)) * (x - ((*it)->getx() - (*it)->getsize() / 2)) + (y - ((*it)->gety() - (*it)->getsize() / 2)) * (y - ((*it)->gety() - (*it)->getsize() / 2))) < size / 2 ||
			sqrt((x - ((*it)->getx() + (*it)->getsize() / 2)) * (x - ((*it)->getx() + (*it)->getsize() / 2)) + (y - ((*it)->gety() + (*it)->getsize() / 2)) * (y - ((*it)->gety() + (*it)->getsize() / 2))) < size / 2 ||
			sqrt((x - ((*it)->getx() + (*it)->getsize() / 2)) * (x - ((*it)->getx() + (*it)->getsize() / 2)) + (y - ((*it)->gety() - (*it)->getsize() / 2)) * (y - ((*it)->gety() - (*it)->getsize() / 2))) < size / 2 ||
			sqrt((x - ((*it)->getx() - (*it)->getsize() / 2)) * (x - ((*it)->getx() - (*it)->getsize() / 2)) + (y - ((*it)->gety() + (*it)->getsize() / 2)) * (y - ((*it)->gety() + (*it)->getsize() / 2))) < size / 2)
		{
			if (msg->values[3] <= 0)
			health -= (*it)->getdamage();
			return true;
		}
		else
			return false;
	}
}


class BattleGround 
{

public:
	void HealthBar(HDC hdc,int health)
	{
		HPEN hPen = CreatePen(PS_INSIDEFRAME, 2, RGB(0, 0, 0));
		HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
		MoveToEx(hdc, XMinGround-wallsize/2, YMaxGround + 20, NULL);
		LineTo(hdc, (XMinGround + ((XMaxGround - XMinGround))), YMaxGround + 20);
		if (health > 0)
		{
			hPen = CreatePen(PS_INSIDEFRAME, 2, RGB(100, 255, 20));
			hOldPen = (HPEN)SelectObject(hdc, hPen);
			MoveToEx(hdc, XMinGround-wallsize/2, YMaxGround + 20, NULL);
			LineTo(hdc, (XMinGround + ((XMaxGround - XMinGround) / MaxLife * health)), YMaxGround + 20);
			DeleteObject(hPen);
		}
		DeleteObject(hPen);
	}


	void Clear(HDC hdc)
	{
		HPEN hPen = CreatePen(PS_INSIDEFRAME, YMaxGround-YMinGround, RGB(0, 0, 0));
		HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
		Rectangle(hdc, XMinGround, YMaxGround, XMaxGround, YMinGround);
		DeleteObject(hPen);
	}


	void StartGame() 
	{
		srand(time(NULL));
		int seed = rand() % 8+2;
		int tick = 0, j = 0, trig = 0, sleep = 10, shift = 0, invulnerability = 0;
		
		HWND hconwnd = GetConsoleWindow();
		HDC hdc = GetDC(hconwnd);
		

		PlayerShip Player;
		vector<Projectile*> EnemyPr;
		vector<Projectile> PlayerPr;

		Wall wall[walls];

		vector<Enemy*> enemy;

		Player.init(XMinGround + (XMaxGround - XMinGround) / 2, YMinGround + (YMaxGround - YMinGround) / 4 * 3, 14, 2, 2, MaxLife);
		wall[0].init(XMinGround-wallsize/2, YMinGround + (YMaxGround - YMinGround) / 2, wallsize, YMaxGround-YMinGround+wallsize);
		wall[1].init(XMinGround + (XMaxGround - XMinGround) / 2, YMinGround, XMaxGround - XMinGround, wallsize);
		wall[2].init(XMaxGround,YMinGround + (YMaxGround-YMinGround)/2, wallsize, YMaxGround-YMinGround+wallsize);
		wall[3].init(XMinGround + (XMaxGround - XMinGround) / 2, YMaxGround, XMaxGround-XMinGround, wallsize);
		
		do
		{
			srand(time(NULL));
			for(short i=0;i<walls;i++)
			Player.Collision(wall[i].getcorner("x1"), wall[i].getcorner("x2"), wall[i].getcorner("y1"), wall[i].getcorner("y2"));

			Player.ObjectMove(hdc);

			memset(collision, 0, sizeof(collision));

			for (vector<Projectile*>::iterator it = EnemyPr.begin();it!=EnemyPr.end();it++)
			{
				if (sqrt((Player.getx() - (*it)->getx()) * (Player.getx() - (*it)->getx()) + (Player.gety() - (*it)->gety()) * (Player.gety() - (*it)->gety())) < (Player.getsize() + (*it)->getsize()))
					if (invulnerability == 0)
					{
						if (Player.CheckArea(it,&msg))
						{
							(*it)->erase(hdc);

							EnemyPr.erase(it);
							invulnerability = 140;
							msg.values[3] = invulnerability;
							break;
						}
					}
					else
						if (Player.CheckArea(it, &msg))
						{
							(*it)->erase(hdc);
							EnemyPr.erase(it);
							break;
						}
			}

			for (vector<Enemy*>::iterator it = enemy.begin(); it != enemy.end(); it++)
			{
				if (shift < enemy.size() - 1)
					shift++;
				if (sqrt((Player.getx() - (*it)->getx()) * (Player.getx() - (*it)->getx()) + (Player.gety() - (*it)->gety()) * (Player.gety() - (*it)->gety())) < (Player.getsize() + (*it)->getsize()))
					if (invulnerability == 0)
					{
						if (Player.CheckArea(it, &msg))
						{
							(*it)->sethealth((*it)->gethealth() - Player.getsize());
							invulnerability = 140;
							msg.values[3] = invulnerability;
						}
					}
					else
						if (Player.CheckArea(it, &msg))
						{
							(*it)->sethealth((*it)->gethealth() - Player.getsize());
						}
				if ((*it)->gethealth() <= 0)
				{
					(*it)->erase(hdc);
					enemy.erase(it);
					msg.code = NULL;
					shift--;
					if (enemy.size() > 0)
					{
						it = enemy.begin();
						advance(it, shift);
					}
					else
						break;
				}
			}

			shift = 0;

			for (vector<Projectile>::iterator it = PlayerPr.begin(); it != PlayerPr.end(); it++)
			{
				it->ObjectMove(hdc, &msg);
				if (shift < PlayerPr.size() - 1)
				shift++;
				if (msg.code == PROJ_DELETE)
				{
					PlayerPr.erase(it);
					msg.code = NULL;
					shift--;
					if (PlayerPr.size() > 0) 
					{
						it = PlayerPr.begin();
						advance(it, shift);
					}
					else
						break;
				}				
			}

			shift = 0;

			for (vector<Enemy*>::iterator iter = enemy.begin(); iter != enemy.end(); iter++) 
			{
				int shift_pr = 0;
				if (shift < enemy.size() - 1)
					shift++;
				for (vector<Projectile>::iterator it = PlayerPr.begin(); it != PlayerPr.end(); it++) 
				{
					if (shift_pr < PlayerPr.size() - 1)
						shift_pr++;
					if ((*iter)->CheckArea(hdc, &msg, it))
					{
						PlayerPr.erase(it);
						shift_pr--;
						if (PlayerPr.size() > 0) 
						{
							it = PlayerPr.begin();
							advance(it, shift_pr);
						}
						else
							if (msg.code == ENEM_DELETE)
							{
								(*iter)->erase(hdc);
								enemy.erase(iter);
								msg.code = NULL;
								shift--;
								if (enemy.size() > 0) 
								{
									iter = enemy.begin();
									advance(iter, shift);
									break;
								}
								else
									break;
							}
							else
								break;
					}
					if (msg.code == ENEM_DELETE)
					{
						(*iter)->erase(hdc);
						enemy.erase(iter);
						msg.code = NULL;
						shift--;
						if (enemy.size() > 0) 
						{
							iter = enemy.begin();
							advance(iter, shift);
						}
						else
							break;
					}
				}
				msg.values[0] = Player.getx(); msg.values[1] = Player.gety();
				(*iter)->Fire(hdc, &EnemyPr, &msg);
				(*iter)->ObjectMove(hdc, &msg);
				if (msg.code == ENEM_DELETE)
				{
					(*iter)->erase(hdc);
					enemy.erase(iter);
					msg.code = NULL;
					shift--;
					if (enemy.size() > 0) 
					{
						iter = enemy.begin();
						advance(iter, shift);
					}
					else
						break;
				}
			}
			
			shift = 0;

			for (vector<Projectile*>::iterator it = EnemyPr.begin(); it != EnemyPr.end(); it++)
			{
				(*it)->ObjectMove(hdc, &msg);
				if(shift<EnemyPr.size()-1)
				shift++;
				if (msg.code == PROJ_DELETE)
				{
					EnemyPr.erase(it);
					msg.code = NULL;	
					shift--;

					if (EnemyPr.size() > 0) 
					{
						it = EnemyPr.begin();
						advance(it, shift);
					}
					else
						break;
				}
			}
			
			shift = 0;

			Player.Fire(hdc, &PlayerPr);

			for (short i = 0; i < walls; i++)
				wall[i].draw(hdc);
			if (pressed[9]) {
				if (tick > 800)
				{
					sleep = 40;
					tick = 200;
				}
			}

				if(enemy.size()<enemycount)
					switch (rand() % 3) 
					{
						case 0:
							{
							j++;
							enemy.push_back(new EnemyPersuer);
							vector<Enemy*>::iterator it;
							it = enemy.end() - 1;
							(*it)->SetRandomSeed(j, seed);
							break; 
							}
						case 1:
							{
							j++;
							enemy.push_back(new EnemyCharger);
							vector<Enemy*>::iterator it;
							it = enemy.end() - 1;
							(*it)->SetRandomSeed(j, seed);
							break; 
							}
						case 2:
						{
							j++;
							enemy.push_back(new EnemyBiter);
							vector<Enemy*>::iterator it;
							it = enemy.end() - 1;
							(*it)->SetRandomSeed(j, seed);
							break;
						}
					}

				HealthBar(hdc, Player.gethealth());


			Sleep(sleep);
			if (sleep == 40)
				tick--;
			else
				tick++;
			if (tick == 0)
				sleep = 10;
			if (invulnerability > 0)
			{
				Player.draw(hdc, RGB(200, 255, 130));
				--invulnerability;
				msg.values[3] = invulnerability;
			}
		} while (Player.gethealth()>0);
		//printf_s("%d", EnemyPr.capacity());
		Player.draw(hdc);
		PlayerPr.clear();
		EnemyPr.clear();
		enemy.clear();
		memset(&msg, 0, sizeof(msg));
		printf_s("Your score: %d\n", Score);
	}

};


int _tmain(int argc, _TCHAR* argv[])

{
	BattleGround Battle;
	char c;
	do 
	{
		c = _getch();
		if (c == 'r')
		{
			HWND hconwnd = GetConsoleWindow();
			HDC hdc = GetDC(hconwnd);
			Battle.Clear(hdc);
			memset(pressed, 0, sizeof(pressed));
			Score = 0;
			Battle.StartGame();
		}
	} while (c != 'y');
	return 0;
}