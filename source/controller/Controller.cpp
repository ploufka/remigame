#include <controller/Controller.h>
#include <data/parser/NodeParser.h>

#ifdef DEBUG0
#include <iostream>
#endif

#include <vector>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;
using namespace data;
using namespace game;

namespace controller
{
	map<SDL_JoystickID, Controller *> Controller::controllers;
	map<string, Mapping *> Controller::mappings;
	
	Controller::Controller( SDL_Joystick * joystick ) : joystick(joystick), id(0), mapping(NULL), player(NULL)
	{
		this->id = SDL_JoystickInstanceID( this->joystick );
		this->loadMapping( SDL_JoystickName( this->joystick ) );
		
		// Initialize buttons states
		this->states[Mapping::BTNUP] = make_pair( Mapping::STATE_RELEASED, 0 );
		this->states[Mapping::BTNRIGHT] = make_pair( Mapping::STATE_RELEASED, 0 );
		this->states[Mapping::BTNDOWN] = make_pair( Mapping::STATE_RELEASED, 0 );
		this->states[Mapping::BTNLEFT] = make_pair( Mapping::STATE_RELEASED, 0 );
		this->states[Mapping::LT1] = make_pair( Mapping::STATE_RELEASED, 0 );
		this->states[Mapping::LT2] = make_pair( Mapping::STATE_RELEASED, 0 );
		this->states[Mapping::LT3] = make_pair( Mapping::STATE_RELEASED, 0 );
		this->states[Mapping::RT1] = make_pair( Mapping::STATE_RELEASED, 0 );
		this->states[Mapping::RT2] = make_pair( Mapping::STATE_RELEASED, 0 );
		this->states[Mapping::RT3] = make_pair( Mapping::STATE_RELEASED, 0 );
		this->states[Mapping::SELECT] = make_pair( Mapping::STATE_RELEASED, 0 );
		this->states[Mapping::START] = make_pair( Mapping::STATE_RELEASED, 0 );
		this->states[Mapping::AXH] = make_pair( Mapping::STATE_RELEASED, 0 );
		this->states[Mapping::AXV] = make_pair( Mapping::STATE_RELEASED, 0 );
		
		#ifdef DEBUG0
		cout << "[Controller#" << this->id << "] Initialized." << endl;
		cout << "[Controller#" << this->id << "] Name: " << SDL_JoystickName( this->joystick ) << endl;
		#endif
	}
	
	Controller::~Controller()
	{
		if( this->player != NULL )
			this->player->setController( NULL );
		
		if( this->joystick != NULL )
		{
			SDL_JoystickClose( this->joystick );
			
			#ifdef DEBUG0
			cout << "[Controller#" << this->id << "] Closed." << endl;
			#endif
		}
	}
	
	unsigned int Controller::getId()
	{
		return static_cast<unsigned int>( this->id );
	}
	
	void Controller::scan()
	{
		#ifdef DEBUG0
		cout << "[Controller] Looking for available controllers..." << endl;
		#endif
	
		SDL_Joystick * joystick = NULL;

		for( int i = 0 ; i < SDL_NumJoysticks() ; i++ )
		{
			joystick = SDL_JoystickOpen( i );
		
			if( joystick != NULL )
			{
				// Only if the controller is not already initialized
				SDL_JoystickID controllerId = SDL_JoystickInstanceID( joystick );
				map<SDL_JoystickID, Controller *>::iterator itCtrl = Controller::controllers.find( controllerId );
				
				if( itCtrl == Controller::controllers.end() )
					Controller::controllers[controllerId] = new Controller( joystick );
			}
			#ifdef DEBUG0
			else
			{
				cout << "[Controller] Can not open #" << i << ": " << SDL_GetError() << endl;
			}
			#endif
		}
		
		#ifdef DEBUG0
		cout << "[Controller] Controllers loaded: " << Controller::controllers.size() << "." << endl;
		#endif
	}
	
	void Controller::destroy()
	{
		for( map<SDL_JoystickID, Controller *>::iterator itCtrl = Controller::controllers.begin() ; itCtrl != Controller::controllers.end() ; )
		{
			delete itCtrl->second;
			Controller::controllers.erase( itCtrl++ );
		}
		
		for( map<string, Mapping *>::iterator itMapping = Controller::mappings.begin() ; itMapping != Controller::mappings.end() ; )
		{
			delete itMapping->second;
			Controller::mappings.erase( itMapping++ );
		}
	}
	
