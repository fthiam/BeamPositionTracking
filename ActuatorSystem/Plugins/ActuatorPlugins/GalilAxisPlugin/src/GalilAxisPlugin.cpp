//=============================================================================
// GalilAxisPlugin.cpp
//=============================================================================
// abstraction.......Plugin to use galilAxisDevice in BeamPositionTracking application
// class.............GalilAxisPlugin
// original author...thiam - SOLEIL
//=============================================================================

#include <GalilAxisPlugin.h>

EXPORT_SINGLECLASS_PLUGIN(GalilAxisPlugin::GalilAxisPlugin, GalilAxisPlugin::GalilAxisPluginInfo);



namespace GalilAxisPlugin
{
std::string SIMULTATED_AXIS_DEVICE_ADRESS_STR = "galilAxisDeviceDeviceUrl";
std::string CMD_MOVE_AXIS_RELATIVE_STR = "MoveRelativeWithSpeed";
std::string CMD_STOP_MOVEMENT = "Stop";
std::string ATTR_POSITION_STR = "position";
// ============================================================================
// GalilAxisPlugin::GalilAxisPlugin
// ============================================================================
GalilAxisPlugin::GalilAxisPlugin()
: yat4tango::TangoLogAdapter(NULL)
{
}

// ============================================================================
// GalilAxisPlugin::~GalilAxisPlugin
// ============================================================================
GalilAxisPlugin::~GalilAxisPlugin()
{

}
// ============================================================================
// SPECIFIC PLUGIN PROPERTIES
// ============================================================================
// ============================================================================
// GalilAxisPlugin::enumerate_properties
// ============================================================================
void GalilAxisPlugin::enumerate_properties (yat4tango::PlugInPropertyInfoList& propInfoList) const
throw (Tango::DevFailed)
{
	//NO properties here ..
}
// ============================================================================
// GalilAxisPlugin::set_properties
// ============================================================================
void GalilAxisPlugin::set_properties (yat4tango::PlugInPropertyValueList& propValueList)
throw (Tango::DevFailed)
{
	//NO properties here ..
}
// ============================================================================
// GalilAxisPlugin::enumerate_attributes
// ============================================================================
void GalilAxisPlugin::enumerate_attributes (yat4tango::DynamicAttributeDescriptionList& attrDescList) const
throw (Tango::DevFailed)
{

}

// ============================================================================
// GalilAxisPlugin::initialize_connection
// ============================================================================
void GalilAxisPlugin::initialize_connection (std::string deviceAdress)
{
	this->_galilAxisDevice = new Tango::DeviceProxy (deviceAdress);
	Tango::DeviceData test  = this->_galilAxisDevice->command_inout("Status");
	std::string statusOut;
	test>>statusOut;
	this->_stateStatus.isConnexionOperational = true;
}
// ============================================================================
// GalilAxisPlugin::start
// ============================================================================
void GalilAxisPlugin::init(Tango::DeviceImpl * host_device, std::string deviceAdress)
  throw (Tango::DevFailed)
{	
	this->initialize_connection(deviceAdress);
	this->_stateStatus.axisStatus = "GalilAxis plugin is ready";
	this->_stateStatus.axisState = Tango::STANDBY;
	this->_deviceURLAdress = deviceAdress;
}

// ============================================================================
// GalilAxisPlugin::getAxisCurrentPosition
// ============================================================================
double GalilAxisPlugin::getAxisCurrentPosition()throw (Tango::DevFailed)
{
	Tango::DevDouble positionValue = NULL;
	//First check if connexion is still alive
	this->updateStateStatus();
	if (_stateStatus.isConnexionOperational){
		Tango::DeviceAttribute positionAttr = this->_galilAxisDevice->read_attribute("position");
		positionAttr >> positionValue;
	}
	
	return (double)positionValue;
}
// ============================================================================
// GalilAxisPlugin::setAxisPosition
// ============================================================================
void GalilAxisPlugin::setAxisPosition(double position)throw (Tango::DevFailed){
	Tango::DeviceAttribute* positionAttribute;
	this->updateStateStatus();
	if (_stateStatus.isConnexionOperational /*&& Normaly check if axe is moving, but its fake anyway*/){
		_stateStatus.axisState = Tango::MOVING;
		_stateStatus.axisStatus = "Movement is starting..";
		//positionAttribute = new Tango::DeviceAttribute(ATTR_POSITION_STR, position);
		//For firsts tests on beamline
		std::cout<<"Suppose to move axis "<<_deviceURLAdress<<", with value : "<<position<<std::endl;
		//this->_galilAxisDevice->write_attribute(*positionAttribute);
	}
}
// ============================================================================
// GalilAxisPlugin::moveAxisRelative
// ============================================================================
void GalilAxisPlugin::moveAxisRelative(double relativeMovement)throw (Tango::DevFailed){
	//First check if connexion is still alive
	this->updateStateStatus();
	if (_stateStatus.isConnexionOperational /*&& Normaly check if axe is moving, but its fake anyway*/){
		_stateStatus.axisState = Tango::MOVING;
		_stateStatus.axisStatus = "Movement is starting..";

		double position = this->getAxisCurrentPosition();
		double positionToWrite = position + relativeMovement;

		Tango::DeviceAttribute* positionAttribute;

		//positionAttribute = new Tango::DeviceAttribute(ATTR_POSITION_STR, positionToWrite);
		//For firsts tests on beamline
		std::cout<<"Suppose to move axis "<<_deviceURLAdress<<", with value : "<<positionToWrite<<std::endl;
		//this->_galilAxisDevice->write_attribute(*positionAttribute);
	}
}
// ============================================================================
// GalilAxisPlugin::stopAxisMovement
// ============================================================================
void GalilAxisPlugin::stopAxisMovement()throw (Tango::DevFailed){
	Tango::DeviceData emptyData;
	sendCommandToActuatorSystem(CMD_STOP_MOVEMENT, false, emptyData);
}
// ============================================================================
// GalilAxisPlugin::getAxisStatus
// ============================================================================
AS::ActuatorInterface::ActuatorInterface::AxisStateStatus GalilAxisPlugin::getAxisStateStatus()throw (Tango::DevFailed){
	this->updateStateStatus();
	return this->_stateStatus;
}
// ============================================================================
// GalilAxisPlugin::updateStateStatus
// ============================================================================
void GalilAxisPlugin::updateStateStatus(){

	_stateStatus.axisState = this->_galilAxisDevice->state();
	//in our case -> standby means ON
	if(_stateStatus.axisState == Tango::STANDBY)
		_stateStatus.axisState = Tango::ON;
	_stateStatus.axisStatus = this->_galilAxisDevice->status();
}
// ============================================================================
// GalilAxisPlugin::sendCommandToActuatorSystem
// ============================================================================
void GalilAxisPlugin::sendCommandToActuatorSystem(std::string cmdName, bool args, Tango::DeviceData argsData){
	if (args)
		this->_galilAxisDevice->command_inout(cmdName, argsData);
	else
		this->_galilAxisDevice->command_inout(cmdName);
}
}// namespace GalilAxisPlugin
