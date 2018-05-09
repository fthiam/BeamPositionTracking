//*****************************************************************************
//* Copyright 2010 - 2013, Thales Optronique SAS - All rights reserved        *
//*****************************************************************************
//
//=============================================================================
// file :        BeamInfo.h
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

#include "BeamInfo.h"
/**
 *  BeamInfo class description:
 *	Regroup the set of information for a image reference OR current image
 */
GlobalBeamInformation::GlobalBeamInformation(BeamSettings &ai_settings)
:m_settings(ai_settings)
{

	if (m_settings.detectorPixelDepth == CHAR_DETECTOR_PIXEL_DEPTH){
		m_imgCcd = cv::Mat(2,2,CV_8S, 0);
	}
	else if (m_settings.detectorPixelDepth == SHORT_DETECTOR_PIXEL_DEPTH){
		m_imgCcd = cv::Mat(2,2,CV_16S, 0);
	}
	isImageCCDFound = false;
	isBeamDetected = false;
	isBeamShapedFound = false;
	min = 0;
	max = 0;
	minLoc;
	maxLoc;
	energyQuality = Tango::ATTR_ALARM;
}

BeamSettings& GlobalBeamInformation::Settings()
{
	return m_settings;
}

void GlobalBeamInformation::lock()
{
	m_dataMutex.lock();
}

void GlobalBeamInformation::unlock()
{
	m_dataMutex.unlock();
}

omni_mutex* GlobalBeamInformation::getlock()
{
	return &m_dataMutex;
}


cv::Mat* GlobalBeamInformation::getImageCcd()
{
	return &m_imgCcd;
}

/**
 *  BeamInfo class description:
 *	Regroup the set of information for 2 images : reference and current 
 */
BeamInfo::BeamInfo()
:m_globalInfoOfNewImage(m_settings)
{

}

BeamInfo::~BeamInfo()
{
	m_globalInfoOfNewImage.m_imgCcd.release();
}

GlobalBeamInformation& BeamInfo::getGlobalInfoOfNewImage()
{
	return m_globalInfoOfNewImage;
}

BeamSettings& BeamInfo::settings()
{
	return m_settings;
}

void BeamInfo::setBeamThresholdDetection(double newThreshold){
	m_settings.percentageDetection = newThreshold;
}

double BeamInfo::getBeamThresholdDetection()
{
	return m_settings.percentageDetection;
}

bool BeamInfo::updateImage(const int ai_width, const int ai_height, const void* ai_data)
{
	return _updateImage(ai_width, ai_height, ai_data, m_globalInfoOfNewImage);
}

bool BeamInfo::_updateImage(const int ai_width, const int ai_height, const void* ai_data, GlobalBeamInformation &aio_globalBeamInfo)
{
	bool w_result = false;
	
	// lock the access to images
	aio_globalBeamInfo.lock();

	try
	{
		cv::Mat& w_imgCcd = *aio_globalBeamInfo.getImageCcd();

		if(ai_width != 0 && ai_height != 0 )
		{

			// Check if the image size has changed (new roi)
			if( w_imgCcd.cols != ai_width && 
				w_imgCcd.rows != ai_height)
			{
				// Deallocation of the previous matrix
				w_imgCcd.release();

				//Different types of image, depending on bit depth value :
				if (m_settings.detectorPixelDepth == CHAR_DETECTOR_PIXEL_DEPTH)
					w_imgCcd = cv::Mat(ai_height,  ai_width, CV_8S);
				else if (m_settings.detectorPixelDepth == SHORT_DETECTOR_PIXEL_DEPTH)
					w_imgCcd = cv::Mat(ai_height,  ai_width, CV_16S);
			}
			// Copy the buffer of the new image in the current matrix
			aio_globalBeamInfo.isImageCCDFound = true;

			//Different types of image, depending on bit depth value :
			if (m_settings.detectorPixelDepth == CHAR_DETECTOR_PIXEL_DEPTH)
				memcpy(w_imgCcd.data, ai_data, w_imgCcd.rows * w_imgCcd.cols * CV_8S);
			else if (m_settings.detectorPixelDepth == SHORT_DETECTOR_PIXEL_DEPTH)
				memcpy(w_imgCcd.data, ai_data, w_imgCcd.rows * w_imgCcd.cols * CV_16U);
						
			w_result = true;
		}
		else
		{
			aio_globalBeamInfo.isImageCCDFound = false;
			w_result = false;
		}
	}
	catch(std::exception &e)
	{
		std::cerr << "BeamInfo: _updateImage - Exception : " << e.what() <<  std::endl; 
		aio_globalBeamInfo.isImageCCDFound = false;
		w_result = false;
	}
	catch(...)
	{
		std::cerr << "BeamInfo: _updateImage - Exception unknown !" << std::endl; 
		aio_globalBeamInfo.isImageCCDFound = false;
		w_result = false;
	}

	// release the access to images
	aio_globalBeamInfo.unlock();

	return w_result;
}
