/**
	Verantwortlich: Daniel Maier, Filip Menke <br>
	Infotext: Die Klasse Sound spielt die Ingame Musik ab.
	Sofern in der Konfigurationsdatei settings.cfg der Sound aktiviert (Sound = True) 
	ist wird ein Sound im Men� und den einzelnen Maps abgespielt. In der Pause wird die
	Soundausgabe pausiert. Weiterhin erfolgt kein Sound im Intro.
	Wird der Sound deaktiviert erfolgt keine Soundausgabe im gesamten Spiel / Men�s.

*/
#include "sounds.h"

gameMusic::gameMusic(){
	//Konstruktor	
};
gameMusic::~gameMusic(){
	//Destruktor
};

sf::Music gameMusic::music;