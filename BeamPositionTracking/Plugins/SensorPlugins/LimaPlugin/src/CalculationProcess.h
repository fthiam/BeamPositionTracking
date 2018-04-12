//*****************************************************************************
//* Copyright 2010 - 2013, Thales Optronique SAS - All rights reserved        *
//*****************************************************************************
//
//=============================================================================
// file :        CalculationProcess.h
//
// description : Include file for the CalculationProcess class
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

#ifndef CalculationProcess_H
#define CalculationProcess_H

#include "BeamInfo.h"
#include "CalculationResults.h"

#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "opencv2/highgui/highgui.hpp"
/**
 *  CalculationProcess class description
 */

namespace CalculationProcess
{

class CalculationProcess
{
public:

	/**
	 * Constructs a newly task object.
	 *
	 */
	CalculationProcess(Tango::DeviceProxy *deviceCCD, double percentageDetection);
	
	~CalculationProcess(void);

	bool refreshBeam();
	/**
	 *	Method processBeam
	 *	Description: retrieve the result of task
	 *
	 */
	CalculationResults processBeam();

	bool checkDetectorPixelDepth();

	void updatePercentageDetection(double newPercentageDetection);

private:
	
	std::string type2str(int type);

	bool ensureCCDDeviceIsRunnning();

	void calculBeam();

	Tango::DeviceProxy *m_deviceCCD;

	CalculationResults *_calculationResults;

	CalculationResults *_refCalculationResults;

	BeamInfo _beamInfo;

};

}
#endif // CalculationProcess_H