//=============================================================================
// SimulatedAxisPlugin.h
//=============================================================================
// abstraction.......Plugin to use a simulated axis in beam tracking position
// class.............SimulatedAxisPlugin
// original author...Thiam- SOLEIL
//=============================================================================

#ifndef _ACTUATOR_AXIS_PLUGIN_H_
#define _ACTUATOR_AXIS_PLUGIN_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <tango.h>

#include <yat/utils/StringTokenizer.h>
#include <yat/plugin/IPlugInInfo.h>
#include <yat/plugin/IPlugInObject.h>
#include <yat/plugin/PlugInSymbols.h>
#include "../../../PluginInterfaces/include/ActuatorInterface.h"
#include <yat4tango/LogHelper.h>
#include <iostream>

namespace SimulatedAxisPlugin
{

//=============================================================================
//! \class SimulatedAxisPluginInfo 
//! \brief Plugin infos class
//!
//! Inherits from yat::IPlugInInfo class.
//=============================================================================
class SimulatedAxisPluginInfo : public yat::IPlugInInfo
{
public: 
  std::string get_plugin_id() const
  { return "SimulatedAxis"; }

   virtual std::string get_interface_name() const 
  { return "ActuatorInterface"; }

   virtual std::string get_version_number() const
  { return "1.0.0"; }
};

//=============================================================================
//! \class SimulatedAxisPlugin 
//!
//! Inherits from BeamPositionTracking_ns::ActuatorInterface::ActuatorInterface class.
//=============================================================================
class SimulatedAxisPlugin : public AS::ActuatorInterface::ActuatorInterface,
							public yat4tango::TangoLogAdapter
{

public:

	SimulatedAxisPlugin();
	~SimulatedAxisPlugin();

	
	//-------------------------------------------------------------------------
	// yat4tango::DevicePlugIn interface
	//-------------------------------------------------------------------------
	//!\brief Gets the plugin's specific properties.
	void enumerate_properties (yat4tango::PlugInPropertyInfoList& propInfoList) const
		throw (Tango::DevFailed);

	//!\brief Sets the plugin properties.
	void set_properties (yat4tango::PlugInPropertyValueList& propValueList)
		throw (Tango::DevFailed);

	//!\brief Gets the plugin dynamic attributes.
	void enumerate_attributes (yat4tango::DynamicAttributeDescriptionList& attrDescList) const
		throw (Tango::DevFailed);
	//-------------------------------------------------------------------------
	// yat4tango::DevicePlugIn interface
	//-------------------------------------------------------------------------

	void init(Tango::DeviceImpl * host_device, std::string deviceAdress) 
		throw (Tango::DevFailed);
			
	void setAxisPosition(double position)
		throw (Tango::DevFailed);

	void moveAxisRelative(double relativeMovement) 
		throw (Tango::DevFailed);

	void stopAxisMovement() 
		throw (Tango::DevFailed);

	double getAxisCurrentPosition() 
		throw (Tango::DevFailed);

	AS::ActuatorInterface::ActuatorInterface::AxisStateStatus getAxisStateStatus()
		throw (Tango::DevFailed);
		
 
private:

	AS::ActuatorInterface::ActuatorInterface::AxisStateStatus _stateStatus;

	bool _simulatedAxisProxyInit;

	void initialize_connection(std::string deviceAdress);

	void sendCommandToActuatorSystem(std::string cmdName, bool args, Tango::DeviceData argsData);

	//	mp_SimulatedAxisDevice:	device to access axis
	Tango::DeviceProxy *_simulatedAxisDevice;
	std::string _simulatedAxisDeviceDeviceAdressStr;

	void updateStateStatus();
};


} // namespace SimulatedAxisPlugin



#endif // _ACTUATOR_AXIS_PLUGIN_H_
