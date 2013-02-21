#include "game.h"

void Game::Init(void)
{
	// Do it!
	Savegame::currentSaveGame = new Savegame;
	ConfigFile::currentConfigFile = new ConfigFile;

	// Lade die Einstellungen des Spiels.. 
	ConfigFile::currentConfigFile->loadConfigFile();
	
	// Pr�fung fehtl noch ob der ORdner schon vorhanden ist @fil
	if(!system("mkdir screenshots"))
		throw "Failed to create the screenshot folder!";

	if(Savegame::currentSaveGame->loadSavegame(true)){
		Game::Start();
	}else{
		Game::Start(true);
	}
}

void Game::Start(bool newgame)
{
	// Wenn der Spielstatus uninitalisiert, verlasse die Methode
	if(_gameState != Uninitialized) return;
	
	// Erzeuge ein neues Fenster mit den in der defines.h hinterlegten Werten
	sf::VideoMode bpp = sf::VideoMode::getDesktopMode();
	
	_mainWindow.create(sf::VideoMode(ConfigFile::currentConfigFile->width, ConfigFile::currentConfigFile->height, bpp.bitsPerPixel), WINDOWTITLE, sf::Style::Close);

	// Deaktiviert den Mauszeiger im Fenster - Klicken geht weiterhin..
	//_mainWindow.setMouseCursorVisible(false);

	// Lade und setze das Fenstericon
	sf::Image Icon;
	if(!Icon.loadFromFile(PATH"include/interface/icon.bmp"))
		std::cout << "Failed to load the Windowicon" << std::endl;
	else
		_mainWindow.setIcon(32,32,Icon.getPixelsPtr());

	_mainWindow.setFramerateLimit(FPS);
	_mainWindow.setVerticalSyncEnabled(true);

	// Setze den Spielstatus auf Intro -> Intro wird angezeigt
	_gameState = Game::ShowingIntro;
	
	// Solange das Spiel nicht beendet wird, f�hre GameLoop aus
	while(!IsExiting()){
		GameLoop(newgame);
	}

	// Wenn der GameLoop beendet wurde, schlie�e das Fenster
	_mainWindow.close();
	//system("pause");
}

bool Game::IsExiting()
{
	// Wenn der Spielstatus auf Beenden gesetzt wird dann gebe ein True zur�ck, ansonsten ein False
	if(_gameState == Game::Exiting){
		
		delete Savegame::currentSaveGame;
		delete ConfigFile::currentConfigFile;
		
		return true;
	}else{ 
		return false;
	}
	// M�glichkeit das Spiel zu Speichern
}

void Game::GameLoop(bool newgame)
{
	sf::View viewCamera  = _mainWindow.getView();
	char gender;

	switch(_gameState){
		case Game::ShowingIntro:
			#ifdef DEBUGINFO
				std::cout << "Show the Intro" << std::endl;
			#endif
			ShowIntro();
		break;
		case Game::ShowingMenu:
			#ifdef DEBUGINFO
				std::cout << "Show the Menu" << std::endl;
			#endif
			ShowMenu(newgame);
            break;
        case Game::ShowingGenderMenu:
			#ifdef DEBUGINFO
				std::cout << "Show the Gender Menu" << std::endl;
			#endif			
            gender = ShowMenuGender();
			std::cout << gender ;
			if(gender == 'M' || gender == 'F')	// somit wird kein neuer spielstand erzeugt, wenn man den zur�ck button im gender men� dr�ckt!
				Savegame::currentSaveGame->saveSavegame(gender,true);
		break;
		case Game::Playing:
			// Hier wird die Map geladen
			#ifdef DEBUGINFO
				std::cout << "Show the Map / Level" << std::endl;
			#endif
			ShowMap(viewCamera);				
            break;
		case Game::NewGame:
			ShowMap(viewCamera);
			break;
		case Game::Paused:
			GamePaused(viewCamera);
			break;
        default:
            break;
	}
}
void Game::GamePaused(sf::View viewCamera){
	Pause PauseMenu;
	bool quitGame = PauseMenu.Show(_mainWindow, viewCamera);
	if(quitGame)
		_gameState = Exiting;
	else
		_gameState = Playing;

	// hier kann ich zur�ck ins men�!
}

void Game::ShowMap(sf::View viewCamera){
	static Map map;
	Map::currentMap = &map;
	

	map.Init(_mainWindow, Savegame::currentSaveGame->mLevelId, viewCamera);
	MapEvent newGameState = map.Show(_mainWindow, Savegame::currentSaveGame->mLevelId, viewCamera);

	
	
	if(newGameState.theReason == MapEvent::pause)
		_gameState = Paused;
	else if(newGameState.theReason == MapEvent::exiting)
		_gameState = Exiting;
	else if(newGameState.theReason == MapEvent::dead){
	//	Savegame::currentSaveGame->loadSavegame();
		_gameState = ShowingMenu;
	}else if(newGameState.theReason == MapEvent::mapchange){
		Map::currentMap->getPlayer()->setLevelId(newGameState.newMapId);
		Map::currentMap->getPlayer()->setPosition(newGameState.newMapPosX,newGameState.newMapPosY);
		
		std::cout << "PlayerLevelId" << Map::currentMap->getPlayer()->getLevelId() << std::endl;
		Savegame::currentSaveGame->saveSavegame();
		
		
	}
	
	
}

void Game::ShowIntro(){
	Intro intro;
	intro.Show(_mainWindow);
	_gameState = Game::ShowingMenu;
}


void Game::ShowMenu(bool newgame){
	MainMenu mainMenu;
	MainMenu::MenuResult result = mainMenu.Show(_mainWindow, newgame);
	//Savegame::currentSaveGame->loadSavegame();
	switch(result)
	{
		case MainMenu::Exit:
			#ifdef DEBUGINFO
				std::cout << "Menu -> Exit Button pressed" << std::endl;
			#endif
				_gameState = Exiting;
			break;
		case MainMenu::Continue:
			#ifdef DEBUGINFO
				std::cout << "Menu -> Continue Button pressed " << std::endl;	
			#endif
				_gameState = Playing;
			break;
        case MainMenu::NewGameGender:
			#ifdef DEBUGINFO
				std::cout << "Menu -> New Game Button pressed " << std::endl;	
			#endif
            _gameState = ShowingGenderMenu;
        default:
            break;  // was soll passieren wenn man daneben klickt? - RICHTIG! NICHTS VERDAMMT!
	}
}

const char Game::ShowMenuGender(){
    MainMenu genderMenu;
    MainMenu::MenuResult result = genderMenu.Show(_mainWindow, false, true);
    switch (result) {
        case MainMenu::Female:
            _gameState = NewGame;
			return 'F';									
            break;                 
        case MainMenu::Male:        
            _gameState = NewGame;   
			return 'M';
			break;
        case MainMenu::Menue:
            _gameState = ShowingMenu;
			return 'X';			// Gebe X Zur�ck damit nicht gespeichert wird, wenn man nicht direkt m�chte (new game)
            break;
		default:
            break; 
    }
    return 'M';
}

// static member variables need to be instantiated outside of the class
Game::GameState Game::_gameState = Uninitialized;
sf::RenderWindow Game::_mainWindow;