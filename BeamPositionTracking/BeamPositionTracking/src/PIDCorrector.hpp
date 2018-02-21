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

		double getStepValue(double error, double loopTime/*, double maxStep*/);

		void initCorrector();

		void setPGain(double value);

		void setIGain(double value);

		void setDGain(double value);

		void setLoopIntervalTime(double loopTime);
		
	private:
		double _dt;
        double _Kp;
        double _Kd;
        double _Ki;
        double _pre_error;
        double _integral;
	};
}//namspace
#endif