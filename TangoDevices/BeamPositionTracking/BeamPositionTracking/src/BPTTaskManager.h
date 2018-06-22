// ============================================================================
//
// = CONTEXT
//    TANGO Project -
//
// = FILENAME
//    BPTTaskManager.hpp
//
// = AUTHOR
//    F. Thiam
//
// ============================================================================

#ifndef _MY_DEVICE_TASK_H_
#define _MY_DEVICE_TASK_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <tango.h>
#include <yat4tango/DeviceTask.h>
#include <yat/utils/XString.h>
#include <yat4tango/DevicePlugInHelper.h>
#include <yat4tango/DynamicAttributeManager.h>


#include "PIDCorrector.hpp"
#include "BPTPluginManager.h"
#include "SensorInterface.h"
#include "math.h"
#include "BPTDefinitions.h"

#include <time.h>
#include <chrono>
#include <ctime>

namespace BPTTaskManager{
// ============================================================================
// class: BPTTaskManager
// ============================================================================
class BPTTaskManager : public yat4tango::DeviceTask
{

public:

    //- Initialization 
	BPTTaskManager (Tango::DeviceImpl* devHost, yat4tango::DynamicAttributeManager* dynAttrManager,
							std::string pluginType, std::string pluginPath, std::string actuatorSystDeviceAdr, 
								PIDCoefficient xPidCoef, PIDCoefficient yPIDCoefficient,
								double xAxisNbStepToCalibrate, double yAxisNbStepToCalibrate, int nbImgToAlign, int taskPeriod, bool simulatedMode, FixedModeDefinition fixedMode);

	//- dtor
    virtual ~BPTTaskManager (void);
    
    yat4tango::DynamicAttributeManager* m_dynAttrManager;

    // Task interface
    void i_runSystemCalibration()throw (Tango::DevFailed);

	void i_turnTrackingModeOn(Target target)throw (Tango::DevFailed);

	void i_turnTrackingModeOff();

	void i_setWarningZone(WarningZone warningZone);

	void i_setXAxisRegThreshold(ushort newXRegThreshold);

	void i_setYAxisRegThreshold(ushort newYRegThreshold);

	void i_setTrackingTarget(Target target);

	StateStatus i_getStateStatus();

	ManagerDataPacket i_getManagerDataPacket();

	void i_acknowlegeStep();

	bool sensorPluginInitialisation();

	std::string i_getSensorUnit();

protected:
	//- process_message (implements yat4tango::DeviceTask pure virtual method)
	virtual void process_message (yat::Message& msg)
    	throw (Tango::DevFailed);
		
	//Mutex for data access
	yat::Mutex _dataMutex;
	//Mutex for actuatorSystem Access
	yat::Mutex m_sensorPluginMutex;
	//Mutex for actuatorSystem Access
	yat::Mutex _actuatorSystMutex;

//METHODS
private:
    //-----------------------------------------//
	//Initialisation
	bool initializeAxesInformations();    
	void initPIDCorrectors();
	void resetXPIDCorrector();
	void resetYPIDCorrector();
	//bool sensorPluginInitialisation();
	void startPlugin();
	bool actuatorSystemProxyInitialisation();

	void waitForUserAcknowlegement();
	//PERIODIC actions
	void periodicJob();
	void refreshDataFromSensorPlugin();
	Tango::DevState refreshDataFromActuatorSystem();
	//Diagnostic
	bool checkIfBeamIsInZone();
	double getXCentroid();
	double getYCentroid();
	//Calibration
	bool runSystemCalibration();
	void runLinearCalibration(Axis axis);
	void runNonLinearCalibration(std::string axisID);
	//Tracking
	void runBeamTracking();
	//Update
	void updateTaskStateStatus(StateStatus stateStus);
	void updateComputedTime(double computedTime);
	BPT::SensorInterface::SensorInterface::sensorData getFreshDataFromSensor();
	BPT::SensorInterface::SensorInterface::sensorState getFreshStateFromSensor();
	Tango::DeviceAttribute readAttributeOnActuatorSystem(std::string attributeName);
	Tango::DevState getActuatorSystemSate();
	//Command
	void writeAttributeOnActuatorSystem(Tango::DeviceAttribute attrToWrite);
	Tango::DeviceData sendCommandToActuatorSystem(std::string cmdName);
	Tango::DeviceData sendCommandToActuatorSystem(std::string cmdName, Tango::DeviceData args);

//MEMBERS
private:

	// CONFIG DOMAIN
    WarningZone m_warningZoneDefinition;
    int m_nbImgToAlign;
    PIDCorrector::PIDCorrector *m_pidXCorrector;
	PIDCorrector::PIDCorrector *m_pidYCorrector;
	
	double m_xTargetedCentroid;
    double m_yTargetedCentroid;
    bool m_simulatedMode;
    bool m_stepAcknowleged;
	// CONFIG SYSTEM
	bool m_initCompleted;
	//   Tango device hosting this task
    Tango::DeviceImpl * m_hostDev;
    int m_taskPeriod;
	// Sensor plugin
	std::string m_sensorUnit;
	yat::IPlugInInfo* m_pluginInfo;
	yat::IPlugInFactory* m_pluginFactory;
    std::string m_pluginType;
    std::string m_pluginPath;
	BPT::SensorInterface::SensorInterface* m_sensorPlugin;
	// Actuator system
	std::string m_actuatorSystDeviceAdress;
	Tango::DeviceProxy* m_actuatorSystem;

	// DATA
	// System state
	StateStatus m_managerStateStatus;
	int m_calibrationTryCounter;
	bool m_isSystemReady;
    bool m_isBeamInZone;
    bool m_isBeamLost;
    bool m_isTrackingModeRunning;
    bool m_isCalibrationRunning;
    std::chrono::high_resolution_clock::time_point m_correctionLoopTime;
    double m_computedTime;
	//Axes data
    Axis m_xAxisData;
    Axis m_yAxisData;
    //Sensor data
	BPT::SensorInterface::SensorInterface::sensorData m_sensorData; 
	std::vector<BPT::SensorInterface::SensorInterface::sensorData> m_sensorDataList;
	BPT::SensorInterface::SensorInterface::sensorState m_sensorState;


	//FixedMode 	
	FixedModeDefinition m_fixedModeDef;
};
} //end Namespace
#endif // _MY_DEVICE_TASK_H_
