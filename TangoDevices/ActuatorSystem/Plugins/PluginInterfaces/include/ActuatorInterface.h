#ifndef _ACTUATOR_SYSTEM_PLUGIN_INTERFACE_H_
#define _ACTUATOR_SYSTEM_PLUGIN_INTERFACE_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <tango.h>
#include <yat4tango/DevicePlugIn.h>

namespace AS
{
namespace ActuatorInterface
{
//=============================================================================
//! \class ActuatorInterface 
//! 
//!
//! Inherits from yat4tango::DevicePlugIn class.
//! 
//=============================================================================
class ActuatorInterface : public yat4tango::DevicePlugIn 
{

	public:

	struct AxisStateStatus{
		bool isConnexionOperational;
		std::string axisStatus;
		Tango::DevState axisState;
	};	

	virtual void init(Tango::DeviceImpl * host_device, std::string deviceAdress)
		throw (Tango::DevFailed) = 0;

	//SETTER
	virtual void setAxisPosition(double position)
		throw (Tango::DevFailed) = 0;
		
	virtual void moveAxisRelative(double relativeMovement)
		throw (Tango::DevFailed) = 0;

	virtual void stopAxisMovement()
		throw (Tango::DevFailed) = 0;

	//GETTER
	virtual double getAxisCurrentPosition()
		throw (Tango::DevFailed) = 0;

	virtual std::string getAxisUnit()
		throw (Tango::DevFailed) = 0;

	// Plugin is supposed to return his state this way: 
	// - ON -> Device is ready to move
	// - MOVING -> Device is currently moving
	// - STANDBY - > Device is waiting for an action (in this state : init/Standby/off ...)
	// - FAULT -> Device is not available (off)

	virtual AxisStateStatus getAxisStateStatus()
		throw (Tango::DevFailed) = 0;
};

} // namespace Actuator Interface

} // namespace Actuator system

#endif