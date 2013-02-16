#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>
#include <math.h>
#include "defines.h"
#include "savegame.h"


class Player{
public:
	void Update		(float ElapsedTime);
	void Render		(sf::RenderWindow &Window){
		Window.draw(sprite);
	}
	Player			(sf::IntRect*** CollisionMap, Savegame& currentSavegame, int controller=0);
	float			getPosX(void){
		return this->x;
	}
	float			getPosY(void){
		return this->y;
	}
	void			setPosition(float PosX, float PosY){
		sprite.setPosition(PosX,PosY);
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
	void setName(std::string pName){
		this->pName = pName;
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
	char getGender(void){
		return this->pGender;
	}
	std::string getName(void){
		return this->pName;
	}
	int getHealthMax(void){
		return this->pHealthMax;
	}
	int getExpMax(void){
		return this->pExpMax;
	}
	void setMapSize( int xMax, int yMax){
		this->MapSize.x = xMax;
		this->MapSize.y = yMax;
	}
    void playerDamage( int damage, int level ){
        int levelDif = level-this->pLvl;
        damage += (levelDif*(level/10));
        if ( damage <= 0 ){
            damage = 1;
        }
        this->pHealth -= damage;
        if( this->pHealth < 0 ){
            this->pHealth = 0;
			std::cout << "Player will be die!" << std::endl;
        }
    }
    void playerHeal( int heal ){
        this->pHealth += heal;
        if( this->pHealth > this->pHealthMax ){
            this->pHealth = this->pHealthMax;
        }
    }
    void playerExp( int exp, int level ){
        int levelDif = level-this->pLvl;
        exp += levelDif*(level/2);
        this->pExp += exp;
        if( this->pExp > this->pExpMax ){       // Hier levelup!
            #ifdef DEBUGINFO
                std::cout << "LEVEL UP!" << std::endl;
            #endif
            this->pExp -= this->pExpMax;
            this->pLvl++;
            this->pHealthMax = BASEHEALTH-HEALTHPERLEVEL+this->pLvl*HEALTHPERLEVEL;
			this->pHealth = this->pHealthMax;
            this->pExpMax = BASEEXP*(int)pow(EXPMULTIPLICATOR,(this->pLvl-1));
        }
    }
	void setBlockControl(bool block=false){
		this->blockControl = block;
		#ifdef DEBUGINFO
			std::cout << "block " << block << std::endl;
		#endif
	}
private:
    int controller;
	bool blockControl;
	float Speed;
	sf::Texture texture;
	sf::Sprite sprite;
	sf::IntRect*** ColMap;
	sf::Vector2i MapSize;
	float x;
	float y;
	int Animation;
	int HealTickRate;
	int pHealth;
	int pHealthMax;
	int pLvl;
	int pExp;
	int pExpMax;	// Exp needed Until Levelup
	char pGender;	// M - Male | F - Female
	std::string pName; 
};

#endif