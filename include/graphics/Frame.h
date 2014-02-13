#ifndef __GRAPHICS_FRAME_H
#define __GRAPHICS_FRAME_H	1

#include <graphics/Point.h>
#include <graphics/Box.h>

#include <vector>
using namespace std;

namespace graphics
{
	class Frame
	{
		protected:
			Box box;
			Point anchor;
			vector<Box *> boundingBoxes;
			vector<Box *> attackAreas;
			vector<Box *> defenceAreas;
		
		public:
			Frame();
			Frame( const Frame& frame );
			Frame( const Box& box );
			~Frame();
			
			Box& getBox();
			Point& getAnchor();
			
			int getBoundingBoxCount() const;
			int getAttackAreasCount() const;
			int getDefenceAreasCount() const;
			
			const Box * getBoundingBox( int index ) const;
			const Box * getAttackArea( int index ) const;
			const Box * getDefenceArea( int index ) const;
			
			void addBoundingBox( const Box& box );
			void addAttackArea( const Box& box );
			void addDefenceArea( const Box& box );
	};
}

#endif
