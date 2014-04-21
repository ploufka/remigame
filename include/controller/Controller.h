#ifndef _CONTROLLER_CONTROLLER_H
#define	_CONTROLLER_CONTROLLER_H	1

#include <SDL2/SDL.h>
#include <controller/Mapping.h>

namespace controller
{
	class Controller;
}

#include <game/Player.h>

#include <map>
#include <string>

using namespace std;
using namespace game;

namespace controller
{
	class Controller
	{
		protected:
			static map<SDL_JoystickID, Controller *> controllers;
			static map<string, Mapping *> mappings;
			
			SDL_Joystick * joystick;
			SDL_JoystickID id;
			Mapping * mapping;
			
			Player * player;
			
			map<Mapping::Button, pair<short int, unsigned int> > states;
			
			Controller( SDL_Joystick * joystick );
			~Controller();
			
			void loadMapping( const char * name );
			void updateState( Mapping::Button button, short int value, unsigned int timestamp );
		
		public:
			static void scan();
			static void destroy();
			static void handleEvent( const SDL_Event * event );
			static void tickEvent( unsigned int timestamp );
			static unsigned int getControllersCount();
			static Controller * getFreeController();
			
			unsigned int getId();
			short int getState( Mapping::Button button );
			unsigned int getStateTimestamp( Mapping::Button button );
			void setPlayer( Player * player );
			Player * getPlayer();
	};
};

#endif

