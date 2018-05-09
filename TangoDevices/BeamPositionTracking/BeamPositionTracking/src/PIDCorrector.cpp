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
		_integral = 0;
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

	void PIDCorrector::initCorrector(){
		_pre_error = 0;
	}
	//--------------------------------------------------------
	/**
	 *	PIDCorrector::getStepValue(double error)
	 *	Description : will calculate step value 
	 */
	//--------------------------------------------------------
	double PIDCorrector::getStepValue(double error, double loopTime){

		_dt = loopTime;
	    // Proportional term
	    double Pout = 0 ;
	    Pout = _Kp * error;
	    // Integral term
	    _integral += error * _dt;
	    double Iout = _Ki * _integral;
	    // Derivative term
	    double derivative = 0;
	    if(_dt != 0)
	    	derivative = (error - _pre_error) / _dt;
	    double Dout = _Kd * derivative;
	    // Calculate total output
	    double output = Pout + Iout + Dout;
	    // Save error to previous error
	    _pre_error = error;

	    return output;
	}
}//namespace