//=============================================================================
// LimaPlugin.cpp
//=============================================================================
// abstraction.......Plugin to use Lima in BeamPositionTracking application
// class.............LimaPlugin
// original author...thiam - SOLEIL
//=============================================================================

#include <LimaPlugin.h>

EXPORT_SINGLECLASS_PLUGIN(LimaPlugin::LimaPlugin, LimaPlugin::LimaPluginInfo);

namespace LimaPlugin
{
std::string LIMA_DEVICE_ADRESS_STR = "LimaDeviceUrl";
std::string PERCENTAGE_DETECTION = "PercentageDetection";

// ============================================================================
// LimaPlugin::LimaPlugin
// ============================================================================
LimaPlugin::LimaPlugin()
: yat4tango::TangoLogAdapter(NULL)
{
	//Init members
	m_limaDeviceAdressStr = "LimaDeviceUrl";
	m_percentageDetectionStr = "PercentageDetection";
	m_sensorState.operational = false;
	m_sensorState.errorStatus = "Not initialized";
	m_calculationProcess = 0;
	m_propertyMissing = false;
	m_proxyCCDInit = false;
	m_xCentroid = yat::ieee_nan();
	m_yCentroid = yat::ieee_nan();
	m_isBeamDetected = false;
	m_isBeamInZone = false;
	m_deviceAdress = "";
	m_deviceCCD = NULL;
}

// ============================================================================
// LimaPlugin::~LimaPlugin
// ============================================================================
LimaPlugin::~LimaPlugin()
{
	if(m_calculationProcess)
		delete m_calculationProcess;
		
	if(m_deviceCCD)
		delete m_deviceCCD;
}
// ============================================================================
// SPECIFIC PLUGIN PROPERTIES
// ============================================================================
// ============================================================================
// LimaPlugin::enumerate_properties
// ============================================================================
void LimaPlugin::enumerate_properties (yat4tango::PlugInPropertyInfoList& propInfoList) const
throw (Tango::DevFailed)
{
	// fill propInfoList with name & type of plugin's needed properties
	try{
		propInfoList[m_limaDeviceAdressStr] = (int)yat::PlugInPropType::STRING;	
		propInfoList[m_percentageDetectionStr] =  (int)yat::PlugInPropType::DOUBLE;	
	}
	catch (...){
	  	/*m_sensorState.operational = false;
	  	m_sensorState.errorStatus = "LimaPlugin couldn't enumerate_properties for unknown reason";*/
	}
}

// ============================================================================
// LimaPlugin::updateStatus
// ============================================================================
void LimaPlugin::updateStatus(std::string message){

	if (m_sensorState.errorStatus != "")
		m_sensorState.errorStatus = m_sensorState.errorStatus + "\n" + message;
	else
		m_sensorState.errorStatus = message;
}
// ============================================================================
// LimaPlugin::set_properties
// ============================================================================
void LimaPlugin::set_properties (yat4tango::PlugInPropertyValueList& propValueList)
throw (Tango::DevFailed)
{
	// get properties' values from propValueList
	yat4tango::PlugInPropertyValueList::iterator propIt;

	bool proxyUrl = false;
	bool percentageDetection = false;

	for (propIt = propValueList.begin(); propIt != propValueList.end(); ++propIt){

	  	if ((*propIt).name == LIMA_DEVICE_ADRESS_STR){
		  try{
			// get value
			if ((*propIt).valid){
				proxyUrl = true;
				m_deviceAdress = yat::any_cast_ext<std::string>((*propIt).value);
			}
		  }
		  catch (...){
		  	m_propertyMissing = true;
			m_sensorState.operational = false;
			updateStatus("Looks like LimaDeviceUrl property is not in the right format");
		  }
	    }
		
		if ((*propIt).name == PERCENTAGE_DETECTION){
		  try{
			// get value
			if ((*propIt).valid){
			  percentageDetection = true;
			  m_percentageDetection = yat::any_cast_ext<double>((*propIt).value);
			}
		  }
		  catch (...){
		  	m_propertyMissing = true;
		  	m_sensorState.operational = false;
		  	updateStatus("Looks like percentageDetection property is not in the right format");
		  }
	    }
		
	}

    if (!percentageDetection){
      	// mandatory property
		m_propertyMissing = true;
		m_sensorState.operational = false;
		updateStatus("Lima plugin doesn't find property percentageDetection...");
		
    }
    if (!proxyUrl){
		// mandatory property
		m_propertyMissing = true;
		m_sensorState.operational = false;
		updateStatus("Lima plugin doesn't find property LimaDeviceUrl (need a proxy on lima device)");
    }
}
// ============================================================================
// LimaPlugin::enumerate_attributes
// ============================================================================
void LimaPlugin::enumerate_attributes (yat4tango::DynamicAttributeDescriptionList& attrDescList) const
throw (Tango::DevFailed)
{

}
// ============================================================================
// LimaPlugin::getSensorData
// ============================================================================
BPT::SensorInterface::SensorInterface::sensorData LimaPlugin::getSensorData()
  throw (Tango::DevFailed)
{
	BPT::SensorInterface::SensorInterface::sensorData dataToReturn;
	if (m_calculationProcess){
		try{
			//First try to refresh beam 
			if(m_calculationProcess->refreshBeam()){
				//Execute calculation, Update local Data
				CalculationResults results = m_calculationProcess->processBeam();
				if(results.m_beamShapefound){
					dataToReturn.xBeamPostionPixels = results.m_centroidCenterX;
					dataToReturn.yBeamPostionPixels = results.m_centroidCenterY;
					dataToReturn.imgHigh = results.m_imgHigh;
					dataToReturn.imgWidth = results.m_imgWidth;
					dataToReturn.tresholdedImg.clear();
					dataToReturn.tresholdedImg = results.tresholdedImg;
					dataToReturn.isBeamDetected = true;
					m_sensorState.operational = true;
				}	
				else{
					dataToReturn.xBeamPostionPixels = 0;
					dataToReturn.yBeamPostionPixels = 0;
					dataToReturn.isBeamDetected = false;
				}
			}else{
				m_sensorState.operational = false;
				m_sensorState.errorStatus = "Processing error, not able to get new image !\
							 Check Lima Device ! : " + m_deviceAdress;
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
// LimaPlugin::getState
// ============================================================================
BPT::SensorInterface::SensorInterface::sensorState LimaPlugin::getSensorState()
  throw (Tango::DevFailed)
{
	return m_sensorState;
}

// ============================================================================
// LimaPlugin::start
// ============================================================================
void LimaPlugin::start(Tango::DeviceImpl * host_device)
  throw (Tango::DevFailed)
{
	try{
		if(!m_propertyMissing){
			initialize_connection();
			//Build calculation with minimum setting configuration
			if (m_proxyCCDInit){
				m_calculationProcess = new CalculationProcess::CalculationProcess ( m_deviceCCD, m_percentageDetection);
				if(m_calculationProcess->checkDetectorPixelDepth()){
					m_sensorState.operational = true;
					m_sensorState.errorStatus = "Idle ready to start processing";
				}else{
					m_sensorState.operational = false;
					m_sensorState.errorStatus = "Lima's plugin can't operate with this detector \nPixel depth not supported !";
				}

			}
		}
	}
	catch (...){
		m_calculationProcess = NULL;
		m_sensorState.operational = false;
		m_sensorState.errorStatus = "Something went wrong with lima's plugin initilialisation!";
    }
}

// ============================================================================
// LimaPlugin::initialize_connection
// ============================================================================
void LimaPlugin::initialize_connection ()
{
	try{
		m_deviceCCD = new Tango::DeviceProxy (m_deviceAdress);
		Tango::DeviceData test  = m_deviceCCD->command_inout("Status");
		std::string statusOut;
		test>>statusOut;
		m_proxyCCDInit = true;
	}
	catch (...){
		m_deviceCCD = NULL;
		m_proxyCCDInit = false;
		m_sensorState.operational = false;
		m_sensorState.errorStatus = "Lima plugin couldn't instanciate deviceCCD's proxy, check adress :\n" + m_deviceAdress;
	}
}
}//namespace LimaPlugin
