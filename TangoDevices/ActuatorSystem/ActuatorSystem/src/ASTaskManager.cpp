// ============================================================================
//
// = CONTEXT
//    TANGO Project -
//
// = FILENAME
//    ASTaskManager.cpp
//
// = AUTHOR
//    F. Thiam
//
// ============================================================================
// ============================================================================
// DEPENDENCIES
// ============================================================================
#include "ASTaskManager.h"

static const unsigned short kPERIODIC_TMOUT_MS = 10;


namespace ActuatorSystem_ns{

std::string X_AXIS_ID = "xAxis";
std::string Y_AXIS_ID = "yAxis";
// ============================================================================
// SOME USER DEFINED MESSAGES
// ============================================================================
#define kALIGNMENT_PROCESS  			(yat::FIRST_USER_MSG + 1001)
#define kSET_X_AXIS_CALIBRATION			(yat::FIRST_USER_MSG + 1002)
#define kSET_Y_AXIS_CALIBRATION			(yat::FIRST_USER_MSG + 1003)
#define kMOVE_X_AXIS					(yat::FIRST_USER_MSG + 1004)
#define kMOVE_Y_AXIS					(yat::FIRST_USER_MSG + 1006)
#define kSTOP_AXES						(yat::FIRST_USER_MSG + 1007)
#define kSET_X_AXIS_CALIBRATION_STATE	(yat::FIRST_USER_MSG + 1008)
#define kSET_Y_AXIS_CALIBRATION_STATE	(yat::FIRST_USER_MSG + 1009)
#define kMOVE_AXES						(yat::FIRST_USER_MSG + 1010)
#define kSET_X_AXIS_POSITION			(yat::FIRST_USER_MSG + 1011)
#define kSET_Y_AXIS_POSITION			(yat::FIRST_USER_MSG + 1012)

int MAX_CALIBRATION_TRY_NB = 100;
size_t DEFAULT_ACTUATOR_SYSTEM_TIMEOUT = 30000;

// ============================================================================
// ASTaskManager::ASTaskManager
// ============================================================================
ASTaskManager::ASTaskManager (	Tango::DeviceImpl* devHost, yat4tango::DynamicAttributeManager* dynAttrManager,
								std::string xAxisPluginType, std::string xAxisDeviceAdress,
								std::string yAxisPluginType, std::string yAxisDeviceAdress, 
								std::string xPluginPath, std::string yPluginPath,
								int xAxisMinPositionInt, int xAxisMaxPositionInt,
								int yAxisMinPositionInt, int yAxisMaxPositionInt,
								bool simulatedMode)
:yat4tango::DeviceTask(devHost),
_dynAttrManager(dynAttrManager),
m_hostDev(devHost), 
m_xAxisPluginType(xAxisPluginType),
m_xAxisDeviceAdress(xAxisDeviceAdress),
m_yAxisPluginType(yAxisPluginType),
m_yAxisDeviceAdress(yAxisDeviceAdress),
m_xPluginPath(xPluginPath),
m_yPluginPath(yPluginPath),
m_xAxisMinPosition(xAxisMinPositionInt),
m_xAxisMaxPosition(xAxisMaxPositionInt),
m_yAxisMinPosition(yAxisMinPositionInt),
m_yAxisMaxPosition(yAxisMaxPositionInt),
m_simulatedMode(simulatedMode)
{
    //Initialize member variables
	yat::MutexLock gardDataMutex(m_dataMutex);
	m_isXAxisCalibrated = false;
    m_isYAxisCalibrated = false;
    m_movementStarted = false;
    m_initCompleted = false;
    m_simulatedXTranslation = 0;
    m_simulatedYTranslation = 0;
    m_axesPositionningState.xAxisPositionOutOfBounds = false;
	m_axesPositionningState.yAxisPositionOutOfBounds = false;
    m_axesPositionningState.xLimitReached = false;
    m_axesPositionningState.yLimitReached = false;
    m_axesPositionningState.xLimitStatus = "";
    m_axesPositionningState.yLimitStatus = "";

 	enable_periodic_msg(false);
	enable_timeout_msg(false);

	m_pluginFactory = NULL;
	m_pluginInfo = NULL;
    m_xActuatorPlugin = NULL;
	m_yActuatorPlugin = NULL;
}
// ============================================================================
// ASTaskManager::~ASTaskManager
// ============================================================================
ASTaskManager::~ASTaskManager ()
{
	// Wait for the end of critical sections

	enable_periodic_msg(false);
	yat::MutexLock gardDataMutex(m_dataMutex);

	delete this->m_xActuatorPlugin;
	
	delete this->m_yActuatorPlugin;
	
	m_initCompleted = false;

}
// ============================================================================
// ASTaskManager::process_message
// ============================================================================
void ASTaskManager::process_message (yat::Message& _msg)
  throw (Tango::DevFailed)
{
    //DEBUG_STREAM << "ASTaskManager::process_message::receiving msg " <<_msg.to_string() << std::endl;
	//- handle msg
	switch (_msg.type())
	{
		//- TASK_INIT ----------------------
	    case yat::TASK_INIT:
	    {
	        DEBUG_STREAM << "ASTaskManager::handle_message::TASK_INIT::thread is starting up" << std::endl;

	        StateStatus stateStatus;
			stateStatus.state = Tango::INIT;
			stateStatus.status = "Initializing ASTaskManager";
			updateTaskStateStatus(stateStatus);
	        //Initialize periodic task 
	        enable_timeout_msg(false);
	        set_timeout_msg_period (500);
	        set_periodic_msg_period(kPERIODIC_TMOUT_MS);
	        enable_periodic_msg(false);  
	        //Try to create both plugins : X and Y axis
	        if (actuatorPluginInitialisation(m_xAxisPluginType, m_xPluginPath, X_AXIS_ID) && 
	        	actuatorPluginInitialisation(m_yAxisPluginType, m_yPluginPath, Y_AXIS_ID)){
	        	//if creation is ok then start them
	        	if(startPlugins()){
	        		//Enable periodic
					enable_periodic_msg(true);	
					stateStatus.state = Tango::STANDBY;
					stateStatus.status = "Initialisation done, ready to start ...";
					updateTaskStateStatus(stateStatus);
					m_initCompleted = true;
	        	} 
	        }
	    } 

	    //- TASK_PERIODIC ------------------
	    case yat::TASK_PERIODIC:
	    {
	    	DEBUG_STREAM << "ASTaskManager::handle_message::TASK_PERIODIC" << std::endl;
	    	//Refresh axes states/Positions
	    	if(m_initCompleted)
	    		if(refreshAxes() && !m_movementStarted)
	    			refreshTaskState();
	    }
	    break;
	    //- kSET_X_AXIS_CALIBRATION_STATE ------------------
	    case kSET_X_AXIS_CALIBRATION_STATE:
	    {
	    	DEBUG_STREAM << "ASTaskManager::handle_message::kSET_X_AXIS_CALIBRATION_STATE" << std::endl;
	    	bool *state;
	        _msg.detach_data(state);      
	        m_isXAxisCalibrated = *state;
	    }
	    break;
	    //- kSET_Y_AXIS_CALIBRATION_STATE ------------------
	    case kSET_Y_AXIS_CALIBRATION_STATE:
	    {
	    	DEBUG_STREAM << "ASTaskManager::handle_message::kSET_Y_AXIS_CALIBRATION_STATE" << std::endl;
	    	bool *state;
	        _msg.detach_data(state);      
	        m_isYAxisCalibrated = *state;
	    }
	    break;

		//- kMOVE_X_AXIS ------------------
	    case kMOVE_X_AXIS:
	    {
	    	DEBUG_STREAM << "ASTaskManager::handle_message::kMOVE_X_AXIS" << std::endl;

	    	double *relativeMovement;
	        _msg.detach_data(relativeMovement);  

	        moveAxisPosition(*relativeMovement, X_AXIS_ID);
			m_movementStarted = false;
	    }
	    break;
	    //- kMOVE_Y_AXIS ------------------
	    case kMOVE_Y_AXIS:
	    {
	    	DEBUG_STREAM << "ASTaskManager::handle_message::kMOVE_Y_AXIS" << std::endl;

	    	double *relativeMovement;
	        _msg.detach_data(relativeMovement);  

	        moveAxisPosition(*relativeMovement, Y_AXIS_ID);
			m_movementStarted = false;
	    }
	    break;
	    //- kSET_X_AXIS_POSITION ------------------
	    case kSET_X_AXIS_POSITION:
	    {
	    	DEBUG_STREAM << "ASTaskManager::handle_message::kSET_X_AXIS_POSITION" << std::endl;

	    	double *position;
	        _msg.detach_data(position);  

	        setAxisPosition(*position, X_AXIS_ID);
			m_movementStarted = false;
	    }
	    break;
	    //- kSET_Y_AXIS_POSITION ------------------
	    case kSET_Y_AXIS_POSITION:
	    {
	    	DEBUG_STREAM << "ASTaskManager::handle_message::kMOVE_Y_AXIS" << std::endl;

	    	double *position;
	        _msg.detach_data(position);  

	        setAxisPosition(*position, Y_AXIS_ID);
			m_movementStarted = false;
	    }
	    break;
		//- kMOVE_AXES ------------------
	    case kMOVE_AXES:
	    {
	    	DEBUG_STREAM << "ASTaskManager::handle_message::kMOVE_AXES" << std::endl;

	    	double *axesTranslations;
			_msg.detach_data(axesTranslations);   

			//Asynchronous post for both axes
			yat::Message *xMovementMsg = new yat::Message(kMOVE_X_AXIS);
			xMovementMsg->attach_data(axesTranslations[0]);  
			post(xMovementMsg);
			yat::Message *yMovementMsg = new yat::Message(kMOVE_Y_AXIS);
			yMovementMsg->attach_data(axesTranslations[1]);  
			post(yMovementMsg);

	    }
	    break;
	    //- kSTOP_AXES ------------------
	    case kSTOP_AXES:
	    {
	    	DEBUG_STREAM << "ASTaskManager::handle_message::kMOVE_Y_AXIS" << std::endl;  
	        stopAxesMovement();
	    
	    }
	    break;
	    //- TASK_EXIT ----------------------
	    case yat::TASK_EXIT:
	    {
	        DEBUG_STREAM << "ASTaskManager::handle_message::TASK_EXIT::thread is quitting" << std::endl;
	        std::cout<<"task exit"<<std::endl;
	    }
	    break;

	    //- THREAD_TIMEOUT -------------------
	    case yat::TASK_TIMEOUT:
	    {
	    
	    }
	    break;

	    //- UNHANDLED MSG --------------------
	    default:
		
	      DEBUG_STREAM << "ASTaskManager::handle_message::unhandled msg type received" << std::endl;
	    break;
    }
}
//-----------------------------------------------------------------------------
//
//
//					TASK - PUBLIC COMANDS
//
//
//-----------------------------------------------------------------------------
//+----------------------------------------------------------------------------
//
// method :  BPTTaskManager::i_setXAxisCalibrationState(bool state)
//
//-----------------------------------------------------------------------------
void ASTaskManager::i_setXAxisCalibrationState(bool state)throw (Tango::DevFailed){

	if(getTaskCurrentState().state != Tango::MOVING){
		yat::Message *xCalibrationStateMsg = new yat::Message(kSET_X_AXIS_CALIBRATION_STATE);
		xCalibrationStateMsg->attach_data(state);
		post(xCalibrationStateMsg);	
	}else{
		THROW_DEVFAILED("DEVICE_ERROR",
			            "System can't change calibration state in moving state",
		              	"ASTaskManager::i_setXAxisCalibrationState");
	}
}
//+----------------------------------------------------------------------------
//
// method :  BPTTaskManager::i_setYAxisCalibrationState(bool state)
//
//-----------------------------------------------------------------------------
void ASTaskManager::i_setYAxisCalibrationState(bool state)throw (Tango::DevFailed){

	if(getTaskCurrentState().state != Tango::MOVING){
		yat::Message *yCalibrationStateMsg = new yat::Message(kSET_Y_AXIS_CALIBRATION_STATE);
		yCalibrationStateMsg->attach_data(state);
		post(yCalibrationStateMsg);	
	}else{
		THROW_DEVFAILED("DEVICE_ERROR",
			            "System can't change calibration state in moving state",
		              	"ASTaskManager::i_setYAxisCalibrationState");
	}
}
//+----------------------------------------------------------------------------
//
// method :  BPTTaskManager::i_moveAxes(int xAxisRelativeMovement, int yAxisRelativeMovement)
//
//-----------------------------------------------------------------------------
void ASTaskManager::i_moveAxes(double xAxisRelativeMovement, double yAxisRelativeMovement){

	if(getTaskCurrentState().state != Tango::MOVING){
	   m_movementStarted = true; 
		StateStatus movementStartStateStatus;
		movementStartStateStatus.state = Tango::MOVING;
		movementStartStateStatus.status = "Movement on X/Y axis is starting...";
		updateTaskStateStatus(movementStartStateStatus);
		yat::Message *xMovementMsg = new yat::Message(kMOVE_AXES, MAX_USER_PRIORITY, true);
		double axesTranslations[2];
		axesTranslations[0] = xAxisRelativeMovement;
		axesTranslations[1] = yAxisRelativeMovement;
		xMovementMsg->attach_data(axesTranslations);
		//Sending message synchronously
		wait_msg_handled(xMovementMsg, DEFAULT_ACTUATOR_SYSTEM_TIMEOUT);
	}else{
		THROW_DEVFAILED("DEVICE_ERROR",
			            "System can't move an axis while moving state",
		              	"ASTaskManager::i_writeXAxisPosition");
	}
}
//+----------------------------------------------------------------------------
//
// method :  BPTTaskManager::i_moveXAxis(double movement)
//
//-----------------------------------------------------------------------------
void ASTaskManager::i_moveXAxis(double movement){

	if(getTaskCurrentState().state != Tango::MOVING){
		m_movementStarted = true; 
		StateStatus movementStartStateStatus;
		movementStartStateStatus.state = Tango::MOVING;
		movementStartStateStatus.status = "Movement on X axis is starting...";
		updateTaskStateStatus(movementStartStateStatus);
		yat::Message *xMovementMsg = new yat::Message(kMOVE_X_AXIS, MAX_USER_PRIORITY, true);
		xMovementMsg->attach_data(movement);
		//Sending message synchronously
		wait_msg_handled(xMovementMsg, DEFAULT_ACTUATOR_SYSTEM_TIMEOUT);
	}else{
		THROW_DEVFAILED("DEVICE_ERROR",
			            "System can't move an axis while moving state",
		              	"ASTaskManager::i_writeXAxisPosition");
	}
}
//+----------------------------------------------------------------------------
//
// method :  BPTTaskManager::i_moveYAxis(double movement)
//
//-----------------------------------------------------------------------------
void ASTaskManager::i_moveYAxis(double movement){

	if(getTaskCurrentState().state != Tango::MOVING){
	    m_movementStarted = true; 
		StateStatus movementStartStateStatus;
		movementStartStateStatus.state = Tango::MOVING;
		movementStartStateStatus.status = "Movement on Y axis is starting...";
		updateTaskStateStatus(movementStartStateStatus);
		yat::Message *yMovementMsg = new yat::Message(kMOVE_Y_AXIS, MAX_USER_PRIORITY, true);
		yMovementMsg->attach_data(movement);
		//Sending message synchronously
		wait_msg_handled(yMovementMsg, DEFAULT_ACTUATOR_SYSTEM_TIMEOUT);	
	}else{
		THROW_DEVFAILED("DEVICE_ERROR",
			            "System can't move an axis while moving state",
		              	"ASTaskManager::i_writeYAxisPosition");
	}
}
//+----------------------------------------------------------------------------
//
// method :  BPTTaskManager::i_setXAxisPosition(double position)
//
//-----------------------------------------------------------------------------
void ASTaskManager::i_setXAxisPosition(double position){
    m_movementStarted = true; 
	StateStatus movementStartStateStatus;
	movementStartStateStatus.state = Tango::MOVING;
	movementStartStateStatus.status = "Positionning X axis ...";
	updateTaskStateStatus(movementStartStateStatus);
	yat::Message *xAxisPositionMsg = new yat::Message(kSET_X_AXIS_POSITION, MAX_USER_PRIORITY, true);
	xAxisPositionMsg->attach_data(position);
	//Sending message synchronously
	wait_msg_handled(xAxisPositionMsg, DEFAULT_ACTUATOR_SYSTEM_TIMEOUT);
}
//+----------------------------------------------------------------------------
//
// method :  BPTTaskManager::i_setYAxisPosition(double position)
//
//-----------------------------------------------------------------------------
void ASTaskManager::i_setYAxisPosition(double position){
    m_movementStarted = true; 
	StateStatus movementStartStateStatus;
	movementStartStateStatus.state = Tango::MOVING;
	movementStartStateStatus.status = "Positionning Y axis ...";
	updateTaskStateStatus(movementStartStateStatus);
	yat::Message *yAxisPositionMsg = new yat::Message(kSET_Y_AXIS_POSITION, MAX_USER_PRIORITY, true);
	yAxisPositionMsg->attach_data(position);
	//Sending message synchronously
	wait_msg_handled(yAxisPositionMsg, DEFAULT_ACTUATOR_SYSTEM_TIMEOUT);	
}
//+----------------------------------------------------------------------------
//
// method :  BPTTaskManager::i_stopAxes()
//
//-----------------------------------------------------------------------------
void ASTaskManager::i_stopAxes(){
	yat::Message *stopAxesMessage = new yat::Message(kSTOP_AXES, MAX_USER_PRIORITY, true);
	wait_msg_handled(stopAxesMessage, DEFAULT_ACTUATOR_SYSTEM_TIMEOUT);	
}

//-----------------------------------------------------------------------------
//
//
//					TASK - PUBLIC GETTERS
//
//
//-----------------------------------------------------------------------------
ASTaskManager::TaskManagerDataPacket ASTaskManager::i_getManagerDataPacket(){

	yat::MutexLock gardDataMutex(m_dataMutex);
	return m_managerDataPacket;
}

//----------------------------------------------------------------------------
//
//
//				TASK - PRIVATE METHODS
//
//
//-----------------------------------------------------------------------------
//+----------------------------------------------------------------------------
//
// method :  ASTaskManager::stopAxisMovement()
// 
// description : 
//
//-----------------------------------------------------------------------------
void ASTaskManager::stopAxesMovement(){
	StateStatus stopCmd;
	if(!m_simulatedMode){
		try{
			m_xActuatorPlugin->stopAxisMovement();
			m_yActuatorPlugin->stopAxisMovement();
		}catch(...){
			stopCmd.state = Tango::FAULT;
			stopCmd.status = "Couldn't stop axes";
			updateTaskStateStatus(stopCmd);
		}
	}
}
//+----------------------------------------------------------------------------
//
// method :  ASTaskManager::moveAxisPosition(double relativeMovement, std::string axisId)
// 
// description : 
//
//-----------------------------------------------------------------------------
void ASTaskManager::moveAxisPosition(double relativeMovement, std::string axisId){
	StateStatus movement;
	movement.state = Tango::MOVING;
	movement.status = "Movement on "+ axisId +" running";
	
	updateTaskStateStatus(movement);
	
	if (relativeMovement != 0){
		double currentAxisPosition = 0;
		double newPosition = 0;
		if(axisId == X_AXIS_ID){
			if (m_simulatedMode)
				m_simulatedXTranslation = relativeMovement;
			else { // Act normal
				// check new position - Setting lower/upper limit if out of bounds 
				try{
					currentAxisPosition = m_xActuatorPlugin->getAxisCurrentPosition();
					newPosition = currentAxisPosition + relativeMovement;
				}catch(...){
					movement.state = Tango::FAULT;
					movement.status = "Couldn't getAxisCurrentPosition while axis " + axisId + " positionning..";
					updateTaskStateStatus(movement);
					return;
				}
				setAxisPosition(newPosition, X_AXIS_ID);
			}
		}
		else if (axisId == Y_AXIS_ID){	
			if (m_simulatedMode)
				m_simulatedYTranslation = relativeMovement;
			else { // Act normal
				// check new position - Setting lower or upper limit if out of bounds 
				try{
					currentAxisPosition = m_yActuatorPlugin->getAxisCurrentPosition();
					newPosition = currentAxisPosition + relativeMovement;
				}catch(...){
					movement.state = Tango::FAULT;
					movement.status = "Couldn't getAxisCurrentPosition while axis " + axisId + " positionning..";
					updateTaskStateStatus(movement);
					return;
				}
				setAxisPosition(newPosition, Y_AXIS_ID);
			}
		}
	}
}
//+----------------------------------------------------------------------------
//
// method :  ASTaskManager::moveAxisPosition(double relativeMovement, std::string axisId)
// 
// description : 
//
//-----------------------------------------------------------------------------
void ASTaskManager::setAxisPosition(double position, std::string axisId){
	StateStatus movement;
	movement.state = Tango::MOVING;
	movement.status = "Movement on "+ axisId +" running";

	updateTaskStateStatus(movement);

	if(axisId == X_AXIS_ID){
			try{
				if (position >= m_xAxisMaxPosition){
					m_axesPositionningState.xLimitReached = true;
					m_axesPositionningState.xLimitStatus = "Upper limit has been reached on X axis !";
					m_xActuatorPlugin->setAxisPosition(m_xAxisMaxPosition);
				}else if (position <= m_xAxisMinPosition){
					m_axesPositionningState.xLimitReached = true;
					m_axesPositionningState.xLimitStatus = "Lower limit has been reached on X axis !";
					m_xActuatorPlugin->setAxisPosition(m_xAxisMinPosition);
				}else if ((position < m_xAxisMaxPosition) && (position > m_xAxisMinPosition)){
					m_axesPositionningState.xLimitReached = false;
					m_xActuatorPlugin->setAxisPosition(position);
				}
			}catch(...){
				movement.state = Tango::FAULT;
				movement.status = "Couldn't move axis " + axisId;
				updateTaskStateStatus(movement);
			}
	}
	else if (axisId == Y_AXIS_ID){	
			try{
				if (position >= m_yAxisMaxPosition){
					m_axesPositionningState.yLimitReached = true;
					m_axesPositionningState.yLimitStatus = "Upper limit has been reached on Y axis !";
					m_yActuatorPlugin->setAxisPosition(m_yAxisMaxPosition);
				}else if (position <= m_yAxisMinPosition){
					m_axesPositionningState.yLimitReached = true;
					m_axesPositionningState.yLimitStatus = "Lower limit has been reached on Y axis !";
					m_yActuatorPlugin->setAxisPosition(m_yAxisMinPosition);
				}else if((position < m_yAxisMaxPosition) && (position > m_yAxisMinPosition)){
					m_yActuatorPlugin->setAxisPosition(position);
					m_axesPositionningState.yLimitReached = false;
				}
			}catch(...){
				movement.state = Tango::FAULT;
				movement.status = "Couldn't move axis " + axisId;
				updateTaskStateStatus(movement);
			}
	}
	
}
//+----------------------------------------------------------------------------
//
// method :  ASTaskManager::refreshAxes()
// 
// description : 
//
//-----------------------------------------------------------------------------
bool ASTaskManager::refreshAxes(){
	yat::MutexLock gardDataMutex(m_dataMutex);
	//Getting position will refresh state
	if ((m_xActuatorPlugin != NULL) && (m_yActuatorPlugin != NULL)){
		AS::ActuatorInterface::ActuatorInterface::AxisStateStatus xAxisStatus;
		try{
			m_managerDataPacket.xAxisPosition = m_xActuatorPlugin->getAxisCurrentPosition();
			xAxisStatus = m_xActuatorPlugin->getAxisStateStatus();
		}catch(...){
			m_managerDataPacket.taskStateStatus.state = Tango::FAULT;
			m_managerDataPacket.taskStateStatus.status = "At least one axis plugin is not correctly initialized! Try to Init...";
			return false;
		}
		//Check if x axis is  out of bounds
		if ((m_managerDataPacket.xAxisPosition > m_xAxisMaxPosition) || (m_managerDataPacket.xAxisPosition < m_xAxisMinPosition))  
			m_axesPositionningState.xAxisPositionOutOfBounds = true;
		else 
			m_axesPositionningState.xAxisPositionOutOfBounds = false;

		AS::ActuatorInterface::ActuatorInterface::AxisStateStatus yAxisStatus;
		try{
			m_managerDataPacket.yAxisPosition = m_yActuatorPlugin->getAxisCurrentPosition();
			yAxisStatus = m_yActuatorPlugin->getAxisStateStatus();
		}catch(...){
			m_managerDataPacket.taskStateStatus.state = Tango::FAULT;
			m_managerDataPacket.taskStateStatus.status = "At least one axis plugin is not correctly initialized! Try to Init...";
			return false;
		}
		//Check if y axis is out of bounds
		if ((m_managerDataPacket.yAxisPosition > m_yAxisMaxPosition) || (m_managerDataPacket.yAxisPosition < m_yAxisMinPosition))  
			m_axesPositionningState.yAxisPositionOutOfBounds = true;
		else 
			m_axesPositionningState.yAxisPositionOutOfBounds = false;

		//Update axes state and status
		m_managerDataPacket.xStateStatus.state = xAxisStatus.axisState;
		m_managerDataPacket.xStateStatus.status = xAxisStatus.axisStatus;
		m_managerDataPacket.yStateStatus.state = yAxisStatus.axisState;
		m_managerDataPacket.yStateStatus.status = yAxisStatus.axisStatus;

		//Only check connexions status -
		if(!xAxisStatus.isConnexionOperational){
			m_managerDataPacket.taskStateStatus.state = Tango::FAULT;
			m_managerDataPacket.taskStateStatus.status = "Connexion with X Axis not operational! Check X Axis device then try to Init";
			m_managerDataPacket.xStateStatus.state = Tango::FAULT;
			m_managerDataPacket.xStateStatus.status = "";
			m_managerDataPacket.yStateStatus.state = Tango::FAULT;
			m_managerDataPacket.yStateStatus.status = "";
			return false;
		}else if(!yAxisStatus.isConnexionOperational){
			m_managerDataPacket.taskStateStatus.state = Tango::FAULT;
			m_managerDataPacket.taskStateStatus.status = "Connexion with Y Axis not operational! Check Y Axis device then try to Init";
			m_managerDataPacket.xStateStatus.state = Tango::FAULT;
			m_managerDataPacket.xStateStatus.status = "";
			m_managerDataPacket.yStateStatus.state = Tango::FAULT;
			m_managerDataPacket.yStateStatus.status = "";
			return false;
		}
		return true;
	}else{
		m_managerDataPacket.taskStateStatus.state = Tango::FAULT;
		m_managerDataPacket.taskStateStatus.status = "At least one axis plugin is not correctly initialized! Try to Init...";
	}
}
//+----------------------------------------------------------------------------
//
// method :  ASTaskManager::refreshTaskState())
// 
// description : 
//
//-----------------------------------------------------------------------------
void ASTaskManager::refreshTaskState(){

	yat::MutexLock gardDataMutex(m_dataMutex);
	if((m_managerDataPacket.xStateStatus.state == Tango::DISABLE) || (m_managerDataPacket.yStateStatus.state == Tango::DISABLE)){
		m_managerDataPacket.taskStateStatus.state = Tango::STANDBY;
		m_managerDataPacket.taskStateStatus.status = "At least one axis is DISABLE, system is not ready to operate !";
	}
	if((m_managerDataPacket.xStateStatus.state == Tango::OFF) || (m_managerDataPacket.yStateStatus.state == Tango::OFF)){
		m_managerDataPacket.taskStateStatus.state = Tango::STANDBY;
		m_managerDataPacket.taskStateStatus.status = "At least one axis is OFF, system is not ready to operate !";
	}
	if((m_managerDataPacket.xStateStatus.state == Tango::INIT) || (m_managerDataPacket.yStateStatus.state == Tango::INIT)){
		m_managerDataPacket.taskStateStatus.state = Tango::STANDBY;
		m_managerDataPacket.taskStateStatus.status = "At least one axis is not initialize yet, system is not ready to operate !";
	}
	//Recently added -> To test !
	if((m_managerDataPacket.xStateStatus.state == Tango::ALARM) || (m_managerDataPacket.yStateStatus.state == Tango::ALARM)){
		m_managerDataPacket.taskStateStatus.state = Tango::STANDBY;
		m_managerDataPacket.taskStateStatus.status = "At least one axis is in ALARM state, can't operate !";
	}
	if((m_managerDataPacket.xStateStatus.state == Tango::MOVING) || (m_managerDataPacket.yStateStatus.state == Tango::MOVING)){
		m_managerDataPacket.taskStateStatus.state = Tango::MOVING;
		m_managerDataPacket.taskStateStatus.status = "At least one axis is moving";
	}
	if((m_managerDataPacket.xStateStatus.state == Tango::FAULT) || (m_managerDataPacket.yStateStatus.state == Tango::FAULT)){
		m_managerDataPacket.taskStateStatus.state = Tango::STANDBY;
		m_managerDataPacket.taskStateStatus.status = "At least one axis is FAULT, system is not ready to operate !";
	}
	if((m_managerDataPacket.xStateStatus.state == Tango::ON) && (m_managerDataPacket.yStateStatus.state == Tango::ON)){
		if(m_isXAxisCalibrated && m_isYAxisCalibrated){
			m_managerDataPacket.taskStateStatus.state = Tango::ON;
			if(!m_simulatedMode)
				m_managerDataPacket.taskStateStatus.status = "Actuator System ready";
			else { //  Simulated mode : Status with current oreder informations
				yat::MutexLock gardDataMutex(m_dataMutex);
				ostringstream convertStepsX; 
				convertStepsX<<m_simulatedXTranslation;
				std::string stepsNbStrX = convertStepsX.str();
				ostringstream convertStepsY; 
				convertStepsY<<m_simulatedYTranslation;
				std::string stepsNbStrY = convertStepsY.str();
				m_managerDataPacket.taskStateStatus.status = "[SIMULATED MODE ACTIVE]\
								 				\nLast request received on ActuatorSystem :\
								   				\n-> X axis should move with " + stepsNbStrX + " setps\
								   				\n-> Y axis should move with " + stepsNbStrY + " setps ";
			} 

		}else{
			m_managerDataPacket.taskStateStatus.state = Tango::STANDBY;
			m_managerDataPacket.taskStateStatus.status = "Make sure that both X and Y axes are calibrated !";
		}
	}
	if((m_axesPositionningState.xAxisPositionOutOfBounds || m_axesPositionningState.yAxisPositionOutOfBounds)){
		
		m_managerDataPacket.taskStateStatus.state = Tango::ALARM;
		m_managerDataPacket.taskStateStatus.status = "Actuator system can't operate with at least one axis out of bounds !";

		if (m_axesPositionningState.xAxisPositionOutOfBounds){
			ostringstream maxPosXStream; 
			maxPosXStream<<m_xAxisMaxPosition;
			std::string maxPosXStr = maxPosXStream.str();
			ostringstream minPosXStream; 
			minPosXStream<<m_xAxisMinPosition;
			std::string minPosXStr = minPosXStream.str();
			m_managerDataPacket.taskStateStatus.status = m_managerDataPacket.taskStateStatus.status + 
															"\nX Axis is out of bounds : [" + minPosXStr + " - " + maxPosXStr + "]";
		}
		if (m_axesPositionningState.yAxisPositionOutOfBounds){
			ostringstream maxPosYStream; 
			maxPosYStream<<m_yAxisMaxPosition;
			std::string maxPosYStr = maxPosYStream.str();
			ostringstream minPosYStream; 
			minPosYStream<<m_yAxisMinPosition;
			std::string minPosYStr = minPosYStream.str();
			m_managerDataPacket.taskStateStatus.status = m_managerDataPacket.taskStateStatus.status + 
															"\nY Axis is out of bounds : [" + minPosYStr + " - " + maxPosYStr + "]";
		}
	}

	if(m_axesPositionningState.xLimitReached)
		m_managerDataPacket.taskStateStatus.status = m_managerDataPacket.taskStateStatus.status + "\n" +
														m_axesPositionningState.xLimitStatus;
	
	if (m_axesPositionningState.yLimitReached)
		m_managerDataPacket.taskStateStatus.status = m_managerDataPacket.taskStateStatus.status + "\n" +
														m_axesPositionningState.yLimitStatus;

}
//+----------------------------------------------------------------------------
//
// method :  ASTaskManager::startPlugins()
// 
// description : 	To start actuator plugin.
//
//-----------------------------------------------------------------------------
bool ASTaskManager::startPlugins(){
	StateStatus stateStatus;
	try{
		m_xActuatorPlugin->init(m_hostDev, m_xAxisDeviceAdress);
		m_yActuatorPlugin->init(m_hostDev, m_yAxisDeviceAdress);
		return true;
	}catch(Tango::DevFailed &df){
		stateStatus.state = Tango::FAULT;
		stateStatus.status = "Couldn't start at least one plugin : caught DevFailed exception..";
		updateTaskStateStatus(stateStatus);
		return false;
	}catch(...){
		stateStatus.state = Tango::FAULT;
		stateStatus.status = "Couldn't start at least one plugin : caught (...) exception..";
		updateTaskStateStatus(stateStatus);
		return false;
	}
}
//+----------------------------------------------------------------------------
//
// method :  ASTaskManager::actuatorPluginsInitialisation(std::string axisPluginType, 
//									std::string pluginPath, std::string axisId)
// 
// description : 	To initialize actuator plugin.
//
//-----------------------------------------------------------------------------
bool ASTaskManager::actuatorPluginInitialisation(std::string axisPluginType, std::string pluginPath, std::string axisId){
	StateStatus stateStatus;

	// STEP 1 : Load actuator plugin
	try{
		// use the plugin manager
		std::pair<yat::IPlugInInfo*, yat::IPlugInFactory*> plugin;
		plugin = ASPluginManager::instance().load(pluginPath);
		m_pluginInfo = plugin.first;
		m_pluginFactory = plugin.second;
	}
	catch (Tango::DevFailed& df){
		stateStatus.state = Tango::FAULT;
		stateStatus.status = "Error on loading Power Supply Control plugin!";
		updateTaskStateStatus(stateStatus);
		return false;
	}
	catch (...){
		stateStatus.state = Tango::FAULT;
		stateStatus.status = "Plugin load failed!";
		updateTaskStateStatus(stateStatus);
		return false;
	}

	// STEP 2 : Interface name check
	if (pluginPath !=  ""){
		if ( m_pluginInfo->get_plugin_id() == axisPluginType ){
			string id = m_pluginInfo->get_plugin_id();
			//Plugin factory instanciation:
			yat::IPlugInObject* obj;
			m_pluginFactory->create(obj);

			if(axisId == X_AXIS_ID)
		    	m_xActuatorPlugin = reinterpret_cast<AS::ActuatorInterface::ActuatorInterface*>(obj);
		    else if(axisId == Y_AXIS_ID)
		    	m_yActuatorPlugin = reinterpret_cast<AS::ActuatorInterface::ActuatorInterface*>(obj);
		}
		else{	
			stateStatus.state = Tango::FAULT;
			stateStatus.status = "Error on loading plugin: bad actuator type name : " + axisPluginType + "\nOn axis " + axisId;
			updateTaskStateStatus(stateStatus);
			return false;
		}
	}
	else{	
		stateStatus.state = Tango::FAULT;
		stateStatus.status = "Error on loading plugin: pluginPath for axis " + axisId + "is empty !";
		updateTaskStateStatus(stateStatus);
		return false;
	}

	// STEP 3 : Init plugin 
	try{
		// Use the plugin helper
		yat4tango::DevicePlugInHelper myPluginHelper(m_hostDev, _dynAttrManager);

		if(axisId == X_AXIS_ID)
	    	myPluginHelper.register_plugin(m_xActuatorPlugin);
	    else if(axisId == Y_AXIS_ID)
	    	myPluginHelper.register_plugin(m_yActuatorPlugin);
	}
	catch (Tango::DevFailed & df){
		stateStatus.state = Tango::FAULT;
		stateStatus.status = "Failed to register Power Supply Control plugin for " + axisId;
		updateTaskStateStatus(stateStatus);
		return false;
	}
	catch (...){
		stateStatus.state = Tango::FAULT;
		stateStatus.status = "Failed to register Power Supply Control plugin for " + axisId;
		updateTaskStateStatus(stateStatus);
		return false;
	}

	//Everything went well then return true
	return true;
}
//+----------------------------------------------------------------------------
//
// method :  ASTaskManager::updateTaskStateStatus(StateStatus stateStatus)
// 
// description :  Serialize the state/status update
//
//-----------------------------------------------------------------------------
void ASTaskManager::updateTaskStateStatus(StateStatus stateStatus){
	yat::MutexLock gardDataMutex(m_dataMutex);
	m_managerDataPacket.taskStateStatus = stateStatus;
}
//+----------------------------------------------------------------------------
//
// method :  ASTaskManager::getTaskCurrentState()
// 
// description :  
//
//-----------------------------------------------------------------------------
ASTaskManager::StateStatus ASTaskManager::getTaskCurrentState(){
	return m_managerDataPacket.taskStateStatus;
}
} // Namespace ASTaskManager




