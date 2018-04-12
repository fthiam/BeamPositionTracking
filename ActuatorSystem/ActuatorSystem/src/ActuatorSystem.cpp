static const char *RcsId = "$Id:  $";
//+=============================================================================
//
// file :         ActuatorSystem.cpp
//
// description :  C++ source for the ActuatorSystem and its commands. 
//                The class is derived from Device. It represents the
//                CORBA servant object which will be accessed from the
//                network. All commands which can be executed on the
//                ActuatorSystem are implemented in this file.
//
// project :      TANGO Device Server
//
// $Author:  $
//
// $Revision:  $
//
// $Revision:  $
// $Date:  $
//
// SVN only:
// $HeadURL: $
//
// CVS only:
// $Source:  $
// $Log:  $
//
// copyleft :    Synchrotron SOLEIL 
//               L'Orme des merisiers - Saint Aubin
//               BP48 - 91192 Gif sur Yvette
//               FRANCE
//
//-=============================================================================
//
//  		This file is generated by POGO
//	(Program Obviously used to Generate tango Object)
//
//         (c) - Software Engineering Group - ESRF
//=============================================================================



//===================================================================
//
//	The following table gives the correspondence
//	between commands and method name.
//
//  Command name                 |  Method name
//	----------------------------------------
//  State                        |  dev_state()
//  Status                       |  dev_status()
//  StopAxes                     |  stop_axes()
//  MoveXAxisRelative            |  move_xaxis_relative()
//  MoveYAxisRelative            |  move_yaxis_relative()
//  ApplyRelativeMovementOnAxes  |  apply_relative_movement_on_axes()
//
//===================================================================


