#include <graphics/Font.h>
#include <data/parser/NodeParser.h>

#ifdef DEBUG0
#include <iostream>
#endif

#include <fstream>
#include <sstream>
#include <string>

using namespace std;
using namespace data;

namespace graphics
{
	map<string,Font*> Font::fonts;
	
	Font::Font() : image(NULL)
	{
	}
	
	Font::~Font()
	{
		if( this->image != NULL )
			delete this->image;
	}
            
	bool Font::load( const string& filename )
	{
		return Font::load( filename.c_str() );
	}
	
	bool Font::load( const char * filename )
	{
		Font * font = new Font();
		string fontName = "";
		bool success = true;
		
		#ifdef DEBUG0
		cout << "[Font#" << font << "] Loading file \"" << filename << "\"..." << endl;
		#endif
		
		ifstream file( filename );
		
		if( file.is_open() )
		{
			stringstream ss;
			ss << file.rdbuf();
			
			parser::NodeParser nParser( ss.str() );
			node::Node * root = nParser.parse();
			
			if( root != NULL )
			{
				node::Node * nFont = root->find( "font" );
				
				if( nFont != NULL )
				{
					if( nFont->hasAttr( "name" ) )
					{
						fontName = nFont->attr( "name" );
						
						#ifdef DEBUG0
						cout << "[Font#" << font << "] Font name is: \"" << fontName << "\"." << endl;
						#endif
						
						if( nFont->hasAttr( "sprite" ) )
						{
							font->image = new Sprite( nFont->attr( "sprite" ) );
						
							if( font->image->isLoaded() )
							{
								// Browse characters
								node::Node * character = nFont->childAt( 0 );
								
								while( character != NULL )
								{
									if( character->getType() == node::Node::Tag && character->getName() == "char" )
									{
										Sprite::Frame frame;
										
										string value = character->hasAttr( "value" ) ? character->attr( "value" ) : "";
										frame.x = character->isIntegerAttr( "x" ) ? character->integerAttr( "x" ) : 0;
										frame.y = character->isIntegerAttr( "y" ) ? character->integerAttr( "y" ) : 0;
										frame.width = character->isIntegerAttr( "width" ) ? character->integerAttr( "width" ) : 0;
										frame.height = character->isIntegerAttr( "height" ) ? character->integerAttr( "height" ) : 0;
										
										if( value.length() > 0 )
										{
											font->characters[value.at(0)] = frame;
											#ifdef DEBUG0
											cout << "[Font#" << font << "] Character \"" << value.at(0) << "\" is at (" << frame.x << "," << frame.y << ") and of size (" << frame.width << "," << frame.height << ")." << endl;
											#endif
										}
									}
									
									character = character->next();
								}
							}
							else
							{
								#ifdef DEBUG0
								cout << "[Font#" << font << "] Unable to load sprite \"" << nFont->attr( "sprite" ) << "\"." << endl;
								#endif
							
								success = false;
							}
						}
						else
						{
							#ifdef DEBUG0
							cout << "[Font#" << font << "] Unable to find \"sprite\" attribute in <font> tag in \"" << filename << "\"." << endl;
							#endif
						
							success = false;
						}
					}
					else
					{
						#ifdef DEBUG0
						cout << "[Font#" << font << "] Unable to find \"name\" attribute in <font> tag in \"" << filename << "\"." << endl;
						#endif
						
						success = false;
					}
					
				}
				else
				{
					#ifdef DEBUG0
					cout << "[Font#" << font << "] Unable to find <font> tag in \"" << filename << "\"." << endl;
					#endif
					
					success = false;
				}
				
				
				delete root;
			}
			else
			{
				#ifdef DEBUG0
				cout << "[Font#" << font << "] Unable to parse file \"" << filename << "\"." << endl;
				#endif
				success = false;
			}
			
			file.close();
		}
		else
		{
			#ifdef DEBUG0
			cout << "[Font#" << font << "] Unable to load file \"" << filename << "\"." << endl;
			#endif
			
			success = false;
		}
		
		if( success )
			Font::fonts[fontName] = font;
			
		else if( font != NULL )
			delete font;
		
		return success;
	}
	
	Font * Font::get( const string& name )
	{
		Font * font = NULL;
		map<string,Font *>::iterator it = Font::fonts.find( name );
		
		if( it != Font::fonts.end() )
			font = it->second;
		
		return font;
	}
	
	Font * Font::get( const char * name )
	{
		return Font::get( string( name ) );
	}
	
	void Font::destroy( const string& name )
	{
		map<string,Font *>::iterator it = Font::fonts.find( name );
		
		if( it != Font::fonts.end() )
		{
			delete it->second;
			Font::fonts.erase( it );
		}
	}
	
	void Font::destroy( const char * name )
	{
		Font::destroy( string( name ) );
	}
			
	void Font::render( int x, int y, const string& text )
	{
		this->render( x, y, text.c_str() );
	}
	
	void Font::render( int x, int y, const char * text )
	{
		int i = 0;
		SDL_Rect dstRect;
		dstRect.x = x;
		dstRect.y = y;
		dstRect.w = 0;
		dstRect.h = 0;
		
		SDL_Rect srcRect;
		srcRect.x = 0;
		srcRect.y = 0;
		srcRect.w = 0;
		srcRect.h = 0;
		
		while( text[i] != 0 )
		{
			map<char,Sprite::Frame>::iterator it = this->characters.find( text[i] );
			
			if( it != this->characters.end() )
			{
				srcRect.x = it->second.x;
				srcRect.y = it->second.y;
				srcRect.w = it->second.width;
				srcRect.h = it->second.height;
				
				dstRect.w = it->second.width;
				dstRect.h = it->second.height;
				
				SDL_RenderCopy( Screen::get()->getRenderer(), this->image->getTexture(), &srcRect, &dstRect );
				
				dstRect.x += dstRect.w;
			}
			else if( text[i] == '\n' )
				dstRect.y += dstRect.h;
			
			i++;
		}
	}
	
	void Font::renderSize( int * x, int * y, const string& text )
	{
		this->renderSize( x, y, text.c_str() );
	}
	
	void Font::renderSize( int * x, int * y, const char * text )
	{
		int i = 0;
		
		while( text[i] != 0 )
		{
			map<char,Sprite::Frame>::iterator it = this->characters.find( text[i] );
			
			if( it != this->characters.end() )
				x += it->second.width;

			else if( text[i] == '\n' )
				y += it->second.height;
			
			i++;
		}
	}
	
	int Font::renderWidth( int x, int y, const string& text )
	{
		this->renderSize( &x, &y, text.c_str() );
		return x;
	}
	
	int Font::renderWidth( int x, int y, const char * text )
	{
		this->renderSize( &x, &y, text );
		return x;
	}
	
	int Font::renderHeight( int x, int y, const string& text )
	{
		this->renderSize( &x, &y, text.c_str() );
		return y;
	}
	
	int Font::renderHeight( int x, int y, const char * text )
	{
		this->renderSize( &x, &y, text );
		return y;
	}
}
