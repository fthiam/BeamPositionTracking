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
std::string FIXED_PERCENTAGE_DETECTION_STR = "FixedPercentageDetection";
const static string percentageDetectionStr = "percentageDetection";
// ============================================================================
// LimaPlugin::LimaPlugin
// ============================================================================
LimaPlugin::LimaPlugin()
: yat4tango::TangoLogAdapter(NULL)
{
	//Init members
	m_limaDeviceAdressStr = "LimaDeviceUrl";
	m_thresholdStr = "FixedPercentageDetection";
	m_sensorState.operational = false;
	m_sensorState.errorStatus = "Not initialized";
	m_calculationProcess = 0;
	m_propertyMissing = false;
	m_proxyCCDInit = false;
	m_xCentroid = yat::ieee_nan();
	m_yCentroid = yat::ieee_nan();
	m_isBeamDetected = false;
	m_deviceCCDAdress = "";
	m_deviceCCD = NULL;
	m_percentageDetection = 1;
	m_fixMode = true;
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
		propInfoList[m_thresholdStr] = (int)yat::PlugInPropType::DOUBLE;	
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
	bool fixedPercentageDetection = false;

	for (propIt = propValueList.begin(); propIt != propValueList.end(); ++propIt){

	  	if ((*propIt).name == LIMA_DEVICE_ADRESS_STR){
		  try{
			// get value
			if ((*propIt).valid){
				proxyUrl = true;
				m_deviceCCDAdress = yat::any_cast_ext<std::string>((*propIt).value);
			}
		  }
		  catch (...){
		  	m_propertyMissing = true;
			m_sensorState.operational = false;
			updateStatus("Looks like LimaDeviceUrl property is not in the right format");
		  }
	    }	

 		if ((*propIt).name == FIXED_PERCENTAGE_DETECTION_STR){
		  try{
			// get value
			if ((*propIt).valid){
				fixedPercentageDetection = true;
				m_percentageDetection = yat::any_cast<double>((*propIt).value);
			}
		  }
		  catch (...){
		  	m_propertyMissing = true;
			m_sensorState.operational = false;
			updateStatus("Looks like percentageDetection property is not in the right format");
		  }
	    }			
	}
    if (!proxyUrl){
		// mandatory property
		m_propertyMissing = true;
		m_sensorState.operational = false;
		updateStatus("Lima plugin doesn't find property LimaDeviceUrl (need a proxy on lima device)");
    }
    if (!fixedPercentageDetection){
		// mandatory property
		m_propertyMissing = true;
		m_sensorState.operational = false;
		updateStatus("Lima plugin doesn't find property FixedPercentageDetection");
    }
}
// ============================================================================
// LimaPlugin::percentageDetection_attribute_present
// ============================================================================
void LimaPlugin::percentageDetection_attribute_present()
{
  try
  {
   	this->m_deviceCCD->read_attribute ("percentageDetection");
    this->m_percentageDetectionPresent = true;
  }
  //An exception means that externalRefEnabled doesn't exists...
  //No need to create attribute ExternalRefEnabled in plugin
  catch (Tango::ConnectionFailed ConF)
  {
    this->m_percentageDetectionPresent = false;
  }  
  catch (Tango::CommunicationFailed  ComF)
  {
    this->m_percentageDetectionPresent = false;
  }  
  catch (...)
  {
    this->m_percentageDetectionPresent = false;
  }  
}
// ============================================================================
// LimaPlugin::enumerate_attributes
// ============================================================================
void LimaPlugin::enumerate_attributes (yat4tango::DynamicAttributeDescriptionList& attrDescList) const
throw (Tango::DevFailed)
{
	//- attribute definition:
	yat4tango::DynamicAttributeInfo dai;
	// dai.dev = this;

	dai.tai.name = percentageDetectionStr;
	dai.tai.label = percentageDetectionStr; 
	dai.tai.data_format = Tango::SCALAR;
	dai.tai.data_type = Tango::DEV_DOUBLE;
	dai.tai.disp_level = Tango::OPERATOR;
	dai.tai.writable = Tango::READ_WRITE;
	dai.tai.description = "Image threshold lvl";

	//- attribute properties:
	dai.tai.unit = " ";
	dai.tai.standard_unit = " ";
	dai.tai.display_unit = " ";
	dai.tai.format = "%6.2f";

	dai.cdb = true;

	//- read callback
	dai.rcb = yat4tango::DynamicAttributeReadCallback::instanciate(const_cast<LimaPlugin&>(*this), 
	                                                          &LimaPlugin::read_callback_percentageDetection);

	dai.wcb = yat4tango::DynamicAttributeWriteCallback::instanciate(const_cast<LimaPlugin&>(*this),
	                                                            &LimaPlugin::write_callback_percentageDetection);

	//- add attribute description in list
	attrDescList.push_back(dai);
}
// ============================================================================
// LimaPlugin::write_callback_externalThreshold
// ============================================================================
void LimaPlugin::write_callback_percentageDetection(yat4tango::DynamicAttributeWriteCallbackData& cbd)
{
	if(!m_fixMode){
	    cbd.tga->get_write_value(m_percentageDetection);
	    setPercentageDetection(m_percentageDetection);
	}else{
		THROW_DEVFAILED(
        _CPTC("PLUGIN_ERROR"), 
        _CPTC("Cannot write percentage detection in fixed mode \nSet value on property : FixPercentageDetection"),
        _CPTC("LimaPlugin::write_callback_percentageDetection()")); 
	}
}
// ============================================================================
// LimaPlugin::read_callback_percentageDetection
// ============================================================================
void LimaPlugin::read_callback_percentageDetection(yat4tango::DynamicAttributeReadCallbackData& cbd)
{
	if (this->m_percentageDetectionPresent)
		cbd.tga->set_value(&m_percentageDetection);
	else
		cbd.tga->set_quality(Tango::AttrQuality::ATTR_INVALID, true);
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
					dataToReturn.thresholdedImg.clear();
					dataToReturn.thresholdedImg = results.thresholdedImg;
					dataToReturn.isBeamDetected = true;
					m_sensorState.operational = true;
				}	
				else{
					//Only use image ...
					dataToReturn.imgHigh = results.m_imgHigh;
					dataToReturn.imgWidth = results.m_imgWidth;
					dataToReturn.thresholdedImg.clear();
					dataToReturn.thresholdedImg = results.thresholdedImg;
					dataToReturn.xBeamPostionPixels = 0;
					dataToReturn.yBeamPostionPixels = 0;
					dataToReturn.isBeamDetected = false;
				}
			}else{
				m_sensorState.operational = false;
				m_sensorState.errorStatus = "Processing error, not able to get new image !\
							 Check Lima Device ! : " + m_deviceCCDAdress;
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
void LimaPlugin::start(Tango::DeviceImpl * host_device, bool fixMode)
  throw (Tango::DevFailed)
{
	m_fixMode = fixMode;
	try{
		if(!m_propertyMissing){
			initialize_connection();
			//Build calculation with minimum setting configuration
			if (m_proxyCCDInit){
				m_calculationProcess = new CalculationProcess::CalculationProcess ( m_deviceCCD, m_percentageDetection);
				if(m_calculationProcess->checkDetectorPixelDepth()){
					//If fixed mode -> set threshold once and for all
					std::cout<<"start, with m_percentageDetection = "<<m_percentageDetection<<std::endl;
					//Init percentage detection with property value 
					setPercentageDetection(m_percentageDetection);
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
		//Init CCD device proxy
		m_deviceCCD = new Tango::DeviceProxy (m_deviceCCDAdress);

		Tango::DeviceData test  = m_deviceCCD->command_inout("Status");
		percentageDetection_attribute_present();
		std::string statusOut;
		test>>statusOut;

		m_proxyCCDInit = true;
	}
	catch (...){
		m_deviceCCD = NULL;
		m_proxyCCDInit = false;
		m_sensorState.operational = false;
		m_sensorState.errorStatus = "Lima plugin couldn't instanciate deviceCCD's proxy, check adress :\n" + m_deviceCCDAdress;
	}
}
// ============================================================================
// LimaPlugin::setPercentageDetection
// ============================================================================
void LimaPlugin::setPercentageDetection(double percentageDetection){
	m_percentageDetection = percentageDetection;
	double localPercentageDetection =  m_percentageDetection/100;

 	if(localPercentageDetection > 1)
 		localPercentageDetection = 1;
 	if(localPercentageDetection < 0)
 		localPercentageDetection = 0;

    if((localPercentageDetection <= 1) && (localPercentageDetection >= 0)) {
	    //Attribute to write in device if present...
	    if(m_calculationProcess)
	       this->m_calculationProcess->updatePercentageDetection(localPercentageDetection);	
	}else{
		  THROW_DEVFAILED(
        _CPTC("PLUGIN_ERROR"), 
        _CPTC("Invalid value for percentage detection, \nmust be between 0 - 100 !"),
        _CPTC("LimaPlugin::write_callback_percentageDetection()")); 
	}
}
}//namespace LimaPlugin
