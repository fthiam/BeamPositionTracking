//*****************************************************************************
//=============================================================================
// file :        PIDCorrector.cpp
//
// description : Implementation file for the PIDCorrector class.
//
// project :     BeamPositionTracking
//
// $Author:  Falilou THIAM
//
// $Revision:  $
// $Date:  $
//
// $HeadURL:  $
//
//=============================================================================
#include "PIDCorrector.hpp"

namespace PIDCorrector
{

	//--------------------------------------------------------
	/**
	 *	Method      : PIDCorrector::PIDCorrector()
	 */
	//--------------------------------------------------------
	PIDCorrector::PIDCorrector(double P, double I, double D)
	{
		_Kp = P; //proportional gain
		_Ki = I; //Integral gain
		_Kd = D; //derivative gain 
		_dt = 0; //loop interval time
		_pre_error = 0;
		_Pout = 0 ;
		_Iout = 0 ;
		_Dout = 0 ;
		_axisPosition = 0;
	}
	//--------------------------------------------------------
	/**
	 *	Method      : PIDCorrector::~PIDCorrector()
	 */
	//--------------------------------------------------------
	PIDCorrector::~PIDCorrector(void){
	}

	void PIDCorrector::setPGain(double pGain){
		_Kp = pGain;
	}

	void PIDCorrector::setIGain(double iGain){
		_Ki = iGain;
	}

	void PIDCorrector::setDGain(double dGain){
		_Kd = dGain;
	}	

	void PIDCorrector::setLoopIntervalTime(double loopTime){
		_dt = loopTime;
	}

	void PIDCorrector::initCorrector(double axisPosition){
		std::cout<<"initCorrector entering  axisPosition = "<<axisPosition<<std::endl;
		_axisPosition = axisPosition;
		_pre_error = 0;
	}
	//--------------------------------------------------------
	/**
	 *	PIDCorrector::getStepValue(double error)
	 *	Description : will calculate step value 
	 */
	//--------------------------------------------------------
	void PIDCorrector::newStepValue(double error, double loopTime){

		_dt = loopTime;
	    // Proportional term
	    _Pout = _Kp * error;
	    // Integral term
	    _axisPosition += _Ki *  error * _dt;;
	    _Iout = _axisPosition;

	    // Derivative term
	    double derivative = 0;
	    if(_dt != 0)
	    	derivative = (error - _pre_error) / _dt;
	    _Dout = _Kd * derivative;

	    // Save error to previous error
	    _pre_error = error;
	}
	//--------------------------------------------------------
	/**
	 *	PIDCorrector::getDelta()
	 */
	//--------------------------------------------------------
	double PIDCorrector::getDelta(){
		double output = _Pout + _Dout;

	    return output;
	}
	//--------------------------------------------------------
	/**
	 *	PIDCorrector::getNewPosition()
	 */
	//--------------------------------------------------------
	double PIDCorrector::getNewPosition(){
		double output = _Pout + _Iout + _Dout;

	    return output;
	}

}//namespace