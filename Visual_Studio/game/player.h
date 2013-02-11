#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <iostream>
#include <SFML/Graphics.hpp>
#include "defines.h"
#include "savegame.h"


class Player{
public:
	void Update		(sf::RenderWindow &Window, float ElapsedTime);
	void Render		(sf::RenderWindow &Window){
		Window.draw(sprite);
	}
	Player			(bool gender, sf::IntRect*** CollisionMap, Savegame& currentSavegame);
	float			getPosX(void){
		return this->x;
	}
	float			getPosY(void){
		return this->y;
	}
	float			getSpeed(void){
		return this->Speed;
	}
	void			increaseSpeed(float speedValue){
		if(this->Speed < 0.35f)
			this->Speed += speedValue;
	}
	void			decreaseSpeed(float speedValue){
		if(this->Speed <= 0.2f)
			this->Speed = 0.1f;
		else
			this->Speed -= speedValue;
	}
	void setHealth(int pHealth){
		this->pHealth = pHealth;
	}
	void setLvl(int pLvl){
		this->pLvl = pLvl;
	}
	void setExp(int pExp){
		this->pExp = pExp;
	}
	void setName(char const * pName){
		strcpy(this->pName,pName);
	}
	int getHealth(void){
		return this->pHealth;
	}
	int getLvl(void){
		return this->pLvl;
	}
	int getExp(void){
		return this->pExp;
	}
	char * getName(void){
		return this->pName;
	}
	void setMapSize( int xMax, int yMax){
		this->MapSize.x = xMax;
		this->MapSize.y = yMax;
	}
private:
	float Speed;
	sf::Texture texture;
	sf::Sprite sprite;
	sf::IntRect*** ColMap;
	sf::Vector2i MapSize;
	float x;
	float y;
	unsigned char Animation;

	int pHealth;
	int pLvl;
	int pExp;
	bool gender;	// 0 - male | 1 - female
	static char pName[11] ;
	
	/*
	
	Anfangswert von Leben auf = 10; -> max_life = 10;
	Beim Speichern wird der Wert gespeichert und beim laden wieder neu gesetzt
	wenn das Level steigt erh�ht sich auch das Maximale Leben

	current_life, max_life, 
	exp, level


	*/
};

#endif