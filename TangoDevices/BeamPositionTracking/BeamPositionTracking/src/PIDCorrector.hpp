//*****************************************************************************
//=============================================================================
// file :        PIDCorrector.h
//
// description : This include file for the PIDCorrector class.
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
//=============================================================================
#ifndef PIDCorrector_H
#define PIDCorrector_H


#include "tango.h"

namespace PIDCorrector
{
	class PIDCorrector
	{	
	public:
		PIDCorrector(double P, double I, double D);
		~PIDCorrector();

		void newStepValue(double error, double loopTime);

		void initCorrector(double axisPosition);

		void setPGain(double value);

		void setIGain(double value);

		void setDGain(double value);

		void setLoopIntervalTime(double loopTime);

		double getDelta();

		double getNewPosition();
		
	private:

		//PID configuration
		double _dt;
        double _Kp;
        double _Kd;
        double _Ki;
        double _pre_error;
        double _axisPosition;

        //PID Outputs
        double _Pout;
        double _Iout;
        double _Dout;
	};
}//namspace
#endif