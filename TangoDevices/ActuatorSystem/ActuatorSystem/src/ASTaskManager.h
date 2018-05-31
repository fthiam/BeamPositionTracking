// ============================================================================
//
// = CONTEXT
//    TANGO Project -
//
// = FILENAME
//    ASTaskManager.hpp
//
// = AUTHOR
//    F. Thiam
//
// ============================================================================

#ifndef _ACUATOR_SYSTEM_TASK_H_
#define _ACUATOR_SYSTEM_TASK_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <tango.h>
#include <yat4tango/DeviceTask.h>
#include <yat/utils/XString.h>
#include <yat4tango/LogHelper.h>
#include <yat4tango/DevicePlugInHelper.h>
#include "ActuatorInterface.h"
#include <yat/plugin/PlugInManager.h>
#include "ASPluginManager.h"

#include <time.h>
#include <chrono>
#include <ctime>

namespace ActuatorSystem_ns{
// ============================================================================
// class: ASTaskManager
// ============================================================================
class ASTaskManager : public yat4tango::DeviceTask
{

public:

    //- Initialization 
	ASTaskManager (Tango::DeviceImpl* devHost, yat4tango::DynamicAttributeManager* dynAttrManager,
					std::string xAxisPluginType, std::string xAxisDeviceAdress,
					std::string yAxisPluginType, std::string yAxisDeviceAdress, 
					std::string xPluginPath, std::string yPluginPath,
					int xAxisMinPositionInt, int xAxisMaxPositionInt,
					int yAxisMinPositionInt, int yAxisMaxPositionInt,
					bool simulatedMode);

	//- dtor
    virtual ~ASTaskManager (void);
    
    yat4tango::DynamicAttributeManager* _dynAttrManager;

    struct StateStatus{
		Tango::DevState state;
		std::string status;
	};

	struct AxesPositionningState{
		bool xAxisPositionOutOfBounds;
		bool yAxisPositionOutOfBounds;
    	// To inform user that limit has been reached 
    	// -> on at least on axis (precise in status...)
		bool xLimitReached;
		bool yLimitReached;
		std::string xLimitStatus;
		std::string yLimitStatus;
	};

	struct TaskManagerDataPacket{
		double xAxisPosition;
		double yAxisPosition;
		StateStatus taskStateStatus;
		StateStatus xStateStatus;
		StateStatus yStateStatus;
	};

	struct AxesUnits{
		std::string xAxisUnit;
		std::string yAxisUnit;
	};

	//GETTERS
	AxesUnits i_getAxesUnits();
	TaskManagerDataPacket i_getManagerDataPacket();


	//COMMANDS
	void i_setXAxisCalibrationState(bool state)throw (Tango::DevFailed);
	void i_setYAxisCalibrationState(bool state)throw (Tango::DevFailed);

	void i_moveXAxis(double movement);
	void i_moveYAxis(double movement);

	void i_moveAxes(double xAxisRelativeMovement, double yAxisRelativeMovement);

	void i_setXAxisPosition(double position);
	void i_setYAxisPosition(double position);

	void i_stopAxes();

protected:
	//- process_message (implements yat4tango::DeviceTask pure virtual method)
	virtual void process_message (yat::Message& msg)
    	throw (Tango::DevFailed);
		
	//Mutex for data access
	yat::Mutex m_dataMutex;

private:

	bool actuatorPluginInitialisation(std::string axisPluginType, std::string pluginPath, std::string axisId);
	bool startPlugins();
	void initAxesUnits();
	bool refreshAxes();
    void refreshTaskState();
	void moveAxisPosition(double relativeMovement, std::string axisId);
	void setAxisPosition(double position, std::string axisId);
	void stopAxesMovement();
    void updateTaskStateStatus(StateStatus stateStatus);
    StateStatus getTaskCurrentState();

    //Actuator plugin
    yat::IPlugInInfo* m_pluginInfo;
	yat::IPlugInFactory* m_pluginFactory;
	AS::ActuatorInterface::ActuatorInterface* m_xActuatorPlugin;
	AS::ActuatorInterface::ActuatorInterface* m_yActuatorPlugin;

	std::string m_xAxisPluginType;
	std::string m_xAxisDeviceAdress;
	std::string m_yAxisPluginType;
	std::string m_yAxisDeviceAdress;
	std::string m_xPluginPath;
 	std::string m_yPluginPath;

 	//Device mode 
 	bool m_simulatedMode;
 	int m_simulatedXTranslation;
	int m_simulatedYTranslation;
	
	//- the Tango device hosting 
    Tango::DeviceImpl * m_hostDev;
    bool m_movementStarted;
    TaskManagerDataPacket m_managerDataPacket;
    bool m_initCompleted;
    AxesUnits m_axesUnits;
    bool m_isXAxisCalibrated;
    bool m_isYAxisCalibrated;

    int m_xAxisMaxPosition;
	int m_xAxisMinPosition;
	int m_yAxisMaxPosition;
	int m_yAxisMinPosition;

	AxesPositionningState m_axesPositionningState;


};
} //end Namespace
#endif // _ACUATOR_SYSTEM_TASK_H_
