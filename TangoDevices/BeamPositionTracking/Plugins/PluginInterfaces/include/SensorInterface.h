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
		double xBeamPosition;
		double yBeamPosition;
		double xBeamPostionPixels;
		double yBeamPostionPixels;
		bool isBeamDetected;
		bool isBeamInZone;
		std::vector<unsigned char> thresholdedImg;
		int imgHeight;
		int imgWidth;
	};	


	virtual void start(Tango::DeviceImpl * host_device, bool fixMode)
		throw (Tango::DevFailed) = 0;

	virtual sensorData getSensorData()
		throw (Tango::DevFailed) = 0;

	virtual std::string getSensorUnit()
		throw (Tango::DevFailed) = 0;

	virtual sensorState getSensorState()
		throw (Tango::DevFailed) = 0;

};

} // namespace SensorInterface

} // namespace BeamPositionTracking

#endif