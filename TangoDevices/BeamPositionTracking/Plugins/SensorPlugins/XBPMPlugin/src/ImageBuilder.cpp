//=============================================================================
// ImageBuilder.cpp
//=============================================================================
// abstraction.......To build an image of a beam at a certain location 
// class.............ImageBuilder
// original author...thiam - SOLEIL
//=============================================================================

#include <ImageBuilder.h>
#include <math.h>

namespace ImageBuilder
{

// ============================================================================
// ImageBuilder::ImageBuilder
// ============================================================================
ImageBuilder::ImageBuilder(int xLength, int yLength, int beamSize)
{
	m_beamSize = beamSize;
	m_xLength = xLength;
	m_yLength = yLength;
}

// ============================================================================
// ImageBuilder::getNewImage
// ============================================================================
std::vector<unsigned char> ImageBuilder::getNewImage(double xBeamCenter, double yBeamCenter){

	int vectorSize = m_xLength * m_yLength;
	std::vector<unsigned char> newImage(vectorSize);
	int it = 0;

	for (int i = 0; i < m_xLength; i++){
		for (int j = 0; j < m_yLength; j++){
			double squareDistance = pow((double)i - xBeamCenter, (double)2);
			squareDistance += pow(((double)j - yBeamCenter), (double)2);
			if (squareDistance > m_beamSize){

				newImage.at(it) =  (unsigned char) 0;
			}
			else {


				newImage.at(it) =  (unsigned char) 1;
			}
			it++;
		}		
	}

	return newImage;
}

};