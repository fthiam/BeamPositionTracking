//=============================================================================
// XBPMPlugin.h
//=============================================================================
// abstraction.......Plugin to use XBPM in BeamPositionTracking application
// class.............XBPMPlugin
// original author...Thiam- SOLEIL
//=============================================================================

#ifndef _XBPM_SENSOR_PLUGIN_H_
#define _XBPM_SENSOR_PLUGIN_H_

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
#include <ImageBuilder.h>

namespace XBPMPlugin
{

//=============================================================================
//! \class XBPMPluginInfo 
//! \brief Plugin infos class
//!
//! Inherits from yat::IPlugInInfo class.
//=============================================================================
class XBPMPluginInfo : public yat::IPlugInInfo
{
public: 
  std::string get_plugin_id() const
  { return "XBPM"; }

   virtual std::string get_interface_name() const 
  { return "SensorInterface"; }

   virtual std::string get_version_number() const
  { return "1.0.0"; }
};

//=============================================================================
//! \class XBPMPlugin 
//!
//! Inherits from BeamPositionTracking_ns::SensorInterface::SensorInterface class.
//=============================================================================
class XBPMPlugin : public BPT::SensorInterface::SensorInterface,
					public yat4tango::TangoLogAdapter
{

public:
	XBPMPlugin();
	~XBPMPlugin();


	void start(Tango::DeviceImpl * host_device, bool fixMode) 
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

	std::string getSensorUnit()
		throw(Tango::DevFailed);

	void write_callback_percentageDetection(yat4tango::DynamicAttributeWriteCallbackData& cbd);

	void read_callback_percentageDetection(yat4tango::DynamicAttributeReadCallbackData& cbd);



		
private:

	BPT::SensorInterface::SensorInterface::sensorState m_sensorState;

	void updateStatus(std::string message);

	void getBeamPosition();
	
	double convertDeltaToPixelPosition(double relativePosition, double axisSize);

	bool m_propertyMissing;

	bool m_proxyXBPMInit;

	void initialize_connection();

	double readDoubleAttributeFromXBPM(std::string attributeName);

	std::string m_deviceXBPMAdress;

	std::string m_currentDeviceAdress;

	ImageBuilder::ImageBuilder *m_imageBuilder;

	//	mp_deviceXBPM:	device to access XBPM
	Tango::DeviceProxy *m_deviceXBPM;

	bool m_percentageDetectionPresent;

	std::string m_XBPMDeviceAdressStr;
	std::string m_thresholdStr;

	///DATA///
	double m_xCentroid;
	double m_yCentroid;
	int m_xImageDim;
	int m_yImageDim;
	bool m_isBeamDetected;
	bool m_isBeamInZone;

	double m_percentageDetection;

	bool m_fixMode;

	double m_xDetecorSize;
	double m_yDetecorSize;
};


} // namespace XBPMPlugin



#endif // _XBPM_SENSOR_PLUGIN_H_
