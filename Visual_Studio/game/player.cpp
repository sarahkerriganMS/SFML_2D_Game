﻿/**
	Infotext: Dies ist die Klasse für die steuerbare Spielfigur.
    Die Spielfigur wird einmalig Initialisiert und dann während des Gameloops immer
    aktualisiert.
    Der Spieler hat verschiedene Interaktionsmöglichkeiten.
    Er kann sich durch Tastatur oder Gamepad eingabe auf der Map bewegen oder Monster
    schlagen.
    Ihm werden von der Map mehrere Infos mitgeteilt, welche er benötigt.
    z.B. die Kollisionsmap oder die Mapgröße.
    Die Kollisionsabfrage wird über 12 Kollisionspunkte realisiert, je nachdem wie der
    Zustand der Punkte ist.
*/
#include <math.h>
#include "savegame.h"
#include "player.h"

/// Der Konstruktor des Players. Er ruft die Init-Methode auf.
Player::Player(int controller){
    Init(controller);
}

/**
 Hier wird der Spieler initialisiert.
 Alle Variablen werden mit Standardwerten versehen.
 Sie bestimmt unter anderem das Aussehen des Spielers
 */
void Player::Init(int controller){
	
	this->Health = Savegame::currentSaveGame->pHealth;
	this->Lvl = Savegame::currentSaveGame->pLvl;
	this->pExp = Savegame::currentSaveGame->pExp;
	this->pGender = Savegame::currentSaveGame->pGender;
	this->Name = Savegame::currentSaveGame->pName;
	this->LevelId = Savegame::currentSaveGame->mLevelId;
    this->isInvincible = false;
    this->showSword = false;
    this->swordTimer = 0.f;
    
	this->pHealthMax = FHPMAX;//(int)(BASEHEALTH*pow(HEALTHMULTIPLICATOR,(float)(this->Lvl-1)));
	this->pExpMax = FEXPMAX;//(int)(BASEEXP*pow(EXPMULTIPLICATOR,(float)(this->Lvl-1)));
    this->AttackPower = FDMG;//(int)(BASEDMG*pow(DMGMULTIPLICATOR,(float)(this->Lvl-1)));

	this->controller = controller;
	this->blockControl = false;
	this->isAttacking = false;
	this->canAttack = true;
	this->lookDirection = 'D';

	sf::String tex;
	if(pGender == 'F'){         // spielertextur abhängig vom gewählten helden.
		tex.insert(0,PATH"include/texture/player/player_female.png");
	}else {
		tex.insert(0,PATH"include/texture/player/player_male.png");
	}

	if(!texture.loadFromFile(tex)){
		throw "Error: Playertexture not found.";
	}
	#ifdef DEBUGINFO
	else {
		std::cout << "Successfully loaded the player texture!" << std::endl;
	}
	#endif
	
	tex.clear();
	tex.insert(0,PATH"include/texture/player/weapon_sword.png");
	if(!weaponTexture.loadFromFile(tex)){
		throw "Error: Weapontexture not found.";
	}
	#ifdef DEBUGINFO
	else {
		std::cout << "Successfully loaded the weapon texture!" << std::endl;
	}
	#endif

	Speed = PLAYERSPEED;
	HealTickRate = 0;
	Animation = 0;

	sprite.setTexture(texture);
	sprite.setOrigin(TILESIZE/2,TILESIZE);
	sprite.setTextureRect(sf::IntRect(0,0,TILESIZE,TILESIZE*2));
	sprite.setPosition(Savegame::currentSaveGame->mPosX, Savegame::currentSaveGame->mPosY);

	weaponSprite.setTexture(weaponTexture);
	weaponSprite.setOrigin(TILESIZE/4,0);
	weaponSprite.setTextureRect(sf::IntRect(0,0,TILESIZE/4+1,TILESIZE));
	weaponSprite.setPosition(Savegame::currentSaveGame->mPosX, Savegame::currentSaveGame->mPosY+TILESIZE/2);
}

