#include <graphics/Color.h>
#include <sstream>

namespace graphics
{
	Color::Color() : red(0), green(0), blue(0), alpha(255)
	{
	}
	
	Color::Color( unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255 ) : red(r), green(g), blue(b), alpha(a)
	{
	}
	
	Color::Color( const string& hexstring ) : red(0), green(0), blue(0), alpha(255)
	{
		this->parseHexString( hexstring );
	}
	
	Color::~Color()
	{
	}
	
	void Color::parseHexString( const string& hexstring )
	{
		
	}
	
	unsigned char Color::getRed() const
	{
		return this->red;
	}
	
	unsigned char Color::getGreen() const
	{
		return this->green;
	}
	
	unsigned char Color::getBlue() const
	{
		return this->blue;
	}
	
	unsigned char Color::getAlpha() const
	{
		return this->alpha;
	}
	
	string Color::getHexString( bool printAlpha ) const
	{
		stringstream ss;
		ss.setf( std::ios::hex, std::ios::basefield );
		ss << static_cast<int>( this->red ) << static_cast<int>( this->green ) << static_cast<int>( this->blue );
		
		if( printAlpha )
			ss << static_cast<int>( this->alpha );
		
		return ss.str();
	}
	
	void Color::setRed( unsigned char r )
	{
		this->red = r;
	}
	
	void Color::setGreen( unsigned char g )
	{
		this->green = g;
	}
	
	void Color::setBlue( unsigned char b )
	{
		this->blue = b;
	}
	
	void Color::setAlpha( unsigned char a )
	{
		this->alpha = a;
	}
	
	void Color::setColor( const string& hexstring )
	{
		this->parseHexString( hexstring );
	}
}
