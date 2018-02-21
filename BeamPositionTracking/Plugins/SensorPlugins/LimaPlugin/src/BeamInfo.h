//*****************************************************************************
//* Copyright 2010 - 2013, Thales Optronique SAS - All rights reserved        *
//*****************************************************************************
//
//=============================================================================
// file :        BeamInfo.h
//
// description : Include file for the BeamInfo class
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

#ifndef BeamInfo_H
#define BeamInfo_H

#include "tango.h"
#include "opencv2/core/core.hpp"


#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "opencv2/highgui/highgui.hpp"


#ifdef _DEBUG
#define DEBUG_MODE 0
#else
#define DEBUG_MODE 0
#endif

#define TRACE_DEBUG(trace)\
	if (DEBUG_MODE)\
		std::cout << trace;

#define TRACE_RELEASE(trace)\
		//std::cout << trace;

/**
 * The IOError_e enum matches all values returned by the readFile and writeFile methods.
 */
typedef enum IOError_e
{
	OK,
	OPEN_FAIL,
	FILE_EXCEED_ATTRIBUT_SIZE
} IOError_e;

/*
* Different image types, match with bit depth 
*/
const unsigned short CHAR_DETECTOR_PIXEL_DEPTH = 8;
const unsigned short SHORT_DETECTOR_PIXEL_DEPTH = 16;

/**
 *  BeamSettings class description:
 *	Regroup the set of settings for the calculs
 */
class BeamSettings
{
public:
	/**
	 * Constructs a Beam Settings
	 *
	 */
	BeamSettings()
	{
		percentageDetection = 0.1355;
		thresholdDetection = 0;
		zoneAlarmRadius = 0;
		zoneAlarmCenterX = 0;
		zoneAlarmCenterY = 0;
		isBarycenterToCompute = false;
		isMaxLocalToCompute = false;
		isDrawContoursForDebug = false;
		detectorPixelDepth = 8;
	}

	// percentageDetections : default threshold percentages detections for each task
	double percentageDetection;

	// thresholdDetection : threshold valiue for detection
	short thresholdDetection;

	// zoneAlarmRadius : radius of the zone alarm
	short zoneAlarmRadius;

	// zoneAlarmCenterX : X pixel position of the zone alarm center
	short zoneAlarmCenterX;

	// zoneAlarmCenterY : Y pixel position of the zone alarm center
	short zoneAlarmCenterY;

	// isBarycenterToCompute : flag to compute the barycenter
	bool isBarycenterToCompute;

	// isMaxLocalToCompute : flag to compute the maximum local
	bool isMaxLocalToCompute;

	// isDrawContoursForDebug : flag to display contours on image bin
	bool isDrawContoursForDebug;

	unsigned short detectorPixelDepth;
};

/**
 *  BeamInfo class description:
 *	Regroup the set of information for a image reference OR current image
 */
class GlobalBeamInformation
{
public:
	/**
	 *	Constructor GlobalBeamInformation
	 *	Description: Implementation of GlobalBeamInformation
	 *
	 */
	GlobalBeamInformation(BeamSettings &ai_settings);

	/**
	 *  This method allow to access on settings
	 */
	BeamSettings& Settings();
	
	/**
	 *  This method allow to lock the acccess to global information (essentially for the image)
	 */
	void lock();

	/**
	 *  This method allow to unlock the acccess to global information (essentially for the image)
	 */
	void unlock();

	/**
	 *  This method allow to provide a lock to access to global information (essentially for the image)
	 */
	omni_mutex* getlock();

	/**
	 *  This method return the orginal ccd image
	 */
	cv::Mat* getImageCcd();

	// isImageCCDFound: flag of the image presence
	bool isImageCCDFound;

	// isbeamDetected: flag of the beam presence
	bool isBeamDetected;

	// isBeamShapedFound: flag of the beam shape presence
	bool isBeamShapedFound;

	// min: minimum intensity of the ccd image
	double min;

	// max: maximum intensity of the ccd image
	double max;

	// minLoc: location of the minimum intensity on the ccd image
	cv::Point minLoc;

	// maxLoc: location of the maximum intensity on the ccd image
	cv::Point maxLoc;

	// energyQuality: quality of the energy 
	Tango::AttrQuality energyQuality;

	// m_imgCcd: original ccd image 
	cv::Mat m_imgCcd;

private:
	
	// m_mutex: The mutex to protect the task 
	omni_mutex	m_dataMutex;

	// m_settings: access to settings
	BeamSettings &m_settings;
};

/**
 *  BeamSettings class description:
 *	Regroup the set of settings for the calculs
 */
class BeamInfo
{
public :

	/**
	 * Constructs a newly EquipmentBase object.
	 *
	 *	@param argin ai_this: the callback listener
	 *	@param argin ai_periodicActivity: the activity
	 *	@param argin ai_dev: log dev
	 */
	BeamInfo();
	~BeamInfo();

	/**
	 *	Method      : BeamInfo::getGlobalInfoOfNewImage()
	 *	Description : This method allow to return the global information of the new image
	 *  
	 */
	GlobalBeamInformation& getGlobalInfoOfNewImage();

	/**
	 *	Method      : BeamInfo::updateImage()
	 *	Description : This method allow to give a new image to analyse
	 */
	bool updateImage(const int ai_width, const int ai_height, const void* ai_data);

	/**
	 *	Method      : BeamInfo::setCalculationResultsList()
	 *	Description : This method allow to give the list of results of the tasks
	 */
//	void setCalculationResultsList(CalculationResultsList& ai_CalculationResultsList);


	/**
	 *	Method      : BeamInfo::settings()
	 *	Description : This method allow to return the settings
	 */
	BeamSettings& settings();

	/**
	 *	Method      : BeamInfo::getBeamThresholdDetection()
	 *	Description : This method allow to return the predefined threshold
	 */
	double getBeamThresholdDetection();

	void setBeamThresholdDetection(double newThreshold);

private :

	/**
	 *	Method      : BeamInfo::_updateImage()
	 *	Description : This method allow to update the new image or the reference image
	 */
	bool _updateImage(const int ai_width, const int ai_height, const void* ai_data, GlobalBeamInformation &aio_globalBeamInfo);

	// m_settings: Setting for the calculs
	BeamSettings m_settings;

	// m_globalInfoOfNewImage: global information of new image
	GlobalBeamInformation m_globalInfoOfNewImage;


};

#endif // BeamInfo_H