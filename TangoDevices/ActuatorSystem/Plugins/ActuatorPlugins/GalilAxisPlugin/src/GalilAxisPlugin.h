//=============================================================================
// GalilAxisPlugin.h
//=============================================================================
// abstraction.......Plugin to use a simulated axis in beam tracking position
// class.............GalilAxisPlugin
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

namespace GalilAxisPlugin
{

//=============================================================================
//! \class GalilAxisPluginInfo 
//! \brief Plugin infos class
//!
//! Inherits from yat::IPlugInInfo class.
//=============================================================================
class GalilAxisPluginInfo : public yat::IPlugInInfo
{
public: 
  std::string get_plugin_id() const
  { return "galilAxis"; }

   virtual std::string get_interface_name() const 
  { return "ActuatorInterface"; }

   virtual std::string get_version_number() const
  { return "1.0.0"; }
};

//=============================================================================
//! \class GalilAxisPlugin 
//!
//! Inherits from BeamPositionTracking_ns::ActuatorInterface::ActuatorInterface class.
//=============================================================================
class GalilAxisPlugin : public AS::ActuatorInterface::ActuatorInterface,
							public yat4tango::TangoLogAdapter
{

public:

	GalilAxisPlugin();
	~GalilAxisPlugin();

	
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

	std::string getAxisUnit()	
		throw (Tango::DevFailed);

	AS::ActuatorInterface::ActuatorInterface::AxisStateStatus getAxisStateStatus()
		throw (Tango::DevFailed);
		
 
private:

	AS::ActuatorInterface::ActuatorInterface::AxisStateStatus _stateStatus;

	bool _galilAxisProxyInit;

	void initialize_connection(std::string deviceAdress);

	void sendCommandToActuatorSystem(std::string cmdName, bool args, Tango::DeviceData argsData);

	//	mp_galilAxisDevice:	device to access axis
	Tango::DeviceProxy *_galilAxisDevice;
	std::string _galilAxisDeviceDeviceAdressStr;

	void updateStateStatus();

	std::string _deviceURLAdress;
};


} // namespace GalilAxisPlugin



#endif // _ACTUATOR_AXIS_PLUGIN_H_
