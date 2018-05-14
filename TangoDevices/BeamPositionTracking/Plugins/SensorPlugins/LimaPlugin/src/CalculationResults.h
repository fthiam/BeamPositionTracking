//*****************************************************************************
//* Copyright 2010 - 2013, Thales Optronique SAS - All rights reserved        *
//*****************************************************************************
//
//=============================================================================
// file :        CalculationResults.h
//
// description : Include file for the CalculationResults class
//
// project :     Supervision
//
// $Author:  THALES OPTRONIQUE SAS
//
// $Revision:  $
// $Date:  $
//
// $HeadURL:  $
//
//=============================================================================
//=============================================================================

#ifndef CalculationResults_H
#define CalculationResults_H

#include "tango.h"
#include "BeamInfo.h"
// #include "core.hpp"
#include "opencv/cv.h"


/**
 *  CalculationResults class description:
 *	  Regroup the set of calculs computed in a Caluclation Task
 */
class CalculationResults
{
public:

	/**
	 * Constructor of results.
	 *
	 */
	CalculationResults(unsigned short bit_depth);
	~CalculationResults();

	/**
	 *	Method reset
	 *	Description: to reset the results
	 *
	 */ 
	void reset();

	// m_beamShapefound : flag for the beam presence
	bool m_beamShapefound;

	// m_centroidCenterX : centroid center position in X
	double m_centroidCenterX;

	// m_centroidCenterY : centroid center position in Y
	double m_centroidCenterY;

	// m_barycenterCenterX : barycenter center position in X
	short m_barycenterCenterX;

	// m_barycenterCenterY : barycenter center position in Y
	short m_barycenterCenterY;

	// m_localMaxX : maximum local position in X
	short m_localMaxX;

	// m_localMaxX : maximum local position in Y
	short m_localMaxY;

	// m_sumMasked : sum of masked image
	float m_sumMasked;

	// m_sumMasked : sum of orignial image
	unsigned long m_sumTotal;

	// m_largestRes : polygone of the largest detected centroid area
	std::vector<cv::Point> m_largestRes;

	// m_beamRectangle : bounding rectangle of the centroid largest contour
	CvRect m_beamRectangle;

	std::vector<unsigned char> thresholdedImg;

	int m_imgHigh;

	int m_imgWidth;

private :


};

#endif // CalculationResults_H