#include <tango.h>
#include <ActuatorSystem.h>
#include <ActuatorSystemClass.h>
#define SIMULATED_MODE std::string("SIMULATED")
#define NORMAL_MODE std::string("NORMAL")
#define PROPERTIES_OK std::string("OK")
namespace ActuatorSystem_ns
{

//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::ActuatorSystem(string &s)
// 
// description : 	constructor for simulated ActuatorSystem
//
// in : - cl : Pointer to the DeviceClass object
//      - s : Device name 
//
//-----------------------------------------------------------------------------
ActuatorSystem::ActuatorSystem(Tango::DeviceClass *cl,string &s)
:Tango::Device_4Impl(cl,s.c_str())
{
	init_device();
}

ActuatorSystem::ActuatorSystem(Tango::DeviceClass *cl,const char *s)
:Tango::Device_4Impl(cl,s)
{
	init_device();
}

ActuatorSystem::ActuatorSystem(Tango::DeviceClass *cl,const char *s,const char *d)
:Tango::Device_4Impl(cl,s,d)
{
	init_device();
}
//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::delete_device()
// 
// description : 	will be called at device destruction or at init command.
//
//-----------------------------------------------------------------------------
void ActuatorSystem::delete_device()
{

	if (_taskManager)
		delete _taskManager;

	// delete _dynAttrManager
	if (this->_dynAttrManager){
		try{
			_dynAttrManager->remove_attributes();
		}
		catch (...){
		}
		delete this->_dynAttrManager;
		this->_dynAttrManager = NULL;
	}
	
	//	Delete device allocated objects
	DELETE_SCALAR_ATTRIBUTE(attr_isYCalibrated_read);
	DELETE_SCALAR_ATTRIBUTE(attr_isXCalibrated_read);
	DELETE_SCALAR_ATTRIBUTE(attr_isSystemReady_read);

	//- remove the DeviceInfo
    yat4tango::DeviceInfo::release(this);
	//- remove the InnerAppender
    yat4tango::InnerAppender::release(this);

}
//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::init_device()
// 
// description : 	will be called at device initialization.
//
//-----------------------------------------------------------------------------
void ActuatorSystem::init_device()
{

	// Initialise variables to default values
	//-------------------------------------------- 
	get_device_property();
	CREATE_SCALAR_ATTRIBUTE(attr_isYCalibrated_read, false);
	CREATE_SCALAR_ATTRIBUTE(attr_isXCalibrated_read, false);
	CREATE_SCALAR_ATTRIBUTE(attr_isSystemReady_read, false);
	_dynAttrManager = 0;
    _taskManager = 0;
    this->_initDone = false;


    INFO_STREAM << "Create the InnerAppender in order to manage logs." << endl;
	yat4tango::InnerAppender::initialize(this);
	INFO_STREAM << "Create the DeviceInfo in order to manage info on versions." << endl;
    yat4tango::DeviceInfo::initialize( this, YAT_XSTR(PROJECT_NAME), YAT_XSTR(PROJECT_VERSION));
    yat4tango::DeviceInfo::add_dependency(this, "ActuatorSystem ", YAT_XSTR(actuatorSystem_PROJECT_VERSION));
 	INFO_STREAM << "ActuatorSystem::ActuatorSystem() create device " << device_name << endl;

  	std::string propertiesState = check_properties();
	
	if (propertiesState != PROPERTIES_OK){
        set_status (propertiesState);
        set_state (Tango::FAULT);
	}else{
	    bool simulated_mode = true;
		//check Mode
		if (deviceMode == NORMAL_MODE)
				simulated_mode = false;
		// Creation of dynAttrManager
		try{
			_dynAttrManager = new yat4tango::DynamicAttributeManager(this);
		}catch( Tango::DevFailed& df ){
	        ERROR_STREAM << df << std::endl;
	        set_status ("Initialization error [Tango Exception caught while instantiating DynamicAttributeManager]");
	        set_state (Tango::FAULT);
	        return;
	    }
	    catch(...){
	        std::string err_msg = "Initialization error [unknown exception caught while instantiating DynamicAttributeManager]";
	        ERROR_STREAM << err_msg << std::endl;
	        set_status (err_msg);
	        set_state (Tango::FAULT);
	        return;
	    }
	    int xAxisMaxPositionInt = 0;
	    int xAxisMinPositionInt = 0;
	    int yAxisMaxPositionInt = 0;
	    int yAxisMinPositionInt = 0;
	    xAxisMaxPositionInt = (int) xAxisMaxPosition;
	    xAxisMinPositionInt = (int) xAxisMinPosition;
	    yAxisMaxPositionInt = (int) yAxisMaxPosition;
	    yAxisMinPositionInt = (int) yAxisMinPosition;

		// Creation of ASTaskManager
		try{
			_taskManager = new ASTaskManager::ASTaskManager(this, _dynAttrManager,   
																	xAxisPluginType, xAxisDeviceAdress,
																	yAxisPluginType, yAxisDeviceAdress,
																	xAxisPluginPath, yAxisPluginPath,
																	xAxisMinPositionInt, xAxisMaxPositionInt,
																	yAxisMinPositionInt, yAxisMaxPositionInt,
																	simulated_mode);
		}catch( Tango::DevFailed& df ){
	        ERROR_STREAM << df << std::endl;
	        set_status ("Initialization error [Tango Exception caught while instantiating ASTaskManager]");
	        set_state (Tango::FAULT);
	        return;
	    }
	    catch(...){
	        std::string err_msg = "Initialization error [unknown exception caught while instantiating ASTaskManager]";
	        ERROR_STREAM << err_msg << std::endl;
	        set_status (err_msg);
	        set_state (Tango::FAULT);
	        return;
	    }
	    // Start ASTaskManager
	   try
	    {
	        if (_taskManager)
				_taskManager->go(500);
	        
	        set_state(Tango::INIT);
	        set_status("Device initialisation started...");
	    }
	    catch( Tango::DevFailed& df )
	    {
	    	std::cout<<"devFailed task go"<<std::endl;
	        ERROR_STREAM << df << std::endl;
	        set_status ("Initialization error [Tango Error: " +  std::string(df.errors[0].desc) + ", while starting ASTaskManager]");
	        set_state (Tango::FAULT);
	        _taskManager = NULL;
	        return;
	    }
	    catch(...)
	    {
	    	std::cout<<"(...) task go"<<std::endl;
	        std::string err_msg = "Initialization error [Unknown Error while starting ASTaskManager]";  
	        ERROR_STREAM << err_msg << std::endl;
	        set_status (err_msg);
	        set_state (Tango::FAULT);
	        _taskManager = NULL;
	        return;
	    }
	    this->_initDone = true;
	}
}


//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::get_device_property()
// 
// description : 	Read the device properties from database.
//
//-----------------------------------------------------------------------------
void ActuatorSystem::get_device_property()
{
	//	Initialize your default values here (if not done with  POGO).
	//------------------------------------------------------------------

	//	Read device properties from database.(Automatic code generation)
	//------------------------------------------------------------------
	Tango::DbData	dev_prop;
	dev_prop.push_back(Tango::DbDatum("XAxisPluginPath"));
	dev_prop.push_back(Tango::DbDatum("XAxisPluginType"));
	dev_prop.push_back(Tango::DbDatum("YAxisPluginType"));
	dev_prop.push_back(Tango::DbDatum("YAxisPluginPath"));
	dev_prop.push_back(Tango::DbDatum("XAxisDeviceAdress"));
	dev_prop.push_back(Tango::DbDatum("YAxisDeviceAdress"));
	dev_prop.push_back(Tango::DbDatum("XAxisMaxPosition"));
	dev_prop.push_back(Tango::DbDatum("XAxisMinPosition"));
	dev_prop.push_back(Tango::DbDatum("YAxisMaxPosition"));
	dev_prop.push_back(Tango::DbDatum("YAxisMinPosition"));
	dev_prop.push_back(Tango::DbDatum("DeviceMode"));

	//	Call database and extract values
	//--------------------------------------------
	if (Tango::Util::instance()->_UseDb==true)
		get_db_device()->get_property(dev_prop);
	Tango::DbDatum	def_prop, cl_prop;
	ActuatorSystemClass	*ds_class =
		(static_cast<ActuatorSystemClass *>(get_device_class()));
	int	i = -1;

	//	Try to initialize XAxisPluginPath from class property
	cl_prop = ds_class->get_class_property(dev_prop[++i].name);
	if (cl_prop.is_empty()==false)	cl_prop  >>  xAxisPluginPath;
	else {
		//	Try to initialize XAxisPluginPath from default device value
		def_prop = ds_class->get_default_device_property(dev_prop[i].name);
		if (def_prop.is_empty()==false)	def_prop  >>  xAxisPluginPath;
	}
	//	And try to extract XAxisPluginPath value from database
	if (dev_prop[i].is_empty()==false)	dev_prop[i]  >>  xAxisPluginPath;

	//	Try to initialize XAxisPluginType from class property
	cl_prop = ds_class->get_class_property(dev_prop[++i].name);
	if (cl_prop.is_empty()==false)	cl_prop  >>  xAxisPluginType;
	else {
		//	Try to initialize XAxisPluginType from default device value
		def_prop = ds_class->get_default_device_property(dev_prop[i].name);
		if (def_prop.is_empty()==false)	def_prop  >>  xAxisPluginType;
	}
	//	And try to extract XAxisPluginType value from database
	if (dev_prop[i].is_empty()==false)	dev_prop[i]  >>  xAxisPluginType;

	//	Try to initialize YAxisPluginType from class property
	cl_prop = ds_class->get_class_property(dev_prop[++i].name);
	if (cl_prop.is_empty()==false)	cl_prop  >>  yAxisPluginType;
	else {
		//	Try to initialize YAxisPluginType from default device value
		def_prop = ds_class->get_default_device_property(dev_prop[i].name);
		if (def_prop.is_empty()==false)	def_prop  >>  yAxisPluginType;
	}
	//	And try to extract YAxisPluginType value from database
	if (dev_prop[i].is_empty()==false)	dev_prop[i]  >>  yAxisPluginType;

	//	Try to initialize YAxisPluginPath from class property
	cl_prop = ds_class->get_class_property(dev_prop[++i].name);
	if (cl_prop.is_empty()==false)	cl_prop  >>  yAxisPluginPath;
	else {
		//	Try to initialize YAxisPluginPath from default device value
		def_prop = ds_class->get_default_device_property(dev_prop[i].name);
		if (def_prop.is_empty()==false)	def_prop  >>  yAxisPluginPath;
	}
	//	And try to extract YAxisPluginPath value from database
	if (dev_prop[i].is_empty()==false)	dev_prop[i]  >>  yAxisPluginPath;

	//	Try to initialize XAxisDeviceAdress from class property
	cl_prop = ds_class->get_class_property(dev_prop[++i].name);
	if (cl_prop.is_empty()==false)	cl_prop  >>  xAxisDeviceAdress;
	else {
		//	Try to initialize XAxisDeviceAdress from default device value
		def_prop = ds_class->get_default_device_property(dev_prop[i].name);
		if (def_prop.is_empty()==false)	def_prop  >>  xAxisDeviceAdress;
	}
	//	And try to extract XAxisDeviceAdress value from database
	if (dev_prop[i].is_empty()==false)	dev_prop[i]  >>  xAxisDeviceAdress;

	//	Try to initialize YAxisDeviceAdress from class property
	cl_prop = ds_class->get_class_property(dev_prop[++i].name);
	if (cl_prop.is_empty()==false)	cl_prop  >>  yAxisDeviceAdress;
	else {
		//	Try to initialize YAxisDeviceAdress from default device value
		def_prop = ds_class->get_default_device_property(dev_prop[i].name);
		if (def_prop.is_empty()==false)	def_prop  >>  yAxisDeviceAdress;
	}
	//	And try to extract YAxisDeviceAdress value from database
	if (dev_prop[i].is_empty()==false)	dev_prop[i]  >>  yAxisDeviceAdress;

	//	Try to initialize XAxisMaxPosition from class property
	cl_prop = ds_class->get_class_property(dev_prop[++i].name);
	if (cl_prop.is_empty()==false)	cl_prop  >>  xAxisMaxPosition;
	else {
		//	Try to initialize XAxisMaxPosition from default device value
		def_prop = ds_class->get_default_device_property(dev_prop[i].name);
		if (def_prop.is_empty()==false)	def_prop  >>  xAxisMaxPosition;
	}
	//	And try to extract XAxisMaxPosition value from database
	if (dev_prop[i].is_empty()==false)	dev_prop[i]  >>  xAxisMaxPosition;

	//	Try to initialize XAxisMinPosition from class property
	cl_prop = ds_class->get_class_property(dev_prop[++i].name);
	if (cl_prop.is_empty()==false)	cl_prop  >>  xAxisMinPosition;
	else {
		//	Try to initialize XAxisMinPosition from default device value
		def_prop = ds_class->get_default_device_property(dev_prop[i].name);
		if (def_prop.is_empty()==false)	def_prop  >>  xAxisMinPosition;
	}
	//	And try to extract XAxisMinPosition value from database
	if (dev_prop[i].is_empty()==false)	dev_prop[i]  >>  xAxisMinPosition;

	//	Try to initialize YAxisMaxPosition from class property
	cl_prop = ds_class->get_class_property(dev_prop[++i].name);
	if (cl_prop.is_empty()==false)	cl_prop  >>  yAxisMaxPosition;
	else {
		//	Try to initialize YAxisMaxPosition from default device value
		def_prop = ds_class->get_default_device_property(dev_prop[i].name);
		if (def_prop.is_empty()==false)	def_prop  >>  yAxisMaxPosition;
	}
	//	And try to extract YAxisMaxPosition value from database
	if (dev_prop[i].is_empty()==false)	dev_prop[i]  >>  yAxisMaxPosition;

	//	Try to initialize YAxisMinPosition from class property
	cl_prop = ds_class->get_class_property(dev_prop[++i].name);
	if (cl_prop.is_empty()==false)	cl_prop  >>  yAxisMinPosition;
	else {
		//	Try to initialize YAxisMinPosition from default device value
		def_prop = ds_class->get_default_device_property(dev_prop[i].name);
		if (def_prop.is_empty()==false)	def_prop  >>  yAxisMinPosition;
	}
	//	And try to extract YAxisMinPosition value from database
	if (dev_prop[i].is_empty()==false)	dev_prop[i]  >>  yAxisMinPosition;

	//	Try to initialize DeviceMode from class property
	cl_prop = ds_class->get_class_property(dev_prop[++i].name);
	if (cl_prop.is_empty()==false)	cl_prop  >>  deviceMode;
	else {
		//	Try to initialize DeviceMode from default device value
		def_prop = ds_class->get_default_device_property(dev_prop[i].name);
		if (def_prop.is_empty()==false)	def_prop  >>  deviceMode;
	}
	//	And try to extract DeviceMode value from database
	if (dev_prop[i].is_empty()==false)	dev_prop[i]  >>  deviceMode;



	//	End of Automatic code generation
	//------------------------------------------------------------------
	yat4tango::PropertyHelper::create_property_if_empty(this, dev_prop, "SimulatedAxis", "XAxisPluginType");
	yat4tango::PropertyHelper::create_property_if_empty(this, dev_prop, "SimulatedAxis", "YAxisPluginType");
	yat4tango::PropertyHelper::create_property_if_empty(this, dev_prop, "Precise/which/device", "XAxisPluginPath");
	yat4tango::PropertyHelper::create_property_if_empty(this, dev_prop, "Precise/which/device", "YAxisPluginPath");
	yat4tango::PropertyHelper::create_property_if_empty(this, dev_prop, "Precise/which/device", "XAxisDeviceAdress");
	yat4tango::PropertyHelper::create_property_if_empty(this, dev_prop, "Precise/which/device", "YAxisDeviceAdress");
	yat4tango::PropertyHelper::create_property_if_empty(this, dev_prop, "0", "XAxisMinPosition");
	yat4tango::PropertyHelper::create_property_if_empty(this, dev_prop, "1", "XAxisMaxPosition");
	yat4tango::PropertyHelper::create_property_if_empty(this, dev_prop, "0", "YAxisMinPosition");
	yat4tango::PropertyHelper::create_property_if_empty(this, dev_prop, "1", "YAxisMaxPosition");
	yat4tango::PropertyHelper::create_property_if_empty(this, dev_prop, "SIMULATED", "DeviceMode");

}
//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::always_executed_hook()
// 
// description : 	method always executed before any command is executed
//
//-----------------------------------------------------------------------------
void ActuatorSystem::always_executed_hook()
{
	if(_initDone){
		if (_taskManager){
			// Refresh beam data 
			ASTaskManager::ASTaskManager::TaskManagerDataPacket managerDataPacket = _taskManager->i_getManagerDataPacket();
			// Update attributs values 
			_xPosition = managerDataPacket.xAxisPosition;
			_yPosition = managerDataPacket.yAxisPosition;
			_xAxisState = managerDataPacket.xStateStatus.state;
			_yAxisState = managerDataPacket.yStateStatus.state;
			// Set Device state and status
			set_state(managerDataPacket.taskStateStatus.state);
			set_status(managerDataPacket.taskStateStatus.status);
		}
	}	
}
//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::read_attr_hardware
// 
// description : 	Hardware acquisition for attributes.
//
//-----------------------------------------------------------------------------
void ActuatorSystem::read_attr_hardware(vector<long> &attr_list)
{
	DEBUG_STREAM << "ActuatorSystem::read_attr_hardware(vector<long> &attr_list) entering... "<< endl;
	//	Add your own code here
}
//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::read_isSystemReady
// 
// description : 	Extract real attribute values for isSystemReady acquisition result.
//
//-----------------------------------------------------------------------------
void ActuatorSystem::read_isSystemReady(Tango::Attribute &attr)
{
	DEBUG_STREAM << "ActuatorSystem::read_isSystemReady(Tango::Attribute &attr) entering... "<< endl;
	if (*attr_isXCalibrated_read && *attr_isYCalibrated_read && (this->get_state() == Tango::ON))
		*attr_isSystemReady_read = true;
	else
		*attr_isSystemReady_read = false;
	attr.set_value(attr_isSystemReady_read);
}


//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::read_xPosition
// 
// description : 	Extract real attribute values for xPosition acquisition result.
//
//-----------------------------------------------------------------------------
void ActuatorSystem::read_xPosition(Tango::Attribute &attr)
{
	DEBUG_STREAM << "ActuatorSystem::read_xPosition(Tango::Attribute &attr) entering... "<< endl;
	attr.set_value(&_xPosition);
}

//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::read_yPosition
// 
// description : 	Extract real attribute values for yPosition acquisition result.
//
//-----------------------------------------------------------------------------
void ActuatorSystem::read_yPosition(Tango::Attribute &attr)
{
	DEBUG_STREAM << "ActuatorSystem::read_yPosition(Tango::Attribute &attr) entering... "<< endl;
	attr.set_value(&_yPosition);
}

//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::read_xState
// 
// description : 	Extract real attribute values for xState acquisition result.
//
//-----------------------------------------------------------------------------
void ActuatorSystem::read_xState(Tango::Attribute &attr)
{
	DEBUG_STREAM << "ActuatorSystem::read_xState(Tango::Attribute &attr) entering... "<< endl;
	attr.set_value(&_xAxisState);
}

//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::read_yState
// 
// description : 	Extract real attribute values for yState acquisition result.
//
//-----------------------------------------------------------------------------
void ActuatorSystem::read_yState(Tango::Attribute &attr)
{
	DEBUG_STREAM << "ActuatorSystem::read_yState(Tango::Attribute &attr) entering... "<< endl;
	attr.set_value(&_yAxisState);
}

//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::read_isXLinear
// 
// description : 	Extract real attribute values for isXLinear acquisition result.
//
//-----------------------------------------------------------------------------
void ActuatorSystem::read_isXLinear(Tango::Attribute &attr)
{
	DEBUG_STREAM << "ActuatorSystem::read_isXLinear(Tango::Attribute &attr) entering... "<< endl;
	bool trueTrue = true;
	attr.set_value(&trueTrue);
}

//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::write_isXLinear
// 
// description : 	Write isXLinear attribute values to hardware.
//
//-----------------------------------------------------------------------------
void ActuatorSystem::write_isXLinear(Tango::WAttribute &attr)
{
	DEBUG_STREAM << "ActuatorSystem::write_isXLinear(Tango::WAttribute &attr) entering... "<< endl;
}

//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::read_isYLinear
// 
// description : 	Extract real attribute values for isYLinear acquisition result.
//
//-----------------------------------------------------------------------------
void ActuatorSystem::read_isYLinear(Tango::Attribute &attr)
{
	DEBUG_STREAM << "ActuatorSystem::read_isYLinear(Tango::Attribute &attr) entering... "<< endl;
	bool trueTrue = true;
	attr.set_value(&trueTrue);
}

//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::write_isYLinear
// 
// description : 	Write isYLinear attribute values to hardware.
//
//-----------------------------------------------------------------------------
void ActuatorSystem::write_isYLinear(Tango::WAttribute &attr)
{
	DEBUG_STREAM << "ActuatorSystem::write_isYLinear(Tango::WAttribute &attr) entering... "<< endl;
}

//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::read_isXCalibrated
// 
// description : 	Extract real attribute values for isXCalibrated acquisition result.
//
//-----------------------------------------------------------------------------
void ActuatorSystem::read_isXCalibrated(Tango::Attribute &attr)
{
	DEBUG_STREAM << "ActuatorSystem::read_isXCalibrated(Tango::Attribute &attr) entering... "<< endl;

	attr.set_value(attr_isXCalibrated_read);
}

//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::write_isXCalibrated
// 
// description : 	Write isXCalibrated attribute values to hardware.
//
//-----------------------------------------------------------------------------
void ActuatorSystem::write_isXCalibrated(Tango::WAttribute &attr)
{
	DEBUG_STREAM << "ActuatorSystem::write_isXCalibrated(Tango::WAttribute &attr) entering... "<< endl;

	if(_initDone){
		if (_taskManager){
			attr.get_write_value(attr_isXCalibrated_write);
			double tmpXRatio = attr_xLinearRatio_write;
			//Check value of xLinearRatio
			if(attr_isXCalibrated_write){
				if(tmpXRatio != 0){
					_taskManager->i_setXAxisCalibrationState(true);
					//Update read value
					*attr_isXCalibrated_read = attr_isXCalibrated_write;
				}
				else
				{
					Tango::Except::throw_exception 
						((const char *)"X linear ratio incorrect",
						 (const char *)"X linear ratio must be different than 0",
						 (const char *)"ActuatorSystem::write_isXCalibrated(Tango::WAttribute &attr)");
				}
			}
			else{
				*attr_isXCalibrated_read = attr_isXCalibrated_write;
				_taskManager->i_setXAxisCalibrationState(false);
			}
		}
	}
}

//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::read_isYCalibrated
// 
// description : 	Extract real attribute values for isYCalibrated acquisition result.
//
//-----------------------------------------------------------------------------
void ActuatorSystem::read_isYCalibrated(Tango::Attribute &attr)
{
	DEBUG_STREAM << "ActuatorSystem::read_isYCalibrated(Tango::Attribute &attr) entering... "<< endl;
	attr.set_value(attr_isYCalibrated_read);
}

//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::write_isYCalibrated
// 
// description : 	Write isYCalibrated attribute values to hardware.
//
//-----------------------------------------------------------------------------
void ActuatorSystem::write_isYCalibrated(Tango::WAttribute &attr)
{
	DEBUG_STREAM << "ActuatorSystem::write_isYCalibrated(Tango::WAttribute &attr) entering... "<< endl;
	if(_initDone){
		if (_taskManager){
			attr.get_write_value(attr_isYCalibrated_write);
			double tmpYRatio = attr_yLinearRatio_write;
			//Check value of xLinearRatio
			if(attr_isYCalibrated_write){
				if(tmpYRatio != 0){
					//To apply last value of ratio, will also set calibrated state to true
					_taskManager->i_setYAxisCalibrationState(true);
					//Update read value
					*attr_isYCalibrated_read = attr_isYCalibrated_write;
				}
				else
				{
					Tango::Except::throw_exception 
						((const char *)"Y linear ratio incorrect",
						 (const char *)"Y linear ratio must be different than 0",
						 (const char *)"ActuatorSystem::write_isXCalibrated(Tango::WAttribute &attr)");
				}
			}	
			else{
				*attr_isYCalibrated_read = attr_isYCalibrated_write;
				_taskManager->i_setYAxisCalibrationState(false);
			}
		}
	}
}

//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::read_xLinearRatio
// 
// description : 	Extract real attribute values for xLinearRatio acquisition result.
//
//-----------------------------------------------------------------------------
void ActuatorSystem::read_xLinearRatio(Tango::Attribute &attr)
{
	DEBUG_STREAM << "ActuatorSystem::read_xLinearRatio(Tango::Attribute &attr) entering... "<< endl;
	attr.set_value(&attr_xLinearRatio_write);
}

//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::write_xLinearRatio
// 
// description : 	Write xLinearRatio attribute values to hardware.
//
//-----------------------------------------------------------------------------
void ActuatorSystem::write_xLinearRatio(Tango::WAttribute &attr)
{
	DEBUG_STREAM << "ActuatorSystem::write_xLinearRatio(Tango::WAttribute &attr) entering... "<< endl;
	attr.get_write_value(attr_xLinearRatio_write);
}

//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::read_yLinearRatio
// 
// description : 	Extract real attribute values for yLinearRatio acquisition result.
//
//-----------------------------------------------------------------------------
void ActuatorSystem::read_yLinearRatio(Tango::Attribute &attr)
{
	DEBUG_STREAM << "ActuatorSystem::read_yLinearRatio(Tango::Attribute &attr) entering... "<< endl;
	attr.set_value(&attr_yLinearRatio_write);
}

//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::write_yLinearRatio
// 
// description : 	Write yLinearRatio attribute values to hardware.
//
//-----------------------------------------------------------------------------
void ActuatorSystem::write_yLinearRatio(Tango::WAttribute &attr)
{
	DEBUG_STREAM << "ActuatorSystem::write_yLinearRatio(Tango::WAttribute &attr) entering... "<< endl;
	attr.get_write_value(attr_yLinearRatio_write);
}

//+------------------------------------------------------------------
/**
 *	method:	ActuatorSystem::stop_axes
 *
 *	description:	method to execute "StopAxes"
 *
 *
 */
//+------------------------------------------------------------------
void ActuatorSystem::stop_axes()
{
	DEBUG_STREAM << "ActuatorSystem::stop_axes(): entering... !" << endl;
	if(_initDone){
		if (_taskManager){
			try{
				_taskManager->i_stopAxes();
			}catch(Tango::DevFailed &df){
				RETHROW_DEVFAILED(df,"","","");
			}
		}
	}
}

//+------------------------------------------------------------------
/**
 *	method:	ActuatorSystem::move_xaxis_relative
 *
 *	description:	method to execute "MoveXAxisRelative"
 *
 * @param	argin	
 *
 */
//+------------------------------------------------------------------
void ActuatorSystem::move_xaxis_relative(Tango::DevDouble argin)
{
	DEBUG_STREAM << "ActuatorSystem::move_xaxis_relative(): entering... !" << endl;
	if(_initDone){
		if (_taskManager){
			if(argin != 0){
				try{
					set_state(Tango::MOVING);
					set_status("Starting movement on x axis...");

					double relativeMovement = argin;
					_taskManager->i_moveXAxis(relativeMovement);
				}catch(Tango::DevFailed &df){
					 RETHROW_DEVFAILED(df,"","","");
				}
			}
		}
	}
}

//+------------------------------------------------------------------
/**
 *	method:	ActuatorSystem::move_yaxis_relative
 *
 *	description:	method to execute "MoveYAxisRelative"
 *
 * @param	argin	
 *
 */
//+------------------------------------------------------------------
void ActuatorSystem::move_yaxis_relative(Tango::DevDouble argin)
{
	DEBUG_STREAM << "ActuatorSystem::move_yaxis_relative(): entering... !" << endl;
	if(_initDone){
		if (_taskManager){
			if(argin != 0){
				try{
					set_state(Tango::MOVING);
					set_status("Starting movement on y axis...");

					double relativeMovement = argin;
					_taskManager->i_moveYAxis(relativeMovement);
				}catch(Tango::DevFailed &df){
					 RETHROW_DEVFAILED(df,"","","");
				}
			}
		}
	}
}

//+------------------------------------------------------------------
/**
 *	method:	ActuatorSystem::apply_relative_movement_on_axes
 *
 *	description:	method to execute "ApplyRelativeMovementOnAxes"
 *	Argin[0] = movementOnXAxis
 *	Argin[1] = movementOnYAxis
 *
 * @param	argin	
 *
 */
//+------------------------------------------------------------------
void ActuatorSystem::apply_relative_movement_on_axes(const Tango::DevVarDoubleArray *argin)
{
	DEBUG_STREAM << "ActuatorSystem::apply_relative_movement_on_axes(): entering... !" << endl;

	if(_initDone){
		if (_taskManager){
			double xRelativeMovement; 
			double yRelativeMovement;
			xRelativeMovement = (double)(*argin)[0]; 
			yRelativeMovement = (double)(*argin)[1];

			if ((xRelativeMovement != 0) || (yRelativeMovement != 0)){

				set_state(Tango::MOVING);
				set_status("Applying pixel correction on axes");

				try{
					double xRelativeMovementToSet = (double)(attr_xLinearRatio_write * xRelativeMovement);
					double yRelativeMovementToSet = (double)(attr_yLinearRatio_write * yRelativeMovement);
					_taskManager->i_moveAxes(xRelativeMovementToSet, yRelativeMovementToSet);
				}catch(Tango::DevFailed &df){
					 RETHROW_DEVFAILED(df,"","","");
				}
			}
		}
	}
}

//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::check_properties
// 
// description : 	To check if properties are ok...
//
//-----------------------------------------------------------------------------
std::string ActuatorSystem::check_properties(){
	if (yAxisMinPosition >= yAxisMaxPosition)
		return "YAxisMinPosition can't be superior to YAxisMaxPosition";
	if (xAxisMinPosition >= xAxisMaxPosition)
		return "XAxisMinPosition can't be superior to XAxisMaxPosition";
	if ((deviceMode != SIMULATED_MODE) && (deviceMode != NORMAL_MODE))
		return "Please check mode, has to be either SIMULATED or NORMAL";

	return PROPERTIES_OK;
}






}	//	namespace
