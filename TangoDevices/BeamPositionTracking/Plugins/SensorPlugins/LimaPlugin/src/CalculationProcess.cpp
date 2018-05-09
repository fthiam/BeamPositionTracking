//*****************************************************************************
//* Copyright 2010 - 2013, Thales Optronique SAS - All rights reserved        *
//*****************************************************************************
//
//=============================================================================
// file :        CalculationProcess.h
//
// description : Implementation file for the CalculationProcess class
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

#include "CalculationProcess.h"

#define CONTOURS_MAX 100

namespace CalculationProcess
{

CalculationProcess::CalculationProcess(Tango::DeviceProxy *deviceCCD, double percentageDetection) {
	m_deviceCCD = deviceCCD;
	_refCalculationResults = NULL;

	//Initialize pixel depth
	Tango::DeviceAttribute pixelDepth = m_deviceCCD->read_attribute("detectorPixelDepth");
	pixelDepth >> _beamInfo.settings().detectorPixelDepth;
	unsigned short pixelDepthTmp;
	pixelDepth >> pixelDepthTmp;
	_calculationResults = new CalculationResults(pixelDepthTmp);
	_beamInfo.setBeamThresholdDetection(percentageDetection);

}

CalculationProcess::~CalculationProcess()
{
	if(_calculationResults)
		delete _calculationResults;
}

bool CalculationProcess::checkDetectorPixelDepth(){
	if(_beamInfo.settings().detectorPixelDepth == CHAR_DETECTOR_PIXEL_DEPTH)
		return true;
	if(_beamInfo.settings().detectorPixelDepth == SHORT_DETECTOR_PIXEL_DEPTH)
		return true;

	return false;
}

CalculationResults CalculationProcess::processBeam(){

	calculBeam();
	CalculationResults resultsToReturn = *_calculationResults;
	return resultsToReturn;

}

bool CalculationProcess::refreshBeam(){
	bool imageFound = false;
	Tango::DeviceAttribute imageAttr;

	if (_beamInfo.settings().detectorPixelDepth == CHAR_DETECTOR_PIXEL_DEPTH){
		Tango::DevVarCharArray* bufferAttr = NULL;
		//First check that camera is running :
		if(ensureCCDDeviceIsRunnning()){
			imageAttr = m_deviceCCD->read_attribute("image");
			// Retrieve the buffer image
			imageAttr >> bufferAttr;
			// Update the new ccd image
			imageFound = _beamInfo.updateImage(imageAttr.get_dim_x(), imageAttr.get_dim_y(), bufferAttr->get_buffer()); 	
		}
		// Release the buffer
		if(bufferAttr){
			delete bufferAttr;
			bufferAttr = NULL;
		}
	}
	else if (_beamInfo.settings().detectorPixelDepth == SHORT_DETECTOR_PIXEL_DEPTH){
		Tango::DevVarUShortArray* bufferAttr = NULL;
		//First check that camera is running :
		if(ensureCCDDeviceIsRunnning()){
			imageAttr = m_deviceCCD->read_attribute("image");
			// Retrieve the buffer image
			imageAttr >> bufferAttr;
			// Update the new ccd image
			imageFound = _beamInfo.updateImage(imageAttr.get_dim_x(), imageAttr.get_dim_y(), bufferAttr->get_buffer()); 	
		}
		// Release the buffer
		if(bufferAttr){
			delete bufferAttr;
			bufferAttr = NULL;
		}
	}

	return imageFound;
}

bool CalculationProcess::ensureCCDDeviceIsRunnning(){
	//Check CCD Device state 
	Tango::DevState ccdDeviceState = m_deviceCCD->state();
	if(ccdDeviceState == Tango::RUNNING){
		return true;
	}
	else if(ccdDeviceState == Tango::STANDBY){
		m_deviceCCD->command_inout("start");
		sleep(2);
		ccdDeviceState = m_deviceCCD->state();
		if (ccdDeviceState != Tango::RUNNING)
			return false;
		else 
			return true;
	}
}

void CalculationProcess::calculBeam()
{
	cv::Mat* w_CCd_tmp;
	CalculationResults *w_result_tmp;
	w_CCd_tmp = _beamInfo.getGlobalInfoOfNewImage().getImageCcd();
	w_result_tmp = _calculationResults;
	//Not really usefull since there is only one thread 
	_beamInfo.getGlobalInfoOfNewImage().lock();
	
	cv::Mat& w_CCd = *w_CCd_tmp; 
	CalculationResults& w_result = *w_result_tmp;

	// Conversion threshold in % of the (max-min) to treshold value
	const short BINARY_TRUE_VALUE = 1;

    //Image binarisation, before threshold
	cv::Mat w_imgCcd8UTMP;
	cv::Mat resultTmp_imgBin = cv::Mat(0,0,CV_8UC1);
	w_CCd.convertTo(w_imgCcd8UTMP, CV_8UC1);

    //We need to know the new image threshold (0 - 255 now..)
	double newMax, newMin;    
	cv::minMaxLoc(w_imgCcd8UTMP, &newMin, &newMax);

	//Estimate threshold
	double w_threshold = (newMax - newMin) * _beamInfo.getBeamThresholdDetection() + newMin;
	cv::threshold( w_imgCcd8UTMP, resultTmp_imgBin, w_threshold, BINARY_TRUE_VALUE, cv::THRESH_BINARY );

    // Extract thresholded image into a vector
    w_result.m_imgHigh = resultTmp_imgBin.rows;
	w_result.m_imgWidth = resultTmp_imgBin.cols;
	w_result.tresholdedImg.clear();
	for (int i = 0; i < w_result.m_imgHigh; i++){
		for (int j = 0; j < w_result.m_imgWidth; j++){
		 	w_result.tresholdedImg.push_back((uchar)resultTmp_imgBin.at<uchar>(i,j));
		}
	}

	// find contour
	bool w_foundContour = false;
	std::vector<std::vector<cv::Point> > w_contours;
	std::vector<cv::Vec4i> w_hierarchy;

	cv::findContours(resultTmp_imgBin, w_contours, w_hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
		
	// Selection of the bigger beam
	std::vector<cv::Point> w_largest_contour;

	// Exclusion of the beam if the dimensions is less than 5x5 pixels
	cv::Rect w_min_rect_engl(0, 0, 5, 5);
	cv::Rect w_rectangleEnglobant;
	
	for(int i = 0; i < w_contours.size(); i++){
		w_rectangleEnglobant = cv::boundingRect(w_contours[i]);
		if (w_rectangleEnglobant.width > w_min_rect_engl.width && 
			w_rectangleEnglobant.height > w_min_rect_engl.height){
			w_min_rect_engl = w_rectangleEnglobant;
			w_largest_contour = w_contours[i];
			w_foundContour = true;
		}
	}

	// Defaults values when beam is not detected
	if (w_foundContour == false){
		w_result.m_beamShapefound = false;
		w_result.m_centroidCenterX = 0;
		w_result.m_centroidCenterY = 0;
	}
	else{
		w_result.m_beamShapefound = true;
		double w_arclength = cv::arcLength(w_largest_contour, true);
		cv::approxPolyDP(w_largest_contour, w_result.m_largestRes, w_arclength * 0.002, false);
	}

	// Centroid calculation : geometric center following the geometric shape
	if(w_largest_contour.size()){
		cv::Moments w_moments = cv::moments(w_largest_contour);
		if( w_moments.m00 == 0){
			w_result.m_centroidCenterX = 0;
			w_result.m_centroidCenterY = 0;
		}
		else{
			w_result.m_centroidCenterX = w_moments.m10 / w_moments.m00;
			w_result.m_centroidCenterY = w_moments.m01 / w_moments.m00;
		}
	}

	else{
		w_result.m_centroidCenterX = 0;
		w_result.m_centroidCenterY = 0;
	}
	if(w_largest_contour.size()){
		// Size of the largest beam
		w_result.m_beamRectangle = cv::boundingRect(w_largest_contour);
	}
	else{
		w_result.m_beamRectangle = cv::Rect(0, 0, 0, 0);
	}

	_beamInfo.getGlobalInfoOfNewImage().unlock();
}
}