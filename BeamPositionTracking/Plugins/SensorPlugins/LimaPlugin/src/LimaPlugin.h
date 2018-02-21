//=============================================================================
// LimaPlugin.h
//=============================================================================
// abstraction.......Plugin to use Lima in BeamPositionTracking application
// class.............LimaPlugin
// original author...Thiam- SOLEIL
//=============================================================================

#ifndef _LIMA_SENSOR_PLUGIN_H_
#define _LIMA_SENSOR_PLUGIN_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <tango.h>

#include <yat/utils/StringTokenizer.h>
#include <yat/plugin/IPlugInInfo.h>
#include <yat/plugin/IPlugInObject.h>
#include <yat/plugin/PlugInSymbols.h>
#include "../../../PluginInterfaces/include/SensorInterface.h"
#include <yat4tango/LogHelper.h>
#include <map>
#include <math.h>
#include <iostream>
#include <CalculationProcess.h>

namespace LimaPlugin
{

//=============================================================================
//! \class LimaPluginInfo 
//! \brief Plugin infos class
//!
//! Inherits from yat::IPlugInInfo class.
//=============================================================================
class LimaPluginInfo : public yat::IPlugInInfo
{
public: 
  std::string get_plugin_id() const
  { return "Lima"; }

   virtual std::string get_interface_name() const 
  { return "SensorInterface"; }

   virtual std::string get_version_number() const
  { return "1.0.0"; }
};

//=============================================================================
//! \class LimaPlugin 
//!
//! Inherits from BeamPositionTracking_ns::SensorInterface::SensorInterface class.
//=============================================================================
class LimaPlugin : public BPT::SensorInterface::SensorInterface,
					public yat4tango::TangoLogAdapter
{

public:
	LimaPlugin();
	~LimaPlugin();


	void start(Tango::DeviceImpl * host_device) 
		throw (Tango::DevFailed);
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
	// LocalCorrectionPlugInInterface interface
	//-------------------------------------------------------------------------
	BPT::SensorInterface::SensorInterface::sensorData getSensorData()
		throw(Tango::DevFailed);

	BPT::SensorInterface::SensorInterface::sensorState getSensorState()
		throw(Tango::DevFailed);
		
private:

	BPT::SensorInterface::SensorInterface::sensorState m_sensorState;

	void updateStatus(std::string message);

	bool m_propertyMissing;

	bool m_proxyCCDInit;

	void initialize_connection();

	short m_reducedImageWidth;

	short m_reducedImageHeight;

	double m_percentageDetection;

	std::string m_deviceAdress;

	CalculationProcess::CalculationProcess *m_calculationProcess;

	//	mp_deviceCCD:	device to access CCD
	Tango::DeviceProxy *m_deviceCCD;


	std::string m_limaDeviceAdressStr;
	std::string m_reducedImageHeightStr;
	std::string m_reducedImageWidthStr;
	std::string m_percentageDetectionStr;
	
	///DATA///
	double m_xCentroid;
	double m_yCentroid;
	bool m_isBeamDetected;
	bool m_isBeamInZone;
};


} // namespace LimaPlugin



#endif // _LIMA_SENSOR_PLUGIN_H_