	void Controller::loadMapping( const char * name )
	{
		#ifdef DEBUG0
		cout << "[Controller#" << this->id << "] Loading file \"data/controller/mappings.xml\"..." << endl;
		#endif
		
		ifstream file( "data/controller/mappings.xml" );
		
		if( file.is_open() )
		{
			stringstream ss;
			ss << file.rdbuf();
			
			parser::NodeParser nParser( ss.str() );
			node::Node * root = nParser.parse();
			
			if( root != NULL )
			{
				node::Node * nMappings = root->find( "mappings" );
				
				if( nMappings != NULL )
				{
					string mapping = "";
					node::Node * nMapping = nMappings->childAt( 0 );
					
					while( nMapping != NULL )
					{
						if( nMapping->getType() == node::Node::Tag && nMapping->getName() == "mapping" )
						{
							if( nMapping->hasAttr( "name" ) && nMapping->hasAttr( "file" ) )
							{
								// Loading generic mapping by default
								if( mapping == "" && nMapping->attr( "name" ) == "generic" )
								{
									mapping = nMapping->attr( "file" );
								}
								
								// Loading matching mapping
								else if( nMapping->attr( "name" ).compare( 0, strlen( name ), name ) == 0 )
								{
									mapping = nMapping->attr( "file" );
									break;
								}
							}
						}
						
						nMapping = nMapping->next();
					}
					
					if( mapping != "" )
					{
						map<string, Mapping *>::iterator itMapping = Controller::mappings.find( mapping );
						
						if( itMapping == Controller::mappings.end() )
						{
							Mapping * m = new Mapping( mapping );
							
							if( m->isLoaded() )
							{
								this->mapping = m;
								Controller::mappings[mapping] = m;
							}
							else
							{
								delete m;
								this->mapping = NULL;
							}
						}
						else
							this->mapping = itMapping->second;
					}
					else
					{
						this->mapping = NULL;

						#ifdef DEBUG0
						cout << "[Controller#" << this->id << "] Unable to find a suitable mapping." << endl;
						#endif
					}
				}
				else
				{
					#ifdef DEBUG0
					cout << "[Controller#" << this->id << "] Unable to find \"mappings\" tag." << endl;
					#endif
				}
			}
			else
			{
				#ifdef DEBUG0
				cout << "[Controller#" << this->id << "] Unable to parse file." << endl;
				#endif
			}
		}
		else
		{
			#ifdef DEBUG0
			cout << "[Controller#" << this->id << "] Unable to open file \"data/controller/mappings.xml\"." << endl;
			#endif
		}
	}
	
	void Controller::tickEvent( unsigned int timestamp )
	{
		for( map<SDL_JoystickID, Controller *>::iterator itController = Controller::controllers.begin() ; itController != Controller::controllers.end() ; itController++ )
		{
			if( itController->second != NULL && itController->second->player != NULL )
				itController->second->player->handleEvent( Mapping::NOBTN, Mapping::STATE_RELEASED, timestamp );
		}
	}
	
	void Controller::handleEvent( const SDL_Event * event )
	{
		map<SDL_JoystickID, Controller *>::iterator itController = Controller::controllers.end();
		
		switch( event->type )
		{
			case SDL_JOYBUTTONUP:
			{
				itController = Controller::controllers.find( event->jbutton.which );
				
				if( itController != Controller::controllers.end() )
				{
					if( itController->second->mapping != NULL )
					{
						itController->second->updateState( itController->second->mapping->getButtonFromButton( event->jbutton.button ), Mapping::STATE_RELEASED, event->jbutton.timestamp );
					}
				}
				
				break;
			}
			
			case SDL_JOYBUTTONDOWN:
			{
				itController = Controller::controllers.find( event->jbutton.which );
				
				if( itController != Controller::controllers.end() )
				{
					if( itController->second->mapping != NULL )
					{
						itController->second->updateState( itController->second->mapping->getButtonFromButton( event->jbutton.button ), Mapping::STATE_PUSHED, event->jbutton.timestamp );
					}
				}
				
				break;
			}
			
			case SDL_JOYAXISMOTION:
			{
				itController = Controller::controllers.find( event->jbutton.which );
				
				if( itController != Controller::controllers.end() )
				{
					itController->second->updateState( itController->second->mapping->getButtonFromAxis( event->jaxis.axis ), event->jaxis.value, event->jaxis.timestamp );
				}
				
				break;
			}
			
			case SDL_JOYDEVICEREMOVED:
			{
				itController = Controller::controllers.find( event->jbutton.which );
				
				if( itController != Controller::controllers.end() )
				{
					delete itController->second;
					Controller::controllers.erase( itController );
				}
				
				break;
			}
		}
	}
	
	void Controller::updateState( Mapping::Button button, short int value, unsigned int timestamp )
	{
		if( this->player != NULL )
			this->player->handleEvent( button, value, timestamp );

		this->states[button] = make_pair( value, timestamp );
		//cout << "[Controller#" << this->id << "] Button#" << button << " set with value " << static_cast<int>( value ) << "." << endl;
	}
	
	short int Controller::getState( Mapping::Button button )
	{
		short int value = Mapping::STATE_RELEASED;
		map<Mapping::Button, pair<short int, unsigned int> >::iterator it = this->states.find( button );
		
		if( it != this->states.end() )
			value = it->second.first;
		
		return value;
	}
	
	unsigned int Controller::getStateTimestamp( Mapping::Button button )
	{
		unsigned int timestamp = 0;
		map<Mapping::Button, pair<short int, unsigned int> >::iterator it = this->states.find( button );
		
		if( it != this->states.end() )
			timestamp = it->second.second;
		
		return timestamp;
	}
	
	unsigned int Controller::getControllersCount()
	{
		return Controller::controllers.size();
	}
	
	Controller * Controller::getFreeController()
	{
		Controller * controller = NULL;
		
		for( map<SDL_JoystickID, Controller *>::iterator it = Controller::controllers.begin() ; it != Controller::controllers.end() ; it++ )
		{
			if( it->second->player == NULL )
			{
				controller = it->second;
				break;
			}
		}
		
		return controller;
	}
	
	void Controller::setPlayer( Player * player )
	{
		if( this->player != player )
		{
			if( this->player != NULL )
				this->player->setController( NULL );
			
			this->player = player;
		}
	}
}
