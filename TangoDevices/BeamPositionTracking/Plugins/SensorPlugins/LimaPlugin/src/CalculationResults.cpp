//*****************************************************************************
//* Copyright 2010 - 2013, Thales Optronique SAS - All rights reserved        *
//*****************************************************************************
//
//=============================================================================
// file :        CalculationResults.h
//
// description : Implementation file for the BeamInfo class
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

#include "CalculationResults.h"
#include"opencv/cv.h"

CalculationResults::CalculationResults(unsigned short bit_depth)
{
	reset();
}

CalculationResults::~CalculationResults()
{
	//delete m_dataBinMutex;
}

void CalculationResults::reset()
{
	m_beamShapefound = false;
	m_centroidCenterX = 0;
	m_centroidCenterY = 0;
	m_barycenterCenterX = 0;
	m_barycenterCenterY = 0;
	m_localMaxX = 0;
	m_localMaxY = 0;
	m_sumMasked = 0;
	m_sumTotal = 0;
	m_largestRes.clear();
	m_beamRectangle = cv::Rect(0,0,0,0);
	m_imgHigh = 1024;
	m_imgWidth = 1024;
}
