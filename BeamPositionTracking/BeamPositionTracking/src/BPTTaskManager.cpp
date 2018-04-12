// ============================================================================
//
// = CONTEXT
//    TANGO Project -
//
// = FILENAME
//    BPTTaskManager.cpp
//
// = AUTHOR
//    F. Thiam
//
// ============================================================================
// ============================================================================
// DEPENDENCIES
// ============================================================================
#include "BPTTaskManager.h"

static const unsigned short kPERIODIC_TMOUT_MS = 1000;


namespace BPTTaskManager{
// ============================================================================
// SOME USER DEFINED MESSAGES
// ============================================================================
#define kALIGNMENT_PROCESS  		 (yat::FIRST_USER_MSG + 1001)
#define kCALIBRATION 				 (yat::FIRST_USER_MSG + 1002)
#define kUPDATE_TRACKING_TARGET		 (yat::FIRST_USER_MSG + 1003)
#define kUPDATE_WARNING_ZONE		 (yat::FIRST_USER_MSG + 1004)
#define kTRACKING_MODE_OFF			 (yat::FIRST_USER_MSG + 1005)
#define kTRACKING_MODE_ON			 (yat::FIRST_USER_MSG + 1006)
#define kUPDATE_X_REG_THRESHOLD_ZONE (yat::FIRST_USER_MSG + 1007)
#define kUPDATE_Y_REG_THRESHOLD_ZONE (yat::FIRST_USER_MSG + 1008)

// - ActuatorSystem DeviceClass
std::string ACTUATOR_SYSTEM_DEVICE_CLASS_STR = "ActuatorSystem";	
std::string X_AXIS_ID = "xAxis";
std::string Y_AXIS_ID = "yAxis";
// - ActuatorSystem Attributes identification		
std::string ATTR_STATE_STR = "State";
std::string ATTR_X_POSITION_STR = "xPosition";
std::string ATTR_Y_POSITION_STR = "yPosition";
std::string ATTR_X_STATE_STR = "xState";
std::string ATTR_Y_STATE_STR = "yState";
std::string ATTR_IS_X_AXIS_LINEAR_STR = "isXLinear";
std::string ATTR_IS_Y_AXIS_LINEAR_STR = "isYLinear"; 
std::string ATTR_IS_X_AXIS_CALIBRATED_STR = "isXCalibrated";
std::string ATTR_IS_Y_AXIS_CALIBRATED_STR = "isYCalibrated";
std::string ATTR_X_AXIS_LINEAR_RATIO_STR = "xLinearRatio";
std::string ATTR_Y_AXIS_LINEAR_RATIO_STR = "yLinearRatio";
std::string ATTR_IS_SYSTEM_READY_STR = "isSystemReady";
// - ActuatorSystem Commands identification		
std::string STOP_ALL_AXES_CMD_STR = "StopAxes";
std::string CMD_MOVE_X_AXIS_RELATIVE_STR = "MoveXAxisRelative";
std::string CMD_MOVE_Y_AXIS_RELATIVE_STR = "MoveYAxisRelative";
std::string CMD_APPLY_RELATIVE_MOVEMENT_ON_AXES_STR = "ApplyRelativeMovementOnAxes";

int MAX_CALIBRATION_TRY_NB = 100;

// ============================================================================
// BPTTaskManager::BPTTaskManager
// ============================================================================
BPTTaskManager::BPTTaskManager (Tango::DeviceImpl * devHost, yat4tango::DynamicAttributeManager* dynAttrManager, 
											std::string pluginType, std::string pluginPath, std::string actuatorSystDeviceAdr,
											PIDCoefficient xPidCoef, PIDCoefficient yPIDCoef,
											double xAxisNbStepToCalibrate, double yAxisNbStepToCalibrate, int nbImgToAlign, int taskPeriod, bool simulatedMode, FixModeDefinition fixMode)
:yat4tango::DeviceTask(devHost),
m_dynAttrManager(dynAttrManager),
m_hostDev(devHost), 
m_pluginType(pluginType), 
m_pluginPath(pluginPath),
m_actuatorSystDeviceAdress(actuatorSystDeviceAdr), 
m_nbImgToAlign(nbImgToAlign),
m_taskPeriod(taskPeriod),
m_simulatedMode(simulatedMode), 
m_fixModeDef(fixMode)
{
    //Initialize member variables
	yat::MutexLock gardDataMutex(_dataMutex);
	yat::MutexLock gardActSystMutex(_actuatorSystMutex);
	yat::MutexLock gardPluginSensorMutex(m_sensorPluginMutex);
	m_sensorDataList.resize((size_t)m_nbImgToAlign + 1);
	m_computedTime = 0;
	m_actuatorSystem = 0; 
	m_pluginInfo = NULL;
	m_pluginFactory = NULL;
	m_sensorPlugin = NULL;
	m_pidXCorrector = NULL;
	m_pidXCorrector = NULL;
	m_isSystemReady = false;
	m_isBeamInZone = false;
	m_isBeamLost = true;
	m_sensorData.xBeamPostionPixels = 0;
	m_sensorData.yBeamPostionPixels = 0;
	m_sensorData.isBeamDetected = false;
	m_sensorData.isBeamInZone = false;
	m_isTrackingModeRunning = false;
	m_calibrationTryCounter = 0;
	m_xAxisData.axisInfo.stepNbToCalibrate = xAxisNbStepToCalibrate;
	m_yAxisData.axisInfo.stepNbToCalibrate = yAxisNbStepToCalibrate;
	m_xAxisData.axisInfo.PIDCoefficients = xPidCoef;
	m_yAxisData.axisInfo.PIDCoefficients = yPIDCoef;
	//init targets and warning zone to fix values
	m_xTargetedCentroid = m_fixModeDef.target.xTargetPoint;
	m_yTargetedCentroid = m_fixModeDef.target.yTargetPoint;
	m_warningZoneDefinition.xCenter = m_fixModeDef.warningZone.xCenter;
	m_warningZoneDefinition.yCenter = m_fixModeDef.warningZone.yCenter;
	m_warningZoneDefinition.radius = m_fixModeDef.warningZone.radius;
	m_isCalibrationRunning = false;
	m_initCompleted = false;
	m_stepAcknowleged = false;

	//Default -> Cold beam ligne values :
	m_xAxisData.axisThreshold = 8;
	m_yAxisData.axisThreshold = 2;
	initPIDCorrectors();
}
// ============================================================================
// BPTTaskManager::~BPTTaskManager
// ============================================================================
BPTTaskManager::~BPTTaskManager ()
{
	enable_periodic_msg(false);
	if(m_actuatorSystem)
		delete m_actuatorSystem;
	if(m_sensorPlugin)
		delete m_sensorPlugin;
	if(m_pidXCorrector)
		delete m_pidXCorrector;
	if(m_pidYCorrector)
		delete m_pidYCorrector;
}
// ============================================================================
// BPTTaskManager::process_message
// ============================================================================
void BPTTaskManager::process_message (yat::Message& _msg)
  throw (Tango::DevFailed)
{
    //DEBUG_STREAM << "BPTTaskManager::process_message::receiving msg " <<_msg.to_string() << std::endl;
	//- handle msg
	switch (_msg.type())
	{
		//- TASK_INIT ----------------------
	    case yat::TASK_INIT:
	    {
	        DEBUG_STREAM << "BPTTaskManager::handle_message::TASK_INIT::thread is starting up" << std::endl;
	        StateStatus stateStatus;
			stateStatus.state = Tango::INIT;
			stateStatus.status = "Initializing BPTTaskManager";
			updateTaskStateStatus(stateStatus);
	        //Initialize periodic task 
	        enable_timeout_msg(false);
	        set_timeout_msg_period (500);
	        set_periodic_msg_period(m_taskPeriod);
	        enable_periodic_msg(false);  
	        startPlugin(); 
        	//Initialize proxy on ActuatorSystem...
        	if(actuatorSystemProxyInitialisation()){ 
	        	//Initialize axes information
	        	if(initializeAxesInformations()){
		        	//Enable periodic
					enable_periodic_msg(true);	
					m_initCompleted = true; 
				}
	        }
	    } 

	    //- TASK_PERIODIC ------------------
	    case yat::TASK_PERIODIC:
	    {
	    	DEBUG_STREAM << "BPTTaskManager::handle_message::TASK_PERIODIC" << std::endl;
	    	if (m_initCompleted)
	    		periodicJob();
	    }
	    break;

	    //- kCALIBRATION ------------------
	    case kCALIBRATION:
	    {
	        DEBUG_STREAM << "BPTTaskManager::handle_message::kCALIBRATION" << std::endl;
	        m_isCalibrationRunning = true;
	        BPT::SensorInterface::SensorInterface::sensorState currentSensorState = getFreshStateFromSensor();
	        if((getActuatorSystemSate() != Tango::FAULT) && currentSensorState.operational){
	        	if(runSystemCalibration())
	        		m_isSystemReady = true;
	        	else 
	        		m_isSystemReady = false;
	    	}
	    	m_isCalibrationRunning = false;
	    }
	    break;
	    //- kUPDATE_TRACKING_TARGET ------------------
	    case kUPDATE_TRACKING_TARGET:
	    {
	        DEBUG_STREAM << "BPTTaskManager::handle_message::kUPDATE_TRACKING_TARGET" << std::endl;

	        Target *target;
	        try{
		        _msg.detach_data(target);      
		        m_xTargetedCentroid = target->xTargetPoint;
				m_yTargetedCentroid = target->yTargetPoint;
	    	}
	    	catch(...){
	    		//NOOP
	    	}
	    }
	    break;
	    //- kTRACKING_MODE_ON ------------------
	    case kTRACKING_MODE_ON:
	    {
	        DEBUG_STREAM << "BPTTaskManager::handle_message::kTRACKING_MODE_ON" << std::endl;
	        //Active tracking mode!
	        m_isTrackingModeRunning = true;
	        m_sensorDataList.clear();
	        //Start timer here
	        m_correctionLoopTime = std::chrono::high_resolution_clock::now(); 
	    }
	    break;

	    //- kTRACKING_MODE_OFF ------------------
	    case kTRACKING_MODE_OFF:
	    {
	        DEBUG_STREAM << "BPTTaskManager::handle_message::kTRACKING_MODE_OFF" << std::endl;
	        //Set tracking mode off!
	        m_isTrackingModeRunning = false;
	        m_sensorDataList.clear();
	    }
	    break;
	    //- kUPDATE_WARNING_ZONE ------------------
	    case kUPDATE_WARNING_ZONE:
	    {
	        DEBUG_STREAM << "BPTTaskManager::handle_message::kUPDATE_WARNING_ZONE" << std::endl;

	        try{
	        	WarningZone *newWarningZone;
		        _msg.detach_data(newWarningZone);      
		        m_warningZoneDefinition = *newWarningZone;
		    }
	    	catch(...){
	    		//NOOP
	    	}
	    }
	    break;
	    //- kUPDATE_X_REG_THRESHOLD_ZONE ------------------
	    case kUPDATE_X_REG_THRESHOLD_ZONE:
	    {
	        DEBUG_STREAM << "BPTTaskManager::handle_message::kUPDATE_X_REG_THRESHOLD_ZONE" << std::endl;

	        try{
	        	ushort *xRegThreshold;
		        _msg.detach_data(xRegThreshold);      
		        m_xAxisData.axisThreshold = *xRegThreshold;
		    }
	    	catch(...){
	    		//NOOP
	    	}
	    }
	    break;
	    //- kUPDATE_Y_REG_THRESHOLD_ZONE ------------------
	    case kUPDATE_Y_REG_THRESHOLD_ZONE:
	    {
	        DEBUG_STREAM << "BPTTaskManager::handle_message::kUPDATE_Y_REG_THRESHOLD_ZONE" << std::endl;

	        try{
	        	ushort *yRegThreshold;
		        _msg.detach_data(yRegThreshold);      
		        m_yAxisData.axisThreshold  = *yRegThreshold;
		    }
	    	catch(...){
	    		//NOOP
	    	}
	    }
	    break;
	    //- TASK_EXIT ----------------------
	    case yat::TASK_EXIT:
	    {
	        DEBUG_STREAM << "BPTTaskManager::handle_message::TASK_EXIT::thread is quitting" << std::endl;
	    }
	    break;

	    //- THREAD_TIMEOUT -------------------
	    case yat::TASK_TIMEOUT:
	    {
	    
	    }
	    break;

	    //- UNHANDLED MSG --------------------
	    default:
		
	      DEBUG_STREAM << "BPTTaskManager::handle_message::unhandled msg type received" << std::endl;
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
void BPTTaskManager::i_acknowlegeStep(){
	m_stepAcknowleged = true;
}

//+----------------------------------------------------------------------------
//
// method :  BPTTaskManager::i_runSystemCalibration()
//
// description : To run system calibration
//
//-----------------------------------------------------------------------------
void BPTTaskManager::i_runSystemCalibration() throw (Tango::DevFailed){
	
	if(m_isBeamInZone){
		post(kCALIBRATION);
	}
	else{
		THROW_DEVFAILED("DEVICE_ERROR",
			            "Calibration not available if beem not in zone",
		              	"BPTTaskManager::runSystemCalibration");
	}
}
//+----------------------------------------------------------------------------
//
// method :  BPTTaskManager::i_turnTrackingModeOn()
//
// description : To turn tracking mode on
//
//-----------------------------------------------------------------------------
void BPTTaskManager::i_turnTrackingModeOn(Target target) throw (Tango::DevFailed){
	
	yat::Message *updateTargetMsg = new yat::Message(kUPDATE_TRACKING_TARGET);
	updateTargetMsg->attach_data(target);

	//In any case, record the new tagerts (synchronously...)
	post(updateTargetMsg);	

	if(m_isBeamInZone){
		if (m_isSystemReady)
			post(kTRACKING_MODE_ON);
		else{
			THROW_DEVFAILED("DEVICE_ERROR",
				            "System must be calibrated to use tracking mode",
			              	"BPTTaskManager::turnTrackingModeOn");
		}	
	}
	else{
		THROW_DEVFAILED("DEVICE_ERROR",
			            "Tracking not available if beem not in zone",
		              	"BPTTaskManager::turnTrackingModeOn");
	}
}
//+----------------------------------------------------------------------------
//
// method :  BPTTaskManager::i_turnTrackingModeOff()
//
// description : To turn tracking mode off
//
//-----------------------------------------------------------------------------
void BPTTaskManager::i_turnTrackingModeOff(){
	post(kTRACKING_MODE_OFF);
}
//+----------------------------------------------------------------------------
//
// method :  BPTTaskManager::i_setTrackingTarget()
//
// description : Only to update target
//
//-----------------------------------------------------------------------------
void BPTTaskManager::i_setTrackingTarget(Target target){
	
	//Just in case, not supposed to be called in fix mode anyway
	if(!m_fixModeDef.isFixModeEnabled){
		yat::Message *updateTargetMsg = new yat::Message(kUPDATE_TRACKING_TARGET);
		updateTargetMsg->attach_data(target);
		post(updateTargetMsg);
	}
}
//+----------------------------------------------------------------------------
//
// method :  BPTTaskManager::i_setWarningZone()
//
// description : Only to set warning zone
//
//-----------------------------------------------------------------------------
void BPTTaskManager::i_setWarningZone(WarningZone newWarningZone){
	
	//Just in case, not supposed to be called in fix mode anyway
	if(!m_fixModeDef.isFixModeEnabled){
		yat::Message *updateWarningMsg = new yat::Message(kUPDATE_WARNING_ZONE);
		updateWarningMsg->attach_data(newWarningZone);
		post(updateWarningMsg);
	}
}

//+----------------------------------------------------------------------------
//
// method :  BPTTaskManager::i_setXAxisRegThreshold()
//
// description : Only to set x axis regulation threshold
//
//-----------------------------------------------------------------------------
void BPTTaskManager::i_setXAxisRegThreshold(ushort newXRegThreshold){
	
	yat::Message *updateXRegThreshold = new yat::Message(kUPDATE_X_REG_THRESHOLD_ZONE);
	updateXRegThreshold->attach_data(newXRegThreshold);

	post(updateXRegThreshold);
}

//+----------------------------------------------------------------------------
//
// method :  BPTTaskManager::i_setYAxisRegThreshold()
//
// description : Only to set x axis regulation threshold
//
//-----------------------------------------------------------------------------
void BPTTaskManager::i_setYAxisRegThreshold(ushort newYRegThreshold){
	
	yat::Message *updateYRegThreshold = new yat::Message(kUPDATE_Y_REG_THRESHOLD_ZONE);
	updateYRegThreshold->attach_data(newYRegThreshold);

	post(updateYRegThreshold);
}

//-----------------------------------------------------------------------------
//
//
//					TASK - PUBLIC GETTERS
//
//
//-----------------------------------------------------------------------------
//+----------------------------------------------------------------------------
//
// method :  BPTTaskManager::i_getManagerDataPacket()
// 
// description : 	Return beam diagnostic and axis data 
//
//-----------------------------------------------------------------------------
ManagerDataPacket BPTTaskManager::i_getManagerDataPacket(){

	BeamDiagnostic beamDiagnostic;
	//Proctect data access
	yat::MutexLock gardDataMutex(_dataMutex);

	beamDiagnostic.xBeamPostionPixels = m_sensorData.xBeamPostionPixels;
	beamDiagnostic.yBeamPostionPixels = m_sensorData.yBeamPostionPixels;
	beamDiagnostic.isBeamDetected = m_sensorData.isBeamDetected;
	beamDiagnostic.isBeamInZone = m_sensorData.isBeamInZone;

	beamDiagnostic.imgHigh = m_sensorData.imgHigh;
	beamDiagnostic.imgWidth = m_sensorData.imgWidth;
	beamDiagnostic.tresholdedImg.clear();
	beamDiagnostic.tresholdedImg = m_sensorData.tresholdedImg;
	
	ManagerDataPacket dataPacketToReturn;
	dataPacketToReturn.beamDiagnostic = beamDiagnostic;
	dataPacketToReturn.xAxisData = m_xAxisData;
	dataPacketToReturn.yAxisData = m_yAxisData;
	dataPacketToReturn.computedTime = m_computedTime * 1000;

	return dataPacketToReturn;
}

//+----------------------------------------------------------------------------
//
// method :  BPTTaskManager::i_getStateStatus()
//
// description : 	Return current task's state 
//
//-----------------------------------------------------------------------------
StateStatus BPTTaskManager::i_getStateStatus(){

	yat::MutexLock gardDataMutex(_dataMutex);
	StateStatus stateStatus = m_managerStateStatus;
	return stateStatus;
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
// method :  BPTTaskManager::waitForUserAcknowlegement()
// 
// description :  Method used to go to next step in a simulated mode
//
//-----------------------------------------------------------------------------
void BPTTaskManager::waitForUserAcknowlegement(){
	while(!m_stepAcknowleged && m_isTrackingModeRunning){
		//do nothing !
		sleep(0.5);
	}
	//then reset acknowlegement
	m_stepAcknowleged = false;	
}
//+----------------------------------------------------------------------------
//
// method :  BPTTaskManager::updateTaskStateStatus(StateStatus stateStatus)
// 
// description :  Serialize the state/status update
//
//-----------------------------------------------------------------------------
void BPTTaskManager::updateTaskStateStatus(StateStatus stateStatus){
	yat::MutexLock gardDataMutex(_dataMutex);
	m_managerStateStatus = stateStatus;
}

void BPTTaskManager::updateComputedTime(double newComputedTime){
	yat::MutexLock gardDataMutex(_dataMutex);
	m_computedTime = newComputedTime;
}
//+----------------------------------------------------------------------------
//
// method :  BPTTaskManager::refreshDataFromSensorPlugin()
// 
// description : 	In order to get fresh data from plugin 
//
//-----------------------------------------------------------------------------
void BPTTaskManager::refreshDataFromSensorPlugin(){
	yat::MutexLock gardDataMutex(_dataMutex);
	StateStatus stateStatus;
	m_sensorData = getFreshDataFromSensor();
	//Beam is detected 
	if (m_sensorData.isBeamDetected){
		m_isBeamLost = false;
		if(checkIfBeamIsInZone()){
			m_isBeamInZone = true;
			if(m_isTrackingModeRunning){
				//store into list of data
				m_sensorDataList.push_back(m_sensorData);
			}
		}else{
			m_isBeamInZone = false;
		}
	}
	else{
		m_isBeamLost = true;
		m_isBeamInZone = false;
	}
}

//+----------------------------------------------------------------------------
//
// method :  BPTTaskManager::checkIfBeamIsInZone()
// 
// description : 	check if beam is in warning zone
//
//-----------------------------------------------------------------------------
bool BPTTaskManager::checkIfBeamIsInZone(){

	double squareDistance = pow((double)m_sensorData.xBeamPostionPixels - m_warningZoneDefinition.xCenter, (double)2);
	squareDistance += pow(((double)m_sensorData.yBeamPostionPixels - m_warningZoneDefinition.yCenter), (double)2);

	// warning zone state
	if(squareDistance > pow((double)m_warningZoneDefinition.radius, 2)){
		m_sensorData.isBeamInZone = false;
	}
	else{
		m_sensorData.isBeamInZone = true;
	}

	return m_sensorData.isBeamInZone;
}
//+----------------------------------------------------------------------------
//
// method :  BPTTaskManager::refreshDataFromActuatorSystem()
// 
// description : 	In order to refresh axes positions and states
//
//-----------------------------------------------------------------------------
Tango::DevState BPTTaskManager::refreshDataFromActuatorSystem(){
	yat::MutexLock gardDataMutex(_dataMutex);

	Tango::DevState actuatorSystemState;
	readAttributeOnActuatorSystem(ATTR_X_STATE_STR)>>m_xAxisData.currentState; 
	readAttributeOnActuatorSystem(ATTR_Y_STATE_STR)>>m_yAxisData.currentState; 
	readAttributeOnActuatorSystem(ATTR_IS_SYSTEM_READY_STR)>>m_isSystemReady;
	//Can't check positions if AS is in fault
	readAttributeOnActuatorSystem(ATTR_STATE_STR)>>actuatorSystemState;
	if(actuatorSystemState != Tango::FAULT){
		readAttributeOnActuatorSystem(ATTR_X_POSITION_STR)>>m_xAxisData.currentPosition; 
		readAttributeOnActuatorSystem(ATTR_Y_POSITION_STR)>>m_yAxisData.currentPosition; 
	}
	return actuatorSystemState; 
}
//+----------------------------------------------------------------------------
//
// method :  BPTTaskManager::runSystemCalibration()
// 
// description : 	To calibrate x/y axes 
//
//-----------------------------------------------------------------------------
bool BPTTaskManager::runSystemCalibration(){

	StateStatus stateStatus;
	if (!m_simulatedMode){
		//For each axis, depending on axis type...
		//For X axis 
		if (m_xAxisData.axisInfo.isAxisLinear /*&& !m_xAxisData.axisInfo.PIDCoefficients.isPIDEnabled*/){
			stateStatus.state = Tango::DISABLE;
			stateStatus.status = "[CALIBRATION]\
								 \nCurrently running linear calibration on X Axis...";
			updateTaskStateStatus(stateStatus);
			//Setting axis state to not calibrated
			Tango::DeviceAttribute* calibratedState;
			calibratedState = new Tango::DeviceAttribute(ATTR_IS_X_AXIS_CALIBRATED_STR, false);
			writeAttributeOnActuatorSystem(*calibratedState); 
			runLinearCalibration(m_xAxisData);
			delete calibratedState;
		}
		else {
			stateStatus.state = Tango::DISABLE;
			stateStatus.status = "[CALIBRATION]\
								 \nCurrently running non linear calibration on X Axis...";
			updateTaskStateStatus(stateStatus);
			runNonLinearCalibration(X_AXIS_ID);
		}
		//For Y axis 
		if (m_yAxisData.axisInfo.isAxisLinear /*&& !m_yAxisData.axisInfo.PIDCoefficients.isPIDEnabled*/){
			stateStatus.state = Tango::DISABLE;
			stateStatus.status = "[CALIBRATION]\
								 \nCurrently running linear calibration on Y Axis...";
			updateTaskStateStatus(stateStatus);
			//Setting axis state to not calibrated
			Tango::DeviceAttribute* calibratedState;
			calibratedState = new Tango::DeviceAttribute(ATTR_IS_Y_AXIS_CALIBRATED_STR, false);
			writeAttributeOnActuatorSystem(*calibratedState); 
			runLinearCalibration(m_yAxisData);
			delete calibratedState;
		}
		else {
			stateStatus.state = Tango::DISABLE;
			stateStatus.status = "[CALIBRATION]\
								 \nCurrently running non linear calibration on Y Axis...";
			updateTaskStateStatus(stateStatus);
			runNonLinearCalibration(Y_AXIS_ID);
		}
	}
	else {
		//not done yet...
		StateStatus stateStatus;
		stateStatus.state = Tango::DISABLE;
		stateStatus.status = "[CALIBRATION]\
							 \nSimulated calibration not available yet...";
		updateTaskStateStatus(stateStatus);	
		sleep(5); // Let user know for 5 sec
	}
	return (m_xAxisData.isAxisCalibrated && m_yAxisData.isAxisCalibrated);
}

//+----------------------------------------------------------------------------
//
// method :  BPTTaskManager::runLinearCalibration()
// 
// description : 	Calibration in case of linear axes
//
//-----------------------------------------------------------------------------
void BPTTaskManager::runLinearCalibration(Axis axis){

	Tango::DeviceData relativeMovment;
	relativeMovment<<axis.axisInfo.stepNbToCalibrate;
	
	// To ensure that motor has been stoped  
	if ((getActuatorSystemSate() == Tango::STANDBY) || (getActuatorSystemSate() == Tango::ON)){
		double oldPosition = 0;
		double newPosition = 0;
		double realDisplacement = 0;
		double oldCentroid = 0;

		if (axis.axisInfo.axisID == X_AXIS_ID){
			oldCentroid = getXCentroid();
			sendCommandToActuatorSystem(CMD_MOVE_X_AXIS_RELATIVE_STR, relativeMovment);
			readAttributeOnActuatorSystem(ATTR_X_POSITION_STR)>>oldPosition;
		}
		else if (axis.axisInfo.axisID == Y_AXIS_ID){
			oldCentroid = getYCentroid();
			sendCommandToActuatorSystem(CMD_MOVE_Y_AXIS_RELATIVE_STR, relativeMovment);
			readAttributeOnActuatorSystem(ATTR_Y_POSITION_STR)>>oldPosition;
		}
		//Wait for axis's end of movement 
		while(getActuatorSystemSate() == Tango::MOVING){
			sleep(1);
		}
		//Check new axis position
		if (axis.axisInfo.axisID == X_AXIS_ID)
			readAttributeOnActuatorSystem(ATTR_X_POSITION_STR)>>newPosition;
		else
			readAttributeOnActuatorSystem(ATTR_Y_POSITION_STR)>>newPosition;
		
		realDisplacement = newPosition - oldPosition;

		if ( realDisplacement == 0){//axis didn't move, to close to its limit (low or hight..)
			StateStatus stateStatus;
			stateStatus.state = Tango::DISABLE;
			stateStatus.status = "[CALIBRATION]\
								 \nSeems to have some trouble with calibration on "+ axis.axisInfo.axisID +"\
								 \nCheck that axes are not to close to their limit on Actuator system...";
			updateTaskStateStatus(stateStatus);	
			sleep(7); // Let user know for 5 sec
			return;
		}

		// Movement is over, retrieve the new centroid position
		double newCentroid = 0;
		if (axis.axisInfo.axisID == X_AXIS_ID)
			newCentroid = getXCentroid();
		else if (axis.axisInfo.axisID == Y_AXIS_ID)
			newCentroid = getYCentroid();
    	//	R A T I O    E S T I M A T I O N  
		double ratio = (double)realDisplacement / (double)(newCentroid - oldCentroid);

		// InvertMovement - Replace motor in init position
		relativeMovment<<-realDisplacement;
		Tango::DeviceAttribute* ratioToWrite = NULL;
		Tango::DeviceAttribute* calibratedState = NULL;
		// Construct corresponding attributes to update infos on Actuator system
		if (axis.axisInfo.axisID == X_AXIS_ID){
			ratioToWrite = new Tango::DeviceAttribute(ATTR_X_AXIS_LINEAR_RATIO_STR, ratio);
			calibratedState = new Tango::DeviceAttribute(ATTR_IS_X_AXIS_CALIBRATED_STR, true);
			m_xAxisData.isAxisCalibrated = true;
			// Move corresponding axis back in place
			sendCommandToActuatorSystem(CMD_MOVE_X_AXIS_RELATIVE_STR, relativeMovment);
		}
		else if (axis.axisInfo.axisID == Y_AXIS_ID){
			ratioToWrite = new Tango::DeviceAttribute(ATTR_Y_AXIS_LINEAR_RATIO_STR, ratio);
			calibratedState = new Tango::DeviceAttribute(ATTR_IS_Y_AXIS_CALIBRATED_STR, true);
			m_yAxisData.isAxisCalibrated = true;
			// Move corresponding axis back in place
			sendCommandToActuatorSystem(CMD_MOVE_Y_AXIS_RELATIVE_STR, relativeMovment);
		}

		//Wait for axis's end of movement 
		while(getActuatorSystemSate() == Tango::MOVING){
			sleep(1);
		}
		// Set ratio on corresponding axis 
		writeAttributeOnActuatorSystem(*ratioToWrite); 
		// Confirm that corresponding axis is calibrated
		writeAttributeOnActuatorSystem(*calibratedState);

		delete ratioToWrite;
		delete calibratedState;
	}else if(getActuatorSystemSate() == Tango::ALARM){
		StateStatus stateStatus;
		stateStatus.state = Tango::DISABLE;
		stateStatus.status = "Currently not possible to run calibration, check Actuator system status";
		updateTaskStateStatus(stateStatus);	
		sleep(2);
	}else {
		sendCommandToActuatorSystem(STOP_ALL_AXES_CMD_STR);
		// Wait a second
		sleep(1);
		// Try again 
		m_calibrationTryCounter++;
		if (m_calibrationTryCounter <= MAX_CALIBRATION_TRY_NB)
			runLinearCalibration(axis);
	}

	m_calibrationTryCounter = 0;	
}
//+----------------------------------------------------------------------------
//
// method :  BPTTaskManager::runNonLinearCalibration()
// 
// description : 	only for non-linear axes
//
//-----------------------------------------------------------------------------
void BPTTaskManager::runNonLinearCalibration(std::string axisID){

	//Is it the place to create the interpollation table?
	//-> or do we consider that this table has to be created already?..
}
//+----------------------------------------------------------------------------
//
// method :  BPTTaskManager::periodicJob()
// 
// description : 	
//
//-----------------------------------------------------------------------------
void BPTTaskManager::periodicJob(){

	StateStatus stateStatus;
	BPT::SensorInterface::SensorInterface::sensorState currentSensorState = getFreshStateFromSensor();

	if(currentSensorState.operational && !m_isCalibrationRunning){
        if(m_initCompleted){	        
	        Tango::DevState actuatorSystemState = refreshDataFromActuatorSystem();
	        if (m_isTrackingModeRunning){
	        	if((actuatorSystemState == Tango::ON)){
		        	refreshDataFromSensorPlugin();
		        	if(m_isBeamInZone){
		        		//If in simulated mode we set status with values of current step - Can't do it here 
		        		if (!m_simulatedMode) {
				        	stateStatus.state = Tango::RUNNING;
							stateStatus.status = "Tracking mode is currently running\
												 \nData acquisition running!";	
							updateTaskStateStatus(stateStatus);	
						}
						//Enough data to go ?
						if(m_sensorDataList.size() >= m_nbImgToAlign){
							runBeamTracking();
							//End of cycle, new loop 
							m_correctionLoopTime = std::chrono::high_resolution_clock::now(); 
						}
					}else{
						m_isTrackingModeRunning = false;
					}
				}else{
					m_isTrackingModeRunning = false;
				}
	        }else {
	        	refreshDataFromSensorPlugin();
				if (m_simulatedMode)
					stateStatus.status = "[SIMULATED MODE]\n";
				// Update task state regarding sensor and acutaor system states..
				if(m_isSystemReady && m_isBeamInZone){
    				stateStatus.state = Tango::ON;
					stateStatus.status = stateStatus.status + "\nBeam founded and currently in warning zone\
										 \nActuator system calibrated \
										 \nReady for tracking mode\n";
				}
				if (!m_isSystemReady){
					stateStatus.state = Tango::STANDBY;
					stateStatus.status = stateStatus.status + "\nActuator system is not ready"
										 "\n>> Make sure to calibrate ActuatorSystem, check axes positions"+
										  "\nTracking mode will not be available until then...\n";
				}
				if (!m_isBeamInZone && !m_isBeamLost){
					stateStatus.state = Tango::STANDBY;
					stateStatus.status = stateStatus.status + "\nBeam has been found but is not currently in warning zone" +
										 "\n>> Make sure to drive beam back in zone " + 
										 "\nTracking mode will not be available until then...\n";
				}
				if (m_isBeamLost){
					stateStatus.state = Tango::STANDBY;
					stateStatus.status = stateStatus.status + "\nBeam lost !" +
										"\n>> Make sure to drive beam back in dectector's area!"+
										  "\nTracking mode will not be available until then...\n";
				}
				if (actuatorSystemState == Tango::FAULT){
					stateStatus.state = Tango::FAULT;
					stateStatus.status = stateStatus.status + "\nCheck Actuator system Device\
										 \nCannot operate if acutator system is in fault!\n";
				}
				updateTaskStateStatus(stateStatus);
	        }
        }else{
        	m_isTrackingModeRunning = false;
        }
    }else{
		stateStatus.state = Tango::FAULT;
		stateStatus.status = "Sensor plugin is currently not operational \n" + currentSensorState.errorStatus;
		updateTaskStateStatus(stateStatus);
	}
}
//+----------------------------------------------------------------------------
//
// method :  BPTTaskManager::runBeamTracking()
// 
// description : 	only for non-linear axes
//					will also deal with the use of PID for each axis
//
//-----------------------------------------------------------------------------
void BPTTaskManager::runBeamTracking(){
	//Initialize data for tracking 
	double xCentroidSum = 0;
	double yCentroidSum = 0;
	double meanXCentroid = 0;
	double meanYCentroid = 0;
	int nbDataAcquired = m_sensorDataList.size();
	//Mean of centroids
 	for (int i = 0; i < nbDataAcquired; i++){
 		xCentroidSum += m_sensorDataList[i].xBeamPostionPixels;
 		yCentroidSum += m_sensorDataList[i].yBeamPostionPixels;
 	}
 	meanXCentroid = xCentroidSum / nbDataAcquired;
 	meanYCentroid = yCentroidSum / nbDataAcquired;
 	//Loop over - Duration?
 	std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now(); 
 	std::chrono::duration<double> loopDuration = std::chrono::duration_cast<std::chrono::duration<double>>(now - m_correctionLoopTime);
 	//Estimate correction using PID corrector
 	double errorXInput = m_xTargetedCentroid - meanXCentroid;
 	double errorYInput = m_yTargetedCentroid - meanYCentroid;

	if (std::abs(errorXInput) <= m_xAxisData.axisThreshold ){
		errorXInput = 0;
	}
	if (std::abs(errorYInput) <= m_yAxisData.axisThreshold ){
		errorYInput = 0;
	}

 	//If PID is enabled, we use the corresponding corrector to estimate error
 	if (m_xAxisData.axisInfo.PIDCoefficients.isPIDEnabled && (errorXInput != 0) ){
 		errorXInput = m_pidXCorrector->getStepValue(errorXInput, (double)loopDuration.count());
 	}
 	if (m_yAxisData.axisInfo.PIDCoefficients.isPIDEnabled && (errorYInput != 0)){
 		errorYInput = m_pidYCorrector->getStepValue(errorYInput, (double)loopDuration.count());
 	}

 	updateComputedTime((double)loopDuration.count());
 	// Let Actuator system deal with convertion ratio if there is one
 	// Will do transformation on axes (linear or not...)
 	Tango::DeviceData pixelsDeltasToSend;
 	std::vector<double> values;

 	//Round pixel error!
 	errorXInput = round(errorXInput);
 	errorYInput = round(errorYInput);
 	//If input error is below threshold, erase it...
	
 	//Don't need to call AS if error on X and Y are null
 	if((errorXInput != 0) || (errorYInput != 0)){
	 	values.push_back(errorXInput);
	 	values.push_back(errorYInput);
	 	pixelsDeltasToSend<<values;
		//If simulated mod is active - Send values to status instead of actuator stystem
		if(m_simulatedMode){
			ostringstream convertX; 
			convertX<<errorXInput;
			std::string errorXInputStr = convertX.str();
			ostringstream convertY; 
			convertY<<errorYInput;
			std::string errorYInputStr = convertY.str();
			StateStatus stateStatus;
			stateStatus.state = Tango::RUNNING;
			stateStatus.status = "[SIMULATED MODE ACTIVE]     \
								 \nBPT should now move axes this way : 	  \
								   \n-> X axis should move with " + errorXInputStr + " setps \
								   \n-> Y axis should move with " + errorYInputStr + " setps \
								 \nWaiting for user acknowlegement ...";
			updateTaskStateStatus(stateStatus);
			waitForUserAcknowlegement();
		}
		// Normal mode, send values on actuator then wait for it's movement to be over		
		else{
	 		sendCommandToActuatorSystem(CMD_APPLY_RELATIVE_MOVEMENT_ON_AXES_STR, pixelsDeltasToSend);
	 		//Wait for axis's end of movement 
			while(getActuatorSystemSate() == Tango::MOVING){
				sleep(1);
			}
		}	
	}
	else if (m_simulatedMode){
		StateStatus stateStatus;
		stateStatus.state = Tango::RUNNING;
		stateStatus.status = "New step in simulated mode \n-> nothing to move in this step!\nPlease check Axes Regulation Thresholds values \nWaiting for acknowlegement ...";
		updateTaskStateStatus(stateStatus);
		waitForUserAcknowlegement();
	}
 	//flush data
 	m_sensorDataList.clear();
}

//+----------------------------------------------------------------------------
//
// method :  BPTTaskManager::initializeAxesInformations()
// 
// description : 	To initialize axes informations
//
//-----------------------------------------------------------------------------
bool BPTTaskManager::initializeAxesInformations(){
	try {
		//Initialize X Axis informations------------------------------------------------------	
		m_xAxisData.axisInfo.axisID = X_AXIS_ID;
		readAttributeOnActuatorSystem(ATTR_IS_X_AXIS_LINEAR_STR) >>  m_xAxisData.axisInfo.isAxisLinear;

		//Initialize Y Axis informations------------------------------------------------------
		m_yAxisData.axisInfo.axisID = Y_AXIS_ID;
		readAttributeOnActuatorSystem(ATTR_IS_Y_AXIS_LINEAR_STR)  >>  m_yAxisData.axisInfo.isAxisLinear;

		return true;
	}catch(...){
		//Noop in some cases, attribute is not readable ...
		return false;
	}
} 
//+----------------------------------------------------------------------------
//
// method :  BPTTaskManager::actuatorSystemProxyInitialisation()
// 
// description : 	To initialize proxy on actuator system
//
//-----------------------------------------------------------------------------
bool BPTTaskManager::actuatorSystemProxyInitialisation(){

	try{
		m_actuatorSystem = new Tango::DeviceProxy (m_actuatorSystDeviceAdress);
		Tango::DeviceData test = m_actuatorSystem->command_inout("Status");
		//Check device Class to ensure proxy is ok
		Tango::DeviceInfo dev_info = m_actuatorSystem->info() ;
		if(dev_info.dev_class == ACTUATOR_SYSTEM_DEVICE_CLASS_STR)
			return true;
	}catch(Tango::DevFailed &df){
		m_actuatorSystem = 0; 
		StateStatus stateStatus;
		stateStatus.state = Tango::FAULT;
		stateStatus.status = "Couldn't initialize proxy on actuator system : caught DevFailed exception..";
		updateTaskStateStatus(stateStatus);
		return false;
	}catch(...){
		m_actuatorSystem = 0; 
		StateStatus stateStatus;
		stateStatus.state = Tango::FAULT;
		stateStatus.status = "Couldn'tinitialize proxy on actuator system : caught (...) exception..";
		updateTaskStateStatus(stateStatus);
		return false;
	}
	return false;
}
//+----------------------------------------------------------------------------
//
// method :  BPTTaskManager::initPIDCorrectors()
// 
// description : 	To initialize both PID correctors if needed
//
//-----------------------------------------------------------------------------
void BPTTaskManager::initPIDCorrectors(){

	if(m_xAxisData.axisInfo.PIDCoefficients.isPIDEnabled)
		m_pidXCorrector = new PIDCorrector::PIDCorrector(m_xAxisData.axisInfo.PIDCoefficients.P, m_xAxisData.axisInfo.PIDCoefficients.I ,m_xAxisData.axisInfo.PIDCoefficients.D);
	if(m_yAxisData.axisInfo.PIDCoefficients.isPIDEnabled)
		m_pidYCorrector = new PIDCorrector::PIDCorrector(m_yAxisData.axisInfo.PIDCoefficients.P, m_yAxisData.axisInfo.PIDCoefficients.I ,m_yAxisData.axisInfo.PIDCoefficients.D);
}
//+----------------------------------------------------------------------------
//
// method :  BPTTaskManager::startPlugin()
// 
// description : 	To start sensor plugin.
//
//-----------------------------------------------------------------------------
void BPTTaskManager::startPlugin(){
	StateStatus stateStatus;
	try{
		if(m_sensorPlugin){
			m_sensorPlugin->start(m_hostDev, m_fixModeDef.isFixModeEnabled);
			stateStatus.state = Tango::STANDBY;
			stateStatus.status = "Initialisation done, ready to start ...";
		}else{
			stateStatus.state = Tango::FAULT;
			stateStatus.status = "Couldn't start plugin : plugin not correctly initialized";
		}
	}catch(Tango::DevFailed &df){
		stateStatus.state = Tango::FAULT;
		stateStatus.status = "Couldn't start plugin : caught DevFailed exception..";
	}catch(...){
		stateStatus.state = Tango::FAULT;
		stateStatus.status = "Couldn't start plugin : caught (...) exception..";
	}
	updateTaskStateStatus(stateStatus);
}
//+----------------------------------------------------------------------------
//
// method :  BPTTaskManager::sensorPluginInitialisation()
// 
// description : 	To initialize sensor plugin.
//
//-----------------------------------------------------------------------------
bool BPTTaskManager::sensorPluginInitialisation(){
	StateStatus stateStatus;
	
	// STEP 1 : Load Sensor plugin
	try{
		// use the plugin manager
		std::pair<yat::IPlugInInfo*, yat::IPlugInFactory*> plugin;
		plugin = BPTPluginManager::instance().load(m_pluginPath);
		m_pluginInfo = plugin.first;
		m_pluginFactory = plugin.second;
	}
	catch (Tango::DevFailed& df){

		std::cout<<"DevFailed catch try to load plugin..."<<std::endl;
		m_pluginInfo = NULL;
		m_pluginFactory = NULL;		
		stateStatus.state = Tango::FAULT;
		stateStatus.status = "Error on loading Power Supply Control plugin!";
		updateTaskStateStatus(stateStatus);
		return false;
	}
	catch (...){
		std::cout<<"... catch try to load plugin..."<<std::endl;
		m_pluginInfo = NULL;
		m_pluginFactory = NULL;		
		stateStatus.state = Tango::FAULT;
		stateStatus.status = "Plugin load failed!";
		updateTaskStateStatus(stateStatus);
		return false;
	}

	// STEP 2 : Interface name check
	if (m_pluginInfo != NULL){
		if ( m_pluginInfo->get_plugin_id() == m_pluginType ){
			string id = m_pluginInfo->get_plugin_id();
			//Plugin factory instanciation:
			yat::IPlugInObject* obj;
			m_pluginFactory->create(obj);
		    m_sensorPlugin = reinterpret_cast<BPT::SensorInterface::SensorInterface*>(obj);
		}
		else{	
			stateStatus.state = Tango::FAULT;
			stateStatus.status = "Error on loading plugin: bad sensor type name : " + m_pluginType;
			updateTaskStateStatus(stateStatus);
			return false;
		}
	}
	else 
		return false;

	// STEP 3 : Init plugin 
	try{
		// Use the plugin helper
		yat4tango::DevicePlugInHelper myPluginHelper(m_hostDev, m_dynAttrManager);
		myPluginHelper.register_plugin(m_sensorPlugin);
	}
	catch (Tango::DevFailed & df){
		std::cout<<"DevFailed while trying to register the plugin"<<std::endl;
		stateStatus.state = Tango::FAULT;
		stateStatus.status = "Failed to register Power Supply Control plugin";
		updateTaskStateStatus(stateStatus);
		return false;
	}
	catch (...){
		std::cout<<"... catched while trying to register the plugin"<<std::endl;
		stateStatus.state = Tango::FAULT;
		stateStatus.status = "Failed to register Power Supply Control plugin";
		updateTaskStateStatus(stateStatus);
		return false;
	}

	//Everything went well then return true
	return true;
}


//+----------------------------------------------------------------------------
//
//						Actuator System Access
//
//-----------------------------------------------------------------------------
Tango::DeviceData BPTTaskManager::sendCommandToActuatorSystem(std::string cmdName){
	yat::MutexLock gardActuatorMutex(_actuatorSystMutex);
	StateStatus stateStatus;
	Tango::DeviceData tmpDataToReturn;
	try{
		if (m_actuatorSystem && !m_simulatedMode){
			tmpDataToReturn = m_actuatorSystem->command_inout(cmdName);
		}
		else{
			stateStatus.state = Tango::FAULT;
			stateStatus.status = "Something went wrong with ActuatorSystem's Proxy initialisation... Try to init";
			updateTaskStateStatus(stateStatus);
		}
	}catch(Tango::ConnectionFailed &conf){
		stateStatus.state = Tango::FAULT;
		stateStatus.status = "Connection failed catched : Check ActuatorSystem device..";
		updateTaskStateStatus(stateStatus);
	}catch(Tango::CommunicationFailed &comf){
		stateStatus.state = Tango::FAULT;
		stateStatus.status = "Communication failed catched : Check ActuatorSystem device..";
		updateTaskStateStatus(stateStatus);
	}catch(Tango::DeviceUnlocked &du){
		stateStatus.state = Tango::FAULT;
		stateStatus.status = "Device unlocked catched : Check ActuatorSystem device..";
		updateTaskStateStatus(stateStatus);
	}catch(Tango::DevFailed &df){
		stateStatus.state = Tango::FAULT;
		stateStatus.status = "Connection failed catched : Check ActuatorSystem device..";
		updateTaskStateStatus(stateStatus);
	}
	return tmpDataToReturn;
}

Tango::DeviceData BPTTaskManager::sendCommandToActuatorSystem(std::string cmdName, Tango::DeviceData args){
	yat::MutexLock gardActuatorMutex(_actuatorSystMutex);
	Tango::DeviceData tmpDataToReturn;
	StateStatus stateStatus;
	try{
		if (m_actuatorSystem && !m_simulatedMode)
			tmpDataToReturn = m_actuatorSystem->command_inout(cmdName, args);
		else{
			stateStatus.state = Tango::FAULT;
			stateStatus.status = "Something went wrong with ActuatorSystem's Proxy initialisation... Try to init";
			updateTaskStateStatus(stateStatus);
		}
	}catch(Tango::ConnectionFailed &conf){
		stateStatus.state = Tango::FAULT;
		stateStatus.status = "Connection failed catched : Check ActuatorSystem device..";
		updateTaskStateStatus(stateStatus);
	}catch(Tango::CommunicationFailed &comf){
		stateStatus.state = Tango::FAULT;
		stateStatus.status = "Communication failed catched : Check ActuatorSystem device..";
		updateTaskStateStatus(stateStatus);
	}catch(Tango::DeviceUnlocked &du){
		stateStatus.state = Tango::FAULT;
		stateStatus.status = "Device unlocked catched : Check ActuatorSystem device..";
		updateTaskStateStatus(stateStatus);
	}catch(Tango::DevFailed &df){
		stateStatus.state = Tango::FAULT;
		stateStatus.status = "Connection failed catched : Check ActuatorSystem device..";
		updateTaskStateStatus(stateStatus);
	}
	return tmpDataToReturn;
}

void BPTTaskManager::writeAttributeOnActuatorSystem(Tango::DeviceAttribute attrToWrite){
	yat::MutexLock gardActuatorMutex(_actuatorSystMutex);
	StateStatus stateStatus;
	try{
		 if (m_actuatorSystem && !m_simulatedMode)
		 	m_actuatorSystem->write_attribute(attrToWrite);
		else{
			stateStatus.state = Tango::FAULT;
			stateStatus.status = "Something went wrong with ActuatorSystem's Proxy initialisation... Try to init";
			updateTaskStateStatus(stateStatus);
		}
	}catch(Tango::ConnectionFailed &conf){
		stateStatus.state = Tango::FAULT;
		stateStatus.status = "Connection failed catched : Check ActuatorSystem device..";
		updateTaskStateStatus(stateStatus);
	}catch(Tango::CommunicationFailed &comf){
		stateStatus.state = Tango::FAULT;
		stateStatus.status = "Communication failed catched : Check ActuatorSystem device..";
		updateTaskStateStatus(stateStatus);
	}catch(Tango::DeviceUnlocked &du){
		stateStatus.state = Tango::FAULT;
		stateStatus.status = "Device unlocked catched : Check ActuatorSystem device..";
		updateTaskStateStatus(stateStatus);
	}catch(Tango::DevFailed &df){
		stateStatus.state = Tango::FAULT;
		stateStatus.status = "Connection failed catched : Check ActuatorSystem device..";
		updateTaskStateStatus(stateStatus);
	}
}

Tango::DeviceAttribute BPTTaskManager::readAttributeOnActuatorSystem(std::string attributeName){
	yat::MutexLock gardActuatorMutex(_actuatorSystMutex);
	StateStatus stateStatus;
	Tango::DeviceAttribute tmpAttributeToReturn;
	try{
		if (m_actuatorSystem){
			tmpAttributeToReturn = m_actuatorSystem->read_attribute(attributeName);
			if (tmpAttributeToReturn.get_quality() != Tango::ATTR_VALID){
				stateStatus.state = Tango::FAULT;
				stateStatus.status = "Something went wrong when trying to read attribute : "+ attributeName +" on ActuatorSystem, check his state before to init...";
				updateTaskStateStatus(stateStatus);
		   	}
	   	}
		else{
			stateStatus.state = Tango::FAULT;
			stateStatus.status = "Something went wrong when trying to read attribute : "+ attributeName +" on ActuatorSystem, Try to init";
			updateTaskStateStatus(stateStatus);
		}
	}catch(Tango::ConnectionFailed &conf){
		stateStatus.state = Tango::FAULT;
		stateStatus.status = "Something went wrong when trying to read attribute : "+ attributeName +" on ActuatorSystem, Check ActuatorSystem device..";
		updateTaskStateStatus(stateStatus);
	}catch(Tango::CommunicationFailed &comf){
		stateStatus.state = Tango::FAULT;
		stateStatus.status = "Something went wrong when trying to read attribute : "+ attributeName +" on ActuatorSystem,Check ActuatorSystem device..";
		updateTaskStateStatus(stateStatus);
	}catch(Tango::DeviceUnlocked &du){
		stateStatus.state = Tango::FAULT;
		stateStatus.status = "Something went wrong when trying to read attribute : "+ attributeName +" on ActuatorSystem,Check ActuatorSystem device..";
		updateTaskStateStatus(stateStatus);
	}catch(Tango::DevFailed &df){
		stateStatus.state = Tango::FAULT;
		stateStatus.status = "Something went wrong when trying to read attribute : "+ attributeName +" on ActuatorSystem, Check ActuatorSystem device..";
		updateTaskStateStatus(stateStatus);
	}
	catch(...){
		stateStatus.state = Tango::FAULT;
		stateStatus.status = "Connection unknow error while trying to read "+ attributeName + " on actuator system.";
		updateTaskStateStatus(stateStatus);
	}
	return tmpAttributeToReturn;
}

Tango::DevState BPTTaskManager::getActuatorSystemSate(){
	yat::MutexLock gardActuatorMutex(_actuatorSystMutex);
	Tango::DevState tmpState;
	Tango::DeviceAttribute tmpAttr;
	tmpAttr = readAttributeOnActuatorSystem(ATTR_STATE_STR);
	tmpAttr>>tmpState;
	return tmpState;
}

//+----------------------------------------------------------------------------
//
//						Sensor Plugin Access
//
//-----------------------------------------------------------------------------

BPT::SensorInterface::SensorInterface::sensorState BPTTaskManager::getFreshStateFromSensor(){
	yat::MutexLock gardPluginMutex(m_sensorPluginMutex);
	return m_sensorPlugin->getSensorState();
}

BPT::SensorInterface::SensorInterface::sensorData BPTTaskManager::getFreshDataFromSensor(){
	yat::MutexLock gardPluginMutex(m_sensorPluginMutex);
	return m_sensorPlugin->getSensorData();
}

double BPTTaskManager::getXCentroid(){
	yat::MutexLock gardPluginMutex(m_sensorPluginMutex);
	BPT::SensorInterface::SensorInterface::sensorData sensorData = m_sensorPlugin->getSensorData();
	
	return sensorData.xBeamPostionPixels;
}

double BPTTaskManager::getYCentroid(){
	yat::MutexLock gardPluginMutex(m_sensorPluginMutex);
	BPT::SensorInterface::SensorInterface::sensorData sensorData = m_sensorPlugin->getSensorData();
	
	return sensorData.yBeamPostionPixels;
}
} // Namespace BPTTaskManager




