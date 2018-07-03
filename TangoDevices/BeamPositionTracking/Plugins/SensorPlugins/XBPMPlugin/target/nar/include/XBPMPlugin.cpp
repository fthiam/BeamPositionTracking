//=============================================================================
// XBPMPlugin.cpp
//=============================================================================
// abstraction.......Plugin to use XBPM in BeamPositionTracking application
// class.............XBPMPlugin
// original author...thiam - SOLEIL
//=============================================================================

#include <XBPMPlugin.h>

EXPORT_SINGLECLASS_PLUGIN(XBPMPlugin::XBPMPlugin, XBPMPlugin::XBPMPluginInfo);

namespace XBPMPlugin
{
std::string XBPM_DEVICE_ADRESS_STR = "XBPMDeviceUrl";

std::string DETECTOR_X_DIMENSION = "DetectorXDimension";
std::string DETECTOR_Y_DIMENSION = "DetectorYDimension";
std::string XBPM_X_CENTROID_ATTR_NAME = "verticalPosition";
std::string XBPM_Y_CENTROID_ATTR_NAME = "horizontalPosition";
std::string SENSOR_UNIT = "mm";
// ============================================================================
// XBPMPlugin::XBPMPlugin
// ============================================================================
XBPMPlugin::XBPMPlugin()
: yat4tango::TangoLogAdapter(NULL)
{
	//Init members
	m_XBPMDeviceAdressStr = "XBPMDeviceUrl";
	m_thresholdStr = "FixedPercentageDetection";
	m_sensorState.operational = false;
	m_sensorState.errorStatus = "Not initialized";
	m_propertyMissing = false;
	m_proxyXBPMInit = false;
	m_xCentroid = yat::ieee_nan();
	m_yCentroid = yat::ieee_nan();
	m_xImageDim = 512;
	m_yImageDim = 512;
	m_isBeamDetected = false;
	m_isBeamInZone = false;
	m_deviceXBPMAdress = "";
	m_deviceXBPM = NULL;
	m_percentageDetection = 1;
	m_fixMode = true;
}

// ============================================================================
// XBPMPlugin::~XBPMPlugin
// ============================================================================
XBPMPlugin::~XBPMPlugin()
{
	//if(m_deviceXBPM)
	//	delete m_deviceXBPM;
}
// ============================================================================
// SPECIFIC PLUGIN PROPERTIES
// ============================================================================
// ============================================================================
// XBPMPlugin::enumerate_properties
// ============================================================================
void XBPMPlugin::enumerate_properties (yat4tango::PlugInPropertyInfoList& propInfoList) const
throw (Tango::DevFailed)
{
	// fill propInfoList with name & type of plugin's needed properties
	try{
		propInfoList[m_XBPMDeviceAdressStr] = (int)yat::PlugInPropType::STRING;	
		propInfoList[DETECTOR_X_DIMENSION] = (int)yat::PlugInPropType::DOUBLE;	
		propInfoList[DETECTOR_Y_DIMENSION] = (int)yat::PlugInPropType::DOUBLE;	
	}
	catch (...){
	  	/*m_sensorState.operational = false;
	  	m_sensorState.errorStatus = "XBPMPlugin couldn't enumerate_properties for unknown reason";*/
	}
}

// ============================================================================
// XBPMPlugin::updateStatus
// ============================================================================
void XBPMPlugin::updateStatus(std::string message){

	if (m_sensorState.errorStatus != "")
		m_sensorState.errorStatus = m_sensorState.errorStatus + "\n" + message;
	else
		m_sensorState.errorStatus = message;
}
// ============================================================================
// XBPMPlugin::set_properties
// ============================================================================
void XBPMPlugin::set_properties (yat4tango::PlugInPropertyValueList& propValueList)
throw (Tango::DevFailed)
{
	// get properties' values from propValueList
	yat4tango::PlugInPropertyValueList::iterator propIt;

	bool proxyUrl = false;
	bool xDetectorSize = false;
	bool yDetectorSize = false;

	for (propIt = propValueList.begin(); propIt != propValueList.end(); ++propIt){

	  	if ((*propIt).name == XBPM_DEVICE_ADRESS_STR){
		  try{
			// get value
			if ((*propIt).valid){
				proxyUrl = true;
				m_deviceXBPMAdress = yat::any_cast_ext<std::string>((*propIt).value);
			}
		  }
		  catch (...){
		  	m_propertyMissing = true;
			m_sensorState.operational = false;
			updateStatus("Looks like XBPMDeviceUrl property is not in the right format");
		  }
	    }	

	    if ((*propIt).name == DETECTOR_X_DIMENSION){
		  try{
			// get value
			if ((*propIt).valid){
				xDetectorSize = true;
				m_xDetecorSize = yat::any_cast<double>((*propIt).value);
			}
		  }
		  catch (...){
		  	m_propertyMissing = true;
			m_sensorState.operational = false;
			updateStatus("Looks like x detector size property is not in the right format");
		  }
	    }	

	    if ((*propIt).name == DETECTOR_Y_DIMENSION){
		  try{
			// get value
			if ((*propIt).valid){
				yDetectorSize = true;
				m_yDetecorSize = yat::any_cast<double>((*propIt).value);
			}
		  }
		  catch (...){
		  	m_propertyMissing = true;
			m_sensorState.operational = false;
			updateStatus("Looks like y detector size property is not in the right format");
		  }
	    }	
	}
    if (!proxyUrl){
		// mandatory property
		m_propertyMissing = true;
		m_sensorState.operational = false;
		updateStatus("XBPM plugin doesn't find property XBPMDeviceUrl (need a proxy on XBPM device)");
    }
    if (!xDetectorSize){
		// mandatory property
		m_propertyMissing = true;
		m_sensorState.operational = false;
		updateStatus("XBPM plugin doesn't find property DetectorXDimension");
    }
    if (!yDetectorSize){
		// mandatory property
		m_propertyMissing = true;
		m_sensorState.operational = false;
		updateStatus("XBPM plugin doesn't find property DetectorYDimension");
    }
}

// ============================================================================
// XBPMPlugin::enumerate_attributes
// ============================================================================
void XBPMPlugin::enumerate_attributes (yat4tango::DynamicAttributeDescriptionList& attrDescList) const
throw (Tango::DevFailed)
{
	//- attribute definition:
}


// ============================================================================
// XBPMPlugin::getSensorData
// ============================================================================
BPT::SensorInterface::SensorInterface::sensorData XBPMPlugin::getSensorData()
  throw (Tango::DevFailed)
{
	BPT::SensorInterface::SensorInterface::sensorData dataToReturn;
	if (m_imageBuilder){
		try{
			this->getBeamPosition();
			//Execute calculation, Update local Data
			if(m_sensorState.operational){
				double xPixelCentroid = convertDeltaToPixelPosition (m_xCentroid, m_xDetecorSize);
				double yPixelCentroid = convertDeltaToPixelPosition (m_yCentroid, m_yDetecorSize);
				std::vector<unsigned char> thresholdedImg = m_imageBuilder->getNewImage(xPixelCentroid, yPixelCentroid);
				dataToReturn.xBeamPostionPixels = xPixelCentroid;
				dataToReturn.yBeamPostionPixels = yPixelCentroid;
				dataToReturn.xBeamPosition = m_xCentroid;
				dataToReturn.yBeamPosition = m_yCentroid;
				dataToReturn.imgWidth = m_xImageDim;
				dataToReturn.imgHeight = m_yImageDim;
				dataToReturn.thresholdedImg = thresholdedImg;
				dataToReturn.isBeamDetected = true;
			}
			else{
				dataToReturn.imgWidth = 0;
				dataToReturn.imgHeight = 0;
				dataToReturn.thresholdedImg.clear();
			}

		
		}catch(Tango::DevFailed &df){
			m_sensorState.operational = false;
			m_sensorState.errorStatus = "Processing error DevFailed catched, try re-init if problem persists";
		}catch(...){
			m_sensorState.operational = false;
			m_sensorState.errorStatus = "Processing error (..) catched, try re-init if problem persists";
		}
	}
	else {
		dataToReturn.xBeamPostionPixels = 0;
		dataToReturn.yBeamPostionPixels = 0;
		dataToReturn.isBeamDetected = false;
	}

	return dataToReturn;
}
// ============================================================================
// XBPMPlugin::getState
// ============================================================================
BPT::SensorInterface::SensorInterface::sensorState XBPMPlugin::getSensorState()
  throw (Tango::DevFailed)
{
	return m_sensorState;
}

// ============================================================================
// XBPMPlugin::start
// ============================================================================
void XBPMPlugin::start(Tango::DeviceImpl * host_device, bool fixMode)
  throw (Tango::DevFailed)
{
	m_fixMode = fixMode;
	try{
		if(!m_propertyMissing){
			initialize_connection();
			//Build calculation with minimum setting configuration
			if (m_proxyXBPMInit){
				m_imageBuilder = new ImageBuilder::ImageBuilder ( m_xImageDim, m_yImageDim, 20);
				m_sensorState.operational = true;
			 	m_sensorState.errorStatus = "Idle ready to start processing";
			}
		}
	}
	catch (...){
		m_sensorState.operational = false;
		m_sensorState.errorStatus = "Something went wrong with XBPM's plugin initilialisation!";
    }
}

// ============================================================================
// XBPMPlugin::initialize_connection
// ============================================================================
void XBPMPlugin::initialize_connection ()
{
	try{
		//Init XBPM device proxy
		m_deviceXBPM = new Tango::DeviceProxy (m_deviceXBPMAdress);
		Tango::DeviceData test  = m_deviceXBPM->command_inout("Status");
		std::string statusOut;
		m_proxyXBPMInit = true;
	}
	catch (...){
		m_deviceXBPM = NULL;
		m_proxyXBPMInit = false;
		m_sensorState.operational = false;
		m_sensorState.errorStatus = "XBPM plugin couldn't instanciate deviceXBPM's proxy, check adress :\n" + m_deviceXBPMAdress;
	}
}
// ============================================================================
// XBPMPlugin::getBeamPosition
// ============================================================================
void XBPMPlugin::getBeamPosition(){

	m_xCentroid = readDoubleAttributeFromXBPM(XBPM_X_CENTROID_ATTR_NAME);
	m_yCentroid = readDoubleAttributeFromXBPM(XBPM_Y_CENTROID_ATTR_NAME);
}

// ============================================================================
// XBPMPlugin::convertDeltaToPixelPosition
// ============================================================================
double XBPMPlugin::convertDeltaToPixelPosition(double relativePosition, double axisSize){

	double positionInPixels = (m_xImageDim * (relativePosition + axisSize/2)) / axisSize;

	return positionInPixels;
}
// ============================================================================
// XBPMPlugin::readAttributeFromXBPM
// ============================================================================
double XBPMPlugin::readDoubleAttributeFromXBPM(std::string attributeName){

	double value = 0;
	try{
		if (m_deviceXBPM){
			Tango::DeviceAttribute tmpAttributeToReturn;
			tmpAttributeToReturn = m_deviceXBPM->read_attribute(attributeName);
			if (tmpAttributeToReturn.get_quality() != Tango::ATTR_VALID){
				m_sensorState.operational = false;
				m_sensorState.errorStatus = "Something went wrong when trying to read attribute : "+ attributeName +" on XBPM device, check his state before to init...";
		   	}

		   	tmpAttributeToReturn>>value;
	   	}
		else{
			m_sensorState.operational = false;
			m_sensorState.errorStatus = "Something went wrong when trying to read attribute : "+ attributeName +" on XBPM device, Try to init";
		}
	}catch(Tango::ConnectionFailed &conf){
		m_sensorState.operational = false;
		m_sensorState.errorStatus = "Something went wrong when trying to read attribute : "+ attributeName +" on XBPM device, Check XBPM device device..";
	}catch(Tango::CommunicationFailed &comf){
		m_sensorState.operational = false;
		m_sensorState.errorStatus = "Something went wrong when trying to read attribute : "+ attributeName +" on XBPM device,Check XBPM device device..";
	}catch(Tango::DeviceUnlocked &du){
		m_sensorState.operational = false;
		m_sensorState.errorStatus = "Something went wrong when trying to read attribute : "+ attributeName +" on XBPM device,Check XBPM device device..";
	}catch(Tango::DevFailed &df){
		m_sensorState.operational = false;
		m_sensorState.errorStatus = "Something went wrong when trying to read attribute : "+ attributeName +" on XBPM device, Check XBPM device device..";
	}
	catch(...){
		m_sensorState.operational = false;
		m_sensorState.errorStatus = "Connection unknow error while trying to read "+ attributeName + " on actuator system.";
	}
	return value;
}


// ============================================================================
// XBPMPlugin::getSensorUnit
// ============================================================================
std::string XBPMPlugin::getSensorUnit()
  throw (Tango::DevFailed){
	return SENSOR_UNIT;
}

}//namespace XBPMPlugin
