#include "schrift.h"
#include "defines.h"

bool Schrift::FontLoaded = false;
sf::Font Schrift::font;

Schrift::Schrift(int X, int Y, sf::String myText, int size, int color){
	
	if( !FontLoaded ){
		if(!Schrift::font.loadFromFile("include/fonts/arial.ttf")){
			std::cout << "Fehler beim Laden der Schrift!\a" << std::endl;
		}
		else {
			FontLoaded = true;
			#ifdef DEBUG
				std::cout << "Font geladen!" << std::endl;
			#endif
		}
	}

	printText.setString(myText);
	printText.setFont(this->font);
	printText.setCharacterSize(size);
	printText.setColor(sf::Color(color,color,color));
	printText.setPosition((float)X,(float)Y);
}

void Schrift::Update(sf::String myText){
	printText.setString(myText);
}

void Schrift::Render(sf::RenderWindow &Window){
	Window.draw(printText);
}
