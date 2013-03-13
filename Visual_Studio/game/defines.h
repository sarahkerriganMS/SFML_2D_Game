#ifndef DEFINES_H_
#define DEFINES_H_

/// Erkennung des Betriebssystems
#if defined(_WIN32) || defined(__WIN32__)
	#define SYS_WINDOWS
#elif defined(__APPLE__) || defined(MACOSX) || defined(macintosh) || defined(Macintosh) 
    #define SYS_MACOS
#elif defined(linux) || defined(__linux)
	#define SYS_LINUX
#endif

/// DEBUG wird vom Compiler gesetzt. DEBUGINFO kann hier bei Bedarf auskommentiert werden.
#ifdef DEBUG
	#define DEBUGINFO
#endif

/// Bilder pro Sekunde (Frames per Second) werden auf 60 Limitiert
#define FPS 60U

/// Version des Spiels.
#define VERSION "Beta 0.9.6"

/// Fenster Titel
#ifdef DEBUGINFO
	#define WINDOWTITLE "The Journey - " VERSION " Build - " __DATE__ " " __TIME__ " - DEBUG Mode is enabled"
#else
	#define WINDOWTITLE "The Journey - " VERSION " Build - " __DATE__ " " __TIME__
#endif

/// Pfad zum /include Ordner
#ifdef SYS_MACOS
    #ifdef DEBUGINFO
        #define PATH "/Users/Kimiko/Documents/Git/SFML_2D_Game/Visual_Studio/game/"
    #else
        #define PATH resourcePath() +
    #endif
#else
    #define PATH ""
#endif

/// für menu.cpp
#define BUTTONWIDTH 704
#define BUTTONHEIGHT 64
#define GENDERBUTTON 128
#define OPTIONWIDTH 100
#define OPTIONHEIGHT 50
#define CONTROLLERAXISMENUE 75.f

/// für map.cpp und player.cpp
#define TILESIZE 32

/// Formeln für Schaden, Hp, Exp von Monstern und Spielern
#define FDMG (int)( 25.f*pow(1.10776f,(float)(this->Lvl-5)) )
#define FHPMAX (int)( 50.f*pow(1.2023913f,(float)(this->Lvl-5)) )
#define FEXPMAX (int)( 250.f*pow(1.1424142f,(float)(this->Lvl-5)) )
#define FMONDMG (int)( 5.f*pow(1.12496f,(float)(this->Lvl-5)) )
#define FMONHP (int)( 50.f*pow(1.124953f,(float)(this->Lvl-5)) )
#define FMONEXP (int)( 10.f*pow(1.08543f,(float)(this->Lvl-5)) )
#define FHPREG (int)( 2.5f*pow(1.14242f,(float)(this->Lvl-5)) )

/// für player.cpp
#define MAXLEVEL 50
#define PLAYERSPEED 0.2f
#define COLLISIONTOLERANCE 4
#define ANIMATIONSPEED 2.8f         // Mehr = langsamere Animation
#define CONTROLLERTOLERANCE 25.f    // Analogstick nie komplett in der 0 possition
#define IDLEHEAL 85					// 
#define COOLDOWN 200

/// für monster.cpp
#define HOLDTIME 5
#define MOVETIME 0.5f
#define MOVETOLLERANCE (TILESIZE/2)
#define DMGRANGE 1
#define DETECTIONRADIUS 5
#define LOSTRADIUS 20

/// für savegame.cpp - Config - Standardeinstellungen
#define SETTINGS "settings.cfg"
#define DEFAULT_WIDTH 1024
#define DEFAULT_HEIGHT 768
#define DEFAULT_WINMODE "window"
#define DEFAULT_SOUND 1 // true

/// für savegame.cpp - Config - Gamepad Tastenbelegung
#ifdef SYS_WINDOWS
	#define DEFAULT_A 0
	#define DEFAULT_B 1
	#define DEFAULT_X 2
	#define DEFAULT_Y 3
	#define DEFAULT_LB 4
	#define DEFAULT_RB 5
	#define DEFAULT_BACK 6
	#define DEFAULT_START 7
	#define DEFAULT_LS 8
	#define DEFAULT_RS 9
#else
	#define DEFAULT_A 0
	#define DEFAULT_B 1
	#define DEFAULT_X 2
	#define DEFAULT_Y 3
	#define DEFAULT_LB 4
	#define DEFAULT_RB 5
	#define DEFAULT_BACK 9
	#define DEFAULT_START 8
	#define DEFAULT_LS 6
	#define DEFAULT_RS 7
#endif

/// für savegame.cpp - Savegame
#ifdef DEBUGINFO
    #define SAVEGAME "savegame_debug.bin"
    #define CHECKSUM 0
#else
    #define SAVEGAME "savegame.bin"
    #define CHECKSUM 4815162342
#endif
#define DEFAULT_HEALTH 23
#define DEFAULT_LVL 1
#define DEFAULT_EXP 0
#define DEFAULT_F_NAME "Ashley"
#define DEFAULT_M_NAME "Fabi"	// in memories
#define DEFAULT_LEVEL "map1"
#define DEFAULT_POSX 1008
#define DEFAULT_POSY 640

#endif