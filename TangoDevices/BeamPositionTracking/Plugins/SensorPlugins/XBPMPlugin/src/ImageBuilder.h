//=============================================================================
// ImageBuilder.h
//=============================================================================
// abstraction.......To build an image of a beam at a certain location 
// class.............ImageBuilder
// original author...Thiam- SOLEIL
//=============================================================================

#ifndef _XBPM_BUILDER_H_
#define _XBPM_BUILDER_H_

#include <iostream>
#include <vector>


namespace ImageBuilder
{

class ImageBuilder 
{

	public :
		ImageBuilder(int xLength, int yLength, int beamSize);

		
		std::vector<unsigned char> getNewImage(double xBeamCenter, double yBeamCenter);

	private :

		int m_xLength;
		int m_yLength;
		int m_beamSize;
};

}

#endif // _XBPM_BUILDER_H_