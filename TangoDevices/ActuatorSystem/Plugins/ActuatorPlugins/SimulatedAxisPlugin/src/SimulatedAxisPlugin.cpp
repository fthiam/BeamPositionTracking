//=============================================================================
// SimulatedAxisPlugin.cpp
//=============================================================================
// abstraction.......Plugin to use SimulatedAxisDevice in BeamPositionTracking application
// class.............SimulatedAxisPlugin
// original author...thiam - SOLEIL
//=============================================================================

#include <SimulatedAxisPlugin.h>
#include <yat/plugin/PlugInSymbols.h>
#include <yat/utils/StringTokenizer.h>
#include <yat/threading/Thread.h>
#include <iostream>

EXPORT_SINGLECLASS_PLUGIN(SimulatedAxisPlugin::SimulatedAxisPlugin, SimulatedAxisPlugin::SimulatedAxisPluginInfo);

namespace SimulatedAxisPlugin
{
std::string SIMULTATED_AXIS_DEVICE_ADRESS_STR = "SimulatedAxisDeviceDeviceUrl";
std::string CMD_MOVE_AXIS_RELATIVE_STR = "MoveRelativeWithSpeed";
std::string CMD_STOP_MOVEMENT = "Stop";
// ============================================================================
// SimulatedAxisPlugin::SimulatedAxisPlugin
// ============================================================================
SimulatedAxisPlugin::SimulatedAxisPlugin()
: yat4tango::TangoLogAdapter(NULL)
{
	_simulatedAxisDevice = NULL;
	this->_stateStatus.isConnexionOperational = false;
}

// ============================================================================
// SimulatedAxisPlugin::~SimulatedAxisPlugin
// ============================================================================
SimulatedAxisPlugin::~SimulatedAxisPlugin()
{
	if(_simulatedAxisDevice)
		delete _simulatedAxisDevice;
	_simulatedAxisDevice = NULL;
}
// ============================================================================
// SPECIFIC PLUGIN PROPERTIES
// ============================================================================
// ============================================================================
// SimulatedAxisPlugin::enumerate_properties
// ============================================================================
void SimulatedAxisPlugin::enumerate_properties (yat4tango::PlugInPropertyInfoList& propInfoList) const
throw (Tango::DevFailed)
{
	//NO properties here ..
}
// ============================================================================
// SimulatedAxisPlugin::set_properties
// ============================================================================
void SimulatedAxisPlugin::set_properties (yat4tango::PlugInPropertyValueList& propValueList)
throw (Tango::DevFailed)
{
	//NO properties here ..
}
// ============================================================================
// SimulatedAxisPlugin::enumerate_attributes
// ============================================================================
void SimulatedAxisPlugin::enumerate_attributes (yat4tango::DynamicAttributeDescriptionList& attrDescList) const
throw (Tango::DevFailed)
{

}

// ============================================================================
// SimulatedAxisPlugin::initialize_connection
// ============================================================================
void SimulatedAxisPlugin::initialize_connection (std::string deviceAdress)
{
	this->_simulatedAxisDevice = new Tango::DeviceProxy (deviceAdress);
	Tango::DeviceData test  = this->_simulatedAxisDevice->command_inout("Status");

	std::string statusOut;
	test>>statusOut;
	this->_stateStatus.isConnexionOperational = true;

}
// ============================================================================
// SimulatedAxisPlugin::init
// ============================================================================
void SimulatedAxisPlugin::init(Tango::DeviceImpl * host_device, std::string deviceAdress) throw (Tango::DevFailed){	
  	
	this->initialize_connection(deviceAdress);
	if (this->_stateStatus.isConnexionOperational){
		this->_stateStatus.axisStatus = "Plugin is ready";
		this->_stateStatus.axisState = Tango::STANDBY;
	}
}

// ============================================================================
// SimulatedAxisPlugin::getAxisCurrentPosition
// ============================================================================
double SimulatedAxisPlugin::getAxisCurrentPosition()throw (Tango::DevFailed){
	Tango::DevShort positionValue = NULL;
	//First check if connexion is still alive
	this->updateStateStatus();
	if (_stateStatus.isConnexionOperational){
		Tango::DeviceAttribute positionAttr = this->_simulatedAxisDevice->read_attribute("RPositionMoteur");
		positionAttr >> positionValue;
	}
	
	return (double)positionValue;
}

// ============================================================================
// SimulatedAxisPlugin::setAxisPosition
// ============================================================================
void SimulatedAxisPlugin::setAxisPosition(double position)throw (Tango::DevFailed){

	if (_stateStatus.isConnexionOperational /*&& Normaly check if axe is moving, but its fake anyway*/){
		_stateStatus.axisState = Tango::MOVING;
		_stateStatus.axisStatus = "Movement is starting..";
		
		double currentPosition = this->getAxisCurrentPosition();
		double relativeMovement = 0;
		//Need to estimate relative movement !
		
		relativeMovement = position - currentPosition;
		 
		moveAxisRelative(relativeMovement);
	}
}
// ============================================================================
// SimulatedAxisPlugin::moveAxisRelative
// ============================================================================
void SimulatedAxisPlugin::moveAxisRelative(double relativeMovement)throw (Tango::DevFailed){
	//First check if connexion is still alive
	this->updateStateStatus();
	if (_stateStatus.isConnexionOperational /*&& Normaly check if axe is moving, but its fake anyway*/){
		_stateStatus.axisState = Tango::MOVING;
		_stateStatus.axisStatus = "Movement is starting..";

		Tango::DevVarShortArray* w_argsStepsSpeed = new Tango::DevVarShortArray();
		w_argsStepsSpeed->length(2);
		(*w_argsStepsSpeed)[0] = (short)relativeMovement;
		(*w_argsStepsSpeed)[1] = 1;
		
		Tango::DeviceData relativeMovmentData;
		relativeMovmentData<<w_argsStepsSpeed;

		sendCommandToActuatorSystem(CMD_MOVE_AXIS_RELATIVE_STR, true, relativeMovmentData);
	}
}
// ============================================================================
// SimulatedAxisPlugin::stopAxisMovement
// ============================================================================
void SimulatedAxisPlugin::stopAxisMovement()throw (Tango::DevFailed){
	Tango::DeviceData emptyData;
	sendCommandToActuatorSystem(CMD_STOP_MOVEMENT, false, emptyData);
}
// ============================================================================
// SimulatedAxisPlugin::getAxisStatus
// ============================================================================
AS::ActuatorInterface::ActuatorInterface::AxisStateStatus SimulatedAxisPlugin::getAxisStateStatus()throw (Tango::DevFailed){
	this->updateStateStatus();
	return this->_stateStatus;
}
// ============================================================================
// SimulatedAxisPlugin::updateStateStatus
// ============================================================================
void SimulatedAxisPlugin::updateStateStatus(){
	try{
		_stateStatus.axisState = this->_simulatedAxisDevice->state();
		//in our case -> standby means ON
		if(_stateStatus.axisState == Tango::STANDBY)
			_stateStatus.axisState = Tango::ON;
		_stateStatus.axisStatus = this->_simulatedAxisDevice->status();
		_stateStatus.isConnexionOperational = true;

	}catch(...){
		_stateStatus.isConnexionOperational = false;
	}
}
// ============================================================================
// SimulatedAxisPlugin::sendCommandToActuatorSystem
// ============================================================================
void SimulatedAxisPlugin::sendCommandToActuatorSystem(std::string cmdName, bool args, Tango::DeviceData argsData){
	if (args)
		this->_simulatedAxisDevice->command_inout(cmdName, argsData);
	else
		this->_simulatedAxisDevice->command_inout(cmdName);
}
}// namespace SimulatedAxisPlugin
