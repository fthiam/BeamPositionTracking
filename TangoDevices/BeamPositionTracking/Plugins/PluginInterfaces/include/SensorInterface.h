#ifndef _BEAM_TRACKING_SENSOR_PLUGIN_INTERFACE_H_
#define _BEAM_TRACKING_SENSOR_PLUGIN_INTERFACE_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <tango.h>
#include <yat4tango/DevicePlugIn.h>


namespace BPT
{

namespace SensorInterface
{
//=============================================================================
//! \class SensorInterface 
//! 
//!
//! Inherits from yat4tango::DevicePlugIn class.
//! 
//=============================================================================
class SensorInterface : public yat4tango::DevicePlugIn 
{

	public:
		

	struct sensorState{
		bool operational;
		std::string errorStatus;
	};	

	struct sensorData{
		double xBeamPostionPixels;
		double yBeamPostionPixels;
		bool isBeamDetected;
		bool isBeamInZone;
		std::vector<unsigned char> tresholdedImg;
		int imgHigh;
		int imgWidth;
	};	


	virtual void start(Tango::DeviceImpl * host_device)
		throw (Tango::DevFailed) = 0;

	virtual sensorData getSensorData()
		throw (Tango::DevFailed) = 0;

	virtual sensorState getSensorState()
		throw (Tango::DevFailed) = 0;

};

} // namespace SensorInterface

} // namespace BeamPositionTracking

#endif