/**
 Auch der Spieler wird im Gameloop immer wieder aktualisiert.
 Sie macht den Spieler über die Tastatur und ein Gamepad steuerbar.
 Außerdem beinhaltet sie die Kollisionskontrolle und die verschiedenen Animationen des Spielers.
 Alle Interaktionsmöglichkeiten des Spielers wie z.B. das Angreifen von Monstern, werden hier implementiert.
 */
void Player::Update(float ElapsedTime){

	this->PosX = sprite.getPosition().x;
	this->PosY = sprite.getPosition().y;

	int tx = ((int)PosX/TILESIZE)-1;
	int ty = ((int)PosY/TILESIZE)-1;

	bool blockUp = false;
	bool blockDown = false;
	bool blockLeft = false;
	bool blockRight = false;

	// 12 Kollisionspunkte
	int CollisionPoint[12][2] = {
		{(int)PosX-TILESIZE/2, (int)PosY},										//  0: Links Oben						+--- Player ----+
		{(int)PosX+TILESIZE/2-1, (int)PosY},									//  1: Rechts Oben						|				|
		{(int)PosX-TILESIZE/2, (int)PosY+TILESIZE-1},							//  2: Links Unten						|				|
		{(int)PosX+TILESIZE/2-1, (int)PosY+TILESIZE-1},							//  3: Rechts Unten						|				| keine Kollisionsabfrage im oberen bereich.
		{(int)PosX-TILESIZE/2+COLLISIONTOLERANCE, (int)PosY},					//  4: Links Oben Hilfspunkt Oben		|				|
		{(int)PosX+TILESIZE/2-1-COLLISIONTOLERANCE, (int)PosY},					//  5: Rechts Oben Hilfspunkt Oben		|				|
		{(int)PosX-TILESIZE/2+COLLISIONTOLERANCE, (int)PosY+TILESIZE-1},		//  6: Links Unten Hilfspunkt Unten		| 00 04   05 01 |
		{(int)PosX+TILESIZE/2-1-COLLISIONTOLERANCE, (int)PosY+TILESIZE-1},		//  7: Rechts Unten Hilfspunkt Unten	| 08         09 |
		{(int)PosX-TILESIZE/2, (int)PosY+COLLISIONTOLERANCE},					//  8: Links Oben Hilfspunkt Links		|				|
		{(int)PosX+TILESIZE/2-1, (int)PosY+COLLISIONTOLERANCE},					//  9: Rechts Oben Hilfspunkt Rechts	| 10         11 |
		{(int)PosX-TILESIZE/2, (int)PosY+TILESIZE-1-COLLISIONTOLERANCE},		// 10: Links Unten Hilfspunkt Links		| 02 06   07 03 |
		{(int)PosX+TILESIZE/2-1, (int)PosY+TILESIZE-1-COLLISIONTOLERANCE},		// 11: Rechts Unten Hilfspunkt Rechts	+---------------+

	};

	bool cp[12] = {0,0,0,0,0,0,0,0,0,0,0,0};    // alle kollisionspunkte sind erstmal false

	for( int i=0; i<9; i++){
		if(tx+(i%3) >= 0 && ty+i/3 >= 0 && tx+(i%3) < MapSize.x && ty+i/3 < MapSize.y && ColMap[tx+(i%3)][ty+i/3] != NULL){
			for( int p=0; p<12; p++){
				if( ColMap[tx+(i%3)][ty+i/3]->contains(CollisionPoint[p][0],CollisionPoint[p][1]) ){
					cp[p] = 1;
				}
			}       // setze alle kollisionspunkte auf 1, welche eine kollision mit der welt haben.
		}
	}

	for( int i=0; i<12; i++){	// Punktabfrage sollte jetzt passen. Fall zu untersuchen: 0 !4 !8 und dann gegen die ecke laufen...
		if( ((cp[0] && cp[4]) || (cp[1] && cp[5])) && (!cp[8] || !cp[9]) ){ // Bei Normaler Auslastung
			blockUp = true;
		}
		else if( ((cp[2] && cp[6]) || (cp[3] && cp[7])) && (!cp[10] || !cp[11]) ){
			blockDown = true;
		}
		if( ((cp[0] && cp[8]) || (cp[2] && cp[10])) && (!cp[4] || !cp[6]) ){
			blockLeft = true;
		}
		else if( ((cp[1] && cp[9]) || (cp[3] && cp[11])) && (!cp[5] || !cp[7]) ){
			blockRight = true;
		}
		if( (cp[0] && cp[4] && cp[8] && !cp[1] && !cp[2])||(cp[0] && cp[1] && cp[2]) ){ // bei hoher Auslastung
			blockUp = true;
			blockLeft = true;
		}
		else if( (cp[1] && cp[5] && cp[9] && !cp[0] && !cp[3])||(cp[1] && cp[0] && cp[3]) ){
			blockUp = true;
			blockRight = true;
		}
		else if( (cp[2] && cp[6] && cp[10] && !cp[0] && !cp[3])||(cp[2] && cp[0] && cp[3]) ){
			blockDown = true;
			blockLeft = true;
		}
		else if( (cp[3] && cp[7] && cp[11] && !cp[1] && !cp[2])||(cp[3] && cp[1] && cp[2]) ){
			blockDown = true;
			blockRight = true;
		}
		else if( cp[0] && cp[1] && !cp[2] && !cp[3] ){
			blockUp = true;
		}
		else if( cp[2] && cp[3] && !cp[0] && !cp[1] ){
			blockDown = true;
		}
		else if( cp[0] && cp[2] && !cp[1] && !cp[3] ){
			blockLeft = true;
		}
		else if( cp[1] && cp[3] && !cp[0] && !cp[2] ){
			blockRight = true;
		}
		else if( cp[0] && cp[1] && cp[2] && cp[3] ){
			std::cout << "KOLLISION ÜBERALL" << std::endl;	// teleport an sicheren ort
		}
	}

	char moveInput = 'n';		// moveInput: // n = nothing // s = stick // d = dpad // k = keyboard
	bool walking = false;
	//std::cout << sf::Joystick::getAxisPosition(controller,sf::Joystick::PovX) << " " << sf::Joystick::getAxisPosition(controller,sf::Joystick::PovY) << std::endl;
	if(!blockControl){
		if( sf::Joystick::isConnected(controller) ){    // analoge axen des controllers nur nutzen, falls auch einer angeschlossen ist
			if( (sf::Joystick::getAxisPosition(controller,sf::Joystick::Y) < -CONTROLLERTOLERANCE) && !blockUp && (moveInput == 'n' || moveInput == 's') ){
				moveInput = 's';
				PosY += (Speed*ElapsedTime*sf::Joystick::getAxisPosition(controller,sf::Joystick::Y)/100);
				blockDown = false;
				walking = true;
			}
			else if( (sf::Joystick::getAxisPosition(controller,sf::Joystick::Y) > CONTROLLERTOLERANCE) && !blockDown && (moveInput == 'n' || moveInput == 's') ){
				moveInput = 's';
				PosY += (Speed*ElapsedTime*sf::Joystick::getAxisPosition(controller,sf::Joystick::Y)/100);
				blockUp = false;
				walking = true;
			}
			if( (sf::Joystick::getAxisPosition(controller,sf::Joystick::X) < -CONTROLLERTOLERANCE) && !blockLeft && (moveInput == 'n' || moveInput == 's') ){
				moveInput = 's';
				PosX += (Speed*ElapsedTime*sf::Joystick::getAxisPosition(controller,sf::Joystick::X)/100);
				blockRight = false;
				walking = true;
			}
			else if( (sf::Joystick::getAxisPosition(controller,sf::Joystick::X) > CONTROLLERTOLERANCE) && !blockRight && (moveInput == 'n' || moveInput == 's') ){
				moveInput = 's';
				PosX += (Speed*ElapsedTime*sf::Joystick::getAxisPosition(controller,sf::Joystick::X)/100);
				blockLeft = false;
				walking = true;
			}
			if(moveInput == 's'){
				float xTemp = sf::Joystick::getAxisPosition(controller,sf::Joystick::X);
				float yTemp = sf::Joystick::getAxisPosition(controller,sf::Joystick::Y);
				if (xTemp <0.f) xTemp = -xTemp;
				if (yTemp <0.f) yTemp = -yTemp;
				if( xTemp > yTemp ){
					if( sf::Joystick::getAxisPosition(controller,sf::Joystick::X) < -CONTROLLERTOLERANCE){
						if(!isAttacking) lookDirection = 'L';
					}
					else if (sf::Joystick::getAxisPosition(controller,sf::Joystick::X) > CONTROLLERTOLERANCE){
						if(!isAttacking) lookDirection = 'R';
					}
				}
				else {
					if( sf::Joystick::getAxisPosition(controller,sf::Joystick::Y) < -CONTROLLERTOLERANCE){
						if(!isAttacking) lookDirection = 'U';
					}
					else if (sf::Joystick::getAxisPosition(controller,sf::Joystick::Y) > CONTROLLERTOLERANCE){
						if(!isAttacking) lookDirection = 'D';
					}
				}
			}

#ifdef SYS_WINDOWS
			if( (sf::Joystick::getAxisPosition(controller,sf::Joystick::PovX) > CONTROLLERTOLERANCE) && !blockUp && (moveInput == 'n' || moveInput == 'd') ){
				moveInput = 'd';
				PosY -= (Speed*ElapsedTime*sf::Joystick::getAxisPosition(controller,sf::Joystick::PovX)/100);
				blockDown = false;
				walking = true;
			}
			else if( (sf::Joystick::getAxisPosition(controller,sf::Joystick::PovX) < -CONTROLLERTOLERANCE) && !blockDown && (moveInput == 'n' || moveInput == 'd') ){
				moveInput = 'd';
				PosY -= (Speed*ElapsedTime*sf::Joystick::getAxisPosition(controller,sf::Joystick::PovX)/100);
				blockUp = false;
				walking = true;
			}
			if( (sf::Joystick::getAxisPosition(controller,sf::Joystick::PovY) < -CONTROLLERTOLERANCE) && !blockLeft && (moveInput == 'n' || moveInput == 'd') ){
				moveInput = 'd';
				PosX += (Speed*ElapsedTime*sf::Joystick::getAxisPosition(controller,sf::Joystick::PovY)/100);
				blockRight = false;
				walking = true;
			}
			else if( (sf::Joystick::getAxisPosition(controller,sf::Joystick::PovY) > CONTROLLERTOLERANCE) && !blockRight && (moveInput == 'n' || moveInput == 'd') ){
				moveInput = 'd';
				PosX += (Speed*ElapsedTime*sf::Joystick::getAxisPosition(controller,sf::Joystick::PovY)/100);
				blockLeft = false;
				walking = true;
			}
			if(moveInput == 'd'){
				float xTemp = sf::Joystick::getAxisPosition(controller,sf::Joystick::PovY);
				float yTemp = sf::Joystick::getAxisPosition(controller,sf::Joystick::PovX);
				if (xTemp <0.f) xTemp = -xTemp;
				if (yTemp <0.f) yTemp = -yTemp;
				if( xTemp > yTemp ){
					if( sf::Joystick::getAxisPosition(controller,sf::Joystick::PovY) < -CONTROLLERTOLERANCE){
						if(!isAttacking) lookDirection = 'L';
					}
					else if (sf::Joystick::getAxisPosition(controller,sf::Joystick::PovY) > CONTROLLERTOLERANCE){
						if(!isAttacking) lookDirection = 'R';
					}
				}
				else {
					if( sf::Joystick::getAxisPosition(controller,sf::Joystick::PovX) > CONTROLLERTOLERANCE){
						if(!isAttacking) lookDirection = 'U';
					}
					else if (sf::Joystick::getAxisPosition(controller,sf::Joystick::PovX) < -CONTROLLERTOLERANCE){
						if(!isAttacking) lookDirection = 'D';
					}
				}
			}
#endif

		}                       // hier nochmal die tastenabfragen. siehe menu.cpp ganz unten für tastenbelegung
		bool directionKey[4] = {false,false,false,false};
		if( (sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
			 sf::Keyboard::isKeyPressed(sf::Keyboard::Up) ||
			 sf::Joystick::isButtonPressed(controller,11) )
		   && !(sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		   && !blockUp && (moveInput == 'n' || moveInput == 'k') ){
			moveInput = 'k';
			directionKey[0] = true;
			PosY -= Speed*ElapsedTime*((directionKey[2]||directionKey[3])?0.707107f:1.f);
			if(!isAttacking) lookDirection = 'U';
			blockDown = false;
			walking = true;
		}    
		if( (sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
			 sf::Keyboard::isKeyPressed(sf::Keyboard::Down) ||
			 sf::Joystick::isButtonPressed(controller,12))
		   && !(sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		   && !blockDown && (moveInput == 'n' || moveInput == 'k') ){
			moveInput = 'k';
			directionKey[1] = true;
			PosY += Speed*ElapsedTime*((directionKey[2]||directionKey[3])?0.707107f:1.f);
			if(!isAttacking) lookDirection = 'D';
			blockUp = false;
			walking = true;
		}
		if ( (sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
			  sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ||
			  sf::Joystick::isButtonPressed(controller,13))
			&& !(sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			&& !blockLeft && (moveInput == 'n' || moveInput == 'k') ){
			moveInput = 'k';
			directionKey[2] = true;
			PosX -= Speed*ElapsedTime*((directionKey[0]||directionKey[1])?0.707107f:1.f);
			if(!isAttacking) lookDirection = 'L';
			blockRight = false;
			walking = true;
		}
		if( (sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
			 sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ||
			 sf::Joystick::isButtonPressed(controller,14))
		   && !(sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
   		   && !blockRight && (moveInput == 'n' || moveInput == 'k') ){
			moveInput = 'k';
			directionKey[3] = true;
            PosX += Speed*ElapsedTime*((directionKey[0]||directionKey[1])?0.707107f:1.f);
			if(!isAttacking) lookDirection = 'R';
			blockLeft = false;
			walking = true;
		}
        if( (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) ||
             sf::Joystick::isButtonPressed(controller,ConfigFile::currentConfigFile->controller_A))
           && canAttack ){
            canAttack = false;
            isAttacking = true;
        }
        else if( (!(sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) &&
                  !(sf::Joystick::isButtonPressed(controller,ConfigFile::currentConfigFile->controller_A)))
                && !canAttack ){
            canAttack = true;
            isAttacking = false;
        }
	}

	if(lookDirection == 'U'){
		sprite.setTextureRect(sf::IntRect(TILESIZE*((Animation/(int)((1/Speed)*ANIMATIONSPEED))%4+1),TILESIZE*2*1,TILESIZE,TILESIZE*2));
		//weaponSprite.setTextureRect(sf::IntRect(0,0,TILESIZE/4+1,TILESIZE));
		weaponSprite.setRotation(180);
	}
	else if(lookDirection == 'D'){
		sprite.setTextureRect(sf::IntRect(TILESIZE*((Animation/(int)((1/Speed)*ANIMATIONSPEED))%4+1),0,TILESIZE,TILESIZE*2));
		//weaponSprite.setTextureRect(sf::IntRect(0,0,TILESIZE/4+1,TILESIZE));
		weaponSprite.setRotation(0);
	}
	else if(lookDirection == 'L'){
		sprite.setTextureRect(sf::IntRect(TILESIZE*((Animation/(int)((1/Speed)*ANIMATIONSPEED))%4+1),TILESIZE*2*2,TILESIZE,TILESIZE*2));
		//weaponSprite.setTextureRect(sf::IntRect(TILESIZE/4+1,0,TILESIZE/4+1,TILESIZE));
		if(!showSword)weaponSprite.setRotation(90);
	}
	else {
		sprite.setTextureRect(sf::IntRect(TILESIZE*((Animation/(int)((1/Speed)*ANIMATIONSPEED))%4+1),TILESIZE*2*3,TILESIZE,TILESIZE*2));
		//weaponSprite.setTextureRect(sf::IntRect(TILESIZE/4+1,0,TILESIZE/4+1,TILESIZE));
		if(!showSword)weaponSprite.setRotation(270);
	}

	if( walking ){      // nur animieren wenn spieler läuft
		if( (Animation/(int)((1/Speed)*ElapsedTime*ANIMATIONSPEED)) >= 4) Animation = 0;
		Animation++;
		ResetCooldown();	// Cool Down Phase, erst danach werden die HP regeneriert
	}
	else {
		sprite.setTextureRect(sf::IntRect(0,sprite.getTextureRect().top,TILESIZE,TILESIZE*2));  // spieler "steht", wenn er sich nicht bewegt.
        if( HealTickRate >= IDLEHEAL ){
			HealTickRate = 0;
			this->playerHeal( FHPREG );     // Heilt den Player wenn er sich nicht bewegt.
		}
		//std::cout << HealTickRate << std::endl;
		HealTickRate++;
	}
    
    // falls das spiel lagt, hier korrekturen für die kollisionsabfrage.
	if( blockUp ){
		if( (TILESIZE-((int)PosY-(((int)PosY/TILESIZE)*TILESIZE))) > COLLISIONTOLERANCE ){
			PosY= (float)(((int)PosY/TILESIZE)*TILESIZE+TILESIZE-1);
			#ifdef DEBUGINFO
				std::cout << "Kolliskorrektur: OBEN - " << PosY << std::endl;
			#endif
		}
	}
	if( blockDown ){
		if( (((int)PosY+TILESIZE)-((((int)PosY+TILESIZE)/TILESIZE)*TILESIZE)) > COLLISIONTOLERANCE ){
			PosY= (float)((((int)PosY+TILESIZE)/TILESIZE)*TILESIZE-TILESIZE+1);
			#ifdef DEBUGINFO
				std::cout << "Kolliskorrektur: UNTEN - " << PosY << std::endl;
			#endif
		}
	}
	if( blockLeft ){
		if( (TILESIZE-(((int)PosX-TILESIZE/2)-((((int)PosX-TILESIZE/2)/TILESIZE)*TILESIZE))) > COLLISIONTOLERANCE ){
			PosX= (float)((((int)PosX-TILESIZE/2)/TILESIZE)*TILESIZE+TILESIZE*3/2-1);
			#ifdef DEBUGINFO
				std::cout << "Kolliskorrektur: LINKS - " << PosX << std::endl;
			#endif
		}
	}
	if( blockRight ){
		if( (((int)PosX+TILESIZE/2)-((((int)PosX+TILESIZE/2)/TILESIZE)*TILESIZE)) > COLLISIONTOLERANCE ){
			PosX= (float)((((int)PosX+TILESIZE/2)/TILESIZE)*TILESIZE-TILESIZE/2+1);
			#ifdef DEBUGINFO
				std::cout << "Kolliskorrektur: RECHTS - " << PosX << std::endl;
			#endif
		}
	}
	weaponSprite.setPosition(PosX,PosY);

	if(isAttacking){
        ResetCooldown();
		weaponDmgBox = weaponSprite.getGlobalBounds();
        weaponDmgBox.top += TILESIZE/2;
		for( int i=0; i < Map::currentMap->getMonsterCounter(); i++){
			Monster* mon = Map::currentMap->getMonsterList();
			if( mon[i].getHitBox().intersects(weaponDmgBox) ){
                showSword = true;
				isAttacking = false;
                switch(mon[i].getType()){
                    case 1:
                    case 2:
                        if(lookDirection=='L')weaponSprite.setRotation(90-35);
                        else if(lookDirection=='R')weaponSprite.setRotation(270+35);

                        break;
                    default:
                        break;
                }
                mon[i].damageMe(AttackPower,this->Lvl);
                mon[i].targetPlayer();

			}
		}
#ifdef DEBUGINFO
        drawSwordBox.setSize(sf::Vector2f(weaponDmgBox.width,weaponDmgBox.height));
        drawSwordBox.setPosition(weaponDmgBox.left, weaponDmgBox.top);
        drawSwordBox.setFillColor(sf::Color(255,255,255,75));
#endif
		//std::cout << "x: " << weaponDmgBox.left << " y: " << weaponDmgBox.top << " b: " << weaponDmgBox.width << " h: " << weaponDmgBox.height << std::endl;
	}

    if(showSword){
        swordTimer += ElapsedTime/10.f;
        if( swordTimer > 16.f){
            swordTimer = 0.f;
            showSword = false;
        }
    }

	sprite.setPosition(PosX,PosY);
}

/// Der Spieler wird hier gezeichnet. Wichtig ist hier die Reihenfolge, wann das Schwert hinter oder vor dem Player angezeigt wird.
void Player::Render(sf::RenderWindow &Window){
	if(lookDirection == 'D' || lookDirection == 'R') Character::Render(Window);
    if(isAttacking || showSword){
        Window.draw(weaponSprite);
    }
	if(lookDirection == 'U' || lookDirection == 'L') Character::Render(Window);
#ifdef DEBUGINFO
    if(isAttacking) Window.draw(drawSwordBox);
#endif
}

/**
 Methode für das zurücksetzen des Heal-Cooldowns.
 Wenn der Spieler mit dem Schwert Schlägt, von einem Monster angegriffen wird, oder sich bewegt, wird dieser zurückgesetzt, damit er nur im Stillstand geheilt wird.
 */
void Player::ResetCooldown(void){
    this->HealTickRate = -COOLDOWN + IDLEHEAL;
}

/// Wird der Spieler angegriffen, fügt diese Methode dem Spieler den Schaden zu.
void Player::playerDamage( int damage, int level ){
    ResetCooldown();
    if(!isInvincible){
        //isInvincible = true;
        damage = calcDamage(damage, level);
        damageText(damage,'p');
        this->Health -= damage;
        if( this->Health <= 0 ){
            this->Health = 0;
        }
    }
}

/// Diese Methode wird aufgerufen, wenn der Spieler geheilt wird.
void Player::playerHeal( int heal ){
    if( this->Health < this->pHealthMax ) damageText(heal,'h');
    this->Health += heal;
    if( this->Health > this->pHealthMax ){
        this->Health = this->pHealthMax;
    }
}

/// Diese Methode wird aufgerufen, wenn der Spieler Erfahrung bekommt (z.B. von Monstern).
void Player::playerExp( int exp, int level ){
    if( this->Lvl < MAXLEVEL ){
        int levelDif = level-this->Lvl;
        
        float multi = 1.f;
        if(levelDif > 0){   // Gegner hat einen höhreren Level
            if(levelDif >10) levelDif = 10;
            multi += (float)levelDif*0.005f;
        }
        else if(levelDif < 0){  // Gegner hat einen niedrigeren Level
            if(levelDif < -10) levelDif = -10;
            multi += (float)levelDif*0.1f;
        }
        exp = (int)((float)exp * multi);
        
        /*exp += levelDif*(level/2);
         if ( exp <= 0 ){
         exp = 1;
         }*/
        if(exp>0)damageText(exp,'e');
        this->pExp += exp;
        while( (this->pExp >= this->pExpMax) && (this->Lvl < MAXLEVEL) ){       // Hier levelup!
            this->pExp -= this->pExpMax;
            this->Lvl++;
            this->pHealthMax = FHPMAX;//(int)(BASEHEALTH*pow(HEALTHMULTIPLICATOR,(float)(this->Lvl-1)));
            this->pExpMax = FEXPMAX;//(int)(BASEEXP*pow(EXPMULTIPLICATOR,(float)(this->Lvl-1)));
            this->AttackPower = FDMG;//(int)(BASEDMG*pow(DMGMULTIPLICATOR,(float)(this->Lvl-1)));
            this->Health = this->pHealthMax;
            damageText(this->Lvl,'l');
        }
		if(this->pExp >= this->pExpMax) this->pExp = this->pExpMax-1;
    }
}

