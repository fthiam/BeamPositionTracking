static const char *ClassId    = "$Id:  $";
static const char *TagName    = "$Name:  $";
static const char *CvsPath    = "$Source:  $";
static const char *SvnPath    = "$HeadURL: $";
static const char *HttpServer = "http://www.esrf.fr/computing/cs/tango/tango_doc/ds_doc/";
//+=============================================================================
//
// file :        BeamPositionTrackingClass.cpp
//
// description : C++ source for the BeamPositionTrackingClass. A singleton
//               class derived from DeviceClass. It implements the
//               command list and all properties and methods required
//               by the BeamPositionTracking once per process.
//
// project :     TANGO Device Server
//
// $Author:  $
//
// $Revision:  $
//
// $Log:  $
//
// copyleft :   European Synchrotron Radiation Facility
//              BP 220, Grenoble 38043
//              FRANCE
//
//-=============================================================================
//
//  		This file is generated by POGO
//	(Program Obviously used to Generate tango Object)
//
//         (c) - Software Engineering Group - ESRF
//=============================================================================


#include <tango.h>

#include <BeamPositionTracking.h>
#include <BeamPositionTrackingClass.h>


//+----------------------------------------------------------------------------
/**
 *	Create BeamPositionTrackingClass singleton and return it in a C function for Python usage
 */
//+----------------------------------------------------------------------------
extern "C" {
#ifdef WIN32

	__declspec(dllexport)

#endif

	Tango::DeviceClass *_create_BeamPositionTracking_class(const char *name) {
		return BeamPositionTracking_ns::BeamPositionTrackingClass::init(name);
	}
}


namespace BeamPositionTracking_ns
{
//+----------------------------------------------------------------------------
//
// method : 		StopBeamTrackingCmd::execute()
// 
// description : 	method to trigger the execution of the command.
//                PLEASE DO NOT MODIFY this method core without pogo   
//
// in : - device : The device on which the command must be executed
//		- in_any : The command input data
//
// returns : The command output data (packed in the Any object)
//
//-----------------------------------------------------------------------------
	CORBA::Any *StopBeamTrackingCmd::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
	{

	cout2 << "StopBeamTrackingCmd::execute(): arrived" << endl;

	((static_cast<BeamPositionTracking *>(device))->stop_beam_tracking());
	return new CORBA::Any();
}

//+----------------------------------------------------------------------------
//
// method : 		StartBeamTrackingCmd::execute()
// 
// description : 	method to trigger the execution of the command.
//                PLEASE DO NOT MODIFY this method core without pogo   
//
// in : - device : The device on which the command must be executed
//		- in_any : The command input data
//
// returns : The command output data (packed in the Any object)
//
//-----------------------------------------------------------------------------
	CORBA::Any *StartBeamTrackingCmd::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
	{

	cout2 << "StartBeamTrackingCmd::execute(): arrived" << endl;

	((static_cast<BeamPositionTracking *>(device))->start_beam_tracking());
	return new CORBA::Any();
}

//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystemCalibrationCmd::execute()
// 
// description : 	method to trigger the execution of the command.
//                PLEASE DO NOT MODIFY this method core without pogo   
//
// in : - device : The device on which the command must be executed
//		- in_any : The command input data
//
// returns : The command output data (packed in the Any object)
//
//-----------------------------------------------------------------------------
	CORBA::Any *ActuatorSystemCalibrationCmd::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
	{

	cout2 << "ActuatorSystemCalibrationCmd::execute(): arrived" << endl;

	((static_cast<BeamPositionTracking *>(device))->actuator_system_calibration());
	return new CORBA::Any();
}

//+----------------------------------------------------------------------------
//
// method : 		AcknowlegeStepCmd::execute()
// 
// description : 	method to trigger the execution of the command.
//                PLEASE DO NOT MODIFY this method core without pogo   
//
// in : - device : The device on which the command must be executed
//		- in_any : The command input data
//
// returns : The command output data (packed in the Any object)
//
//-----------------------------------------------------------------------------
	CORBA::Any *AcknowlegeStepCmd::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
	{

	cout2 << "AcknowlegeStepCmd::execute(): arrived" << endl;

	((static_cast<BeamPositionTracking *>(device))->acknowlege_step());
	return new CORBA::Any();
}






//
//----------------------------------------------------------------
//	Initialize pointer for singleton pattern
//----------------------------------------------------------------
//
BeamPositionTrackingClass *BeamPositionTrackingClass::_instance = NULL;

//+----------------------------------------------------------------------------
//
// method : 		BeamPositionTrackingClass::BeamPositionTrackingClass(string &s)
// 
// description : 	constructor for the BeamPositionTrackingClass
//
// in : - s : The class name
//
//-----------------------------------------------------------------------------
	BeamPositionTrackingClass::BeamPositionTrackingClass(string &s):DeviceClass(s)
	{

		cout2 << "Entering BeamPositionTrackingClass constructor" << endl;
		set_default_property();
		get_class_property();
		write_class_property();

		cout2 << "Leaving BeamPositionTrackingClass constructor" << endl;

	}
//+----------------------------------------------------------------------------
//
// method : 		BeamPositionTrackingClass::~BeamPositionTrackingClass()
// 
// description : 	destructor for the BeamPositionTrackingClass
//
//-----------------------------------------------------------------------------
	BeamPositionTrackingClass::~BeamPositionTrackingClass()
	{
		_instance = NULL;
	}

//+----------------------------------------------------------------------------
//
// method : 		BeamPositionTrackingClass::instance
// 
// description : 	Create the object if not already done. Otherwise, just
//			return a pointer to the object
//
// in : - name : The class name
//
//-----------------------------------------------------------------------------
BeamPositionTrackingClass *BeamPositionTrackingClass::init(const char *name)
{
	if (_instance == NULL)
	{
		try
		{
			string s(name);
			_instance = new BeamPositionTrackingClass(s);
		}
		catch (bad_alloc)
		{
			throw;
		}		
	}		
	return _instance;
}

BeamPositionTrackingClass *BeamPositionTrackingClass::instance()
{
	if (_instance == NULL)
	{
		cerr << "Class is not initialised !!" << endl;
		exit(-1);
	}
	return _instance;
}

//+----------------------------------------------------------------------------
//
// method : 		BeamPositionTrackingClass::command_factory
// 
// description : 	Create the command object(s) and store them in the 
//			command list
//
//-----------------------------------------------------------------------------
void BeamPositionTrackingClass::command_factory()
{
	command_list.push_back(new ActuatorSystemCalibrationCmd("ActuatorSystemCalibration",
		Tango::DEV_VOID, Tango::DEV_VOID,
		"",
		"",
		Tango::OPERATOR));
	command_list.push_back(new StartBeamTrackingCmd("StartBeamTracking",
		Tango::DEV_VOID, Tango::DEV_VOID,
		"",
		"",
		Tango::OPERATOR));
	command_list.push_back(new StopBeamTrackingCmd("StopBeamTracking",
		Tango::DEV_VOID, Tango::DEV_VOID,
		"",
		"",
		Tango::OPERATOR));
	command_list.push_back(new AcknowlegeStepCmd("AcknowlegeStep",
		Tango::DEV_VOID, Tango::DEV_VOID,
		"",
		"",
		Tango::EXPERT));

	//	add polling if any
	for (unsigned int i=0 ; i<command_list.size(); i++)
	{
	}
}

//+----------------------------------------------------------------------------
//
// method : 		BeamPositionTrackingClass::get_class_property
// 
// description : 	Get the class property for specified name.
//
// in :		string	name : The property name
//
//+----------------------------------------------------------------------------
Tango::DbDatum BeamPositionTrackingClass::get_class_property(string &prop_name)
{
	for (unsigned int i=0 ; i<cl_prop.size() ; i++)
		if (cl_prop[i].name == prop_name)
			return cl_prop[i];
	//	if not found, return  an empty DbDatum
	return Tango::DbDatum(prop_name);
}
//+----------------------------------------------------------------------------
//
// method : 		BeamPositionTrackingClass::get_default_device_property()
// 
// description : 	Return the default value for device property.
//
//-----------------------------------------------------------------------------
Tango::DbDatum BeamPositionTrackingClass::get_default_device_property(string &prop_name)
{
	for (unsigned int i=0 ; i<dev_def_prop.size() ; i++)
		if (dev_def_prop[i].name == prop_name)
			return dev_def_prop[i];
	//	if not found, return  an empty DbDatum
	return Tango::DbDatum(prop_name);
}

//+----------------------------------------------------------------------------
//
// method : 		BeamPositionTrackingClass::get_default_class_property()
// 
// description : 	Return the default value for class property.
//
//-----------------------------------------------------------------------------
Tango::DbDatum BeamPositionTrackingClass::get_default_class_property(string &prop_name)
{
	for (unsigned int i=0 ; i<cl_def_prop.size() ; i++)
		if (cl_def_prop[i].name == prop_name)
			return cl_def_prop[i];
	//	if not found, return  an empty DbDatum
	return Tango::DbDatum(prop_name);
}
//+----------------------------------------------------------------------------
//
// method : 		BeamPositionTrackingClass::device_factory
// 
// description : 	Create the device object(s) and store them in the 
//			device list
//
// in :		Tango::DevVarStringArray *devlist_ptr : The device name list
//
//-----------------------------------------------------------------------------
void BeamPositionTrackingClass::device_factory(const Tango::DevVarStringArray *devlist_ptr)
{

	//	Create all devices.(Automatic code generation)
	//-------------------------------------------------------------
	for (unsigned long i=0 ; i < devlist_ptr->length() ; i++)
	{
		cout4 << "Device name : " << (*devlist_ptr)[i].in() << endl;
						
		// Create devices and add it into the device list
		//----------------------------------------------------
		device_list.push_back(new BeamPositionTracking(this, (*devlist_ptr)[i]));							 

		// Export device to the outside world
		// Check before if database used.
		//---------------------------------------------
		if ((Tango::Util::_UseDb == true) && (Tango::Util::_FileDb == false))
			export_device(device_list.back());
		else
			export_device(device_list.back(), (*devlist_ptr)[i]);
	}
	//	End of Automatic code generation
	//-------------------------------------------------------------

				}
//+----------------------------------------------------------------------------
//	Method: BeamPositionTrackingClass::attribute_factory(vector<Tango::Attr *> &att_list)
//-----------------------------------------------------------------------------
void BeamPositionTrackingClass::attribute_factory(vector<Tango::Attr *> &att_list)
{
	//	Attribute : computedTime
	computedTimeAttrib	*computed_time = new computedTimeAttrib();
	Tango::UserDefaultAttrProp	computed_time_prop;
	computed_time_prop.set_label("Computed time");
	computed_time_prop.set_unit("ms");
	computed_time_prop.set_description("Time of the calculation in ms");
	computed_time->set_default_properties(computed_time_prop);
	att_list.push_back(computed_time);

	//	Attribute : xAxisTarget
	xAxisTargetAttrib	*x_axis_target = new xAxisTargetAttrib();
	Tango::UserDefaultAttrProp	x_axis_target_prop;
	x_axis_target_prop.set_unit("Pixels");
	x_axis_target_prop.set_description("Define xAxis target in pixels");
	x_axis_target->set_default_properties(x_axis_target_prop);
	att_list.push_back(x_axis_target);

	//	Attribute : yAxisTarget
	yAxisTargetAttrib	*y_axis_target = new yAxisTargetAttrib();
	Tango::UserDefaultAttrProp	y_axis_target_prop;
	y_axis_target_prop.set_unit("Pixels");
	y_axis_target_prop.set_description("Define yAxis target in pixels");
	y_axis_target->set_default_properties(y_axis_target_prop);
	att_list.push_back(y_axis_target);

	//	Attribute : xAxisCurrentBeamPosition
	xAxisCurrentBeamPositionAttrib	*x_axis_current_beam_position = new xAxisCurrentBeamPositionAttrib();
	Tango::UserDefaultAttrProp	x_axis_current_beam_position_prop;
	x_axis_current_beam_position_prop.set_unit("Pixels");
	x_axis_current_beam_position_prop.set_description("Read current beam position on x axis");
	x_axis_current_beam_position->set_default_properties(x_axis_current_beam_position_prop);
	att_list.push_back(x_axis_current_beam_position);

	//	Attribute : yAxisCurrentBeamPosition
	yAxisCurrentBeamPositionAttrib	*y_axis_current_beam_position = new yAxisCurrentBeamPositionAttrib();
	Tango::UserDefaultAttrProp	y_axis_current_beam_position_prop;
	y_axis_current_beam_position_prop.set_unit("Pixels");
	y_axis_current_beam_position_prop.set_description("Read current beam position on y axis");
	y_axis_current_beam_position->set_default_properties(y_axis_current_beam_position_prop);
	att_list.push_back(y_axis_current_beam_position);

	//	Attribute : warningZoneXCenter
	warningZoneXCenterAttrib	*warning_zone_xcenter = new warningZoneXCenterAttrib();
	Tango::UserDefaultAttrProp	warning_zone_xcenter_prop;
	warning_zone_xcenter_prop.set_description("Define the center of warning zone on X Axis");
	warning_zone_xcenter->set_default_properties(warning_zone_xcenter_prop);
	att_list.push_back(warning_zone_xcenter);

	//	Attribute : warningZoneYCenter
	warningZoneYCenterAttrib	*warning_zone_ycenter = new warningZoneYCenterAttrib();
	Tango::UserDefaultAttrProp	warning_zone_ycenter_prop;
	warning_zone_ycenter_prop.set_description("Define the center of warning zone on Y Axis");
	warning_zone_ycenter->set_default_properties(warning_zone_ycenter_prop);
	att_list.push_back(warning_zone_ycenter);

	//	Attribute : warningZoneRadius
	warningZoneRadiusAttrib	*warning_zone_radius = new warningZoneRadiusAttrib();
	Tango::UserDefaultAttrProp	warning_zone_radius_prop;
	warning_zone_radius_prop.set_description("Define the size of the warning zone ");
	warning_zone_radius->set_default_properties(warning_zone_radius_prop);
	att_list.push_back(warning_zone_radius);

	//	Attribute : xAxisRegulationThreshold
	xAxisRegulationThresholdAttrib	*x_axis_regulation_threshold = new xAxisRegulationThresholdAttrib();
	Tango::UserDefaultAttrProp	x_axis_regulation_threshold_prop;
	x_axis_regulation_threshold_prop.set_unit("Pixels");
	x_axis_regulation_threshold_prop.set_max_value("2048");
	x_axis_regulation_threshold_prop.set_min_value("0");
	x_axis_regulation_threshold_prop.set_description("Represents the threshold of regulation activation on X axis. Under this value, no correction will be send !");
	x_axis_regulation_threshold->set_default_properties(x_axis_regulation_threshold_prop);
	x_axis_regulation_threshold->set_memorized();
	x_axis_regulation_threshold->set_memorized_init(true);
	att_list.push_back(x_axis_regulation_threshold);

	//	Attribute : yAxisRegulationThreshold
	yAxisRegulationThresholdAttrib	*y_axis_regulation_threshold = new yAxisRegulationThresholdAttrib();
	Tango::UserDefaultAttrProp	y_axis_regulation_threshold_prop;
	y_axis_regulation_threshold_prop.set_unit("Pixels");
	y_axis_regulation_threshold_prop.set_max_value("2048");
	y_axis_regulation_threshold_prop.set_min_value("0");
	y_axis_regulation_threshold_prop.set_description("Represents the threshold of regulation activation on Y axis. Under this value, no correction will be send !");
	y_axis_regulation_threshold->set_default_properties(y_axis_regulation_threshold_prop);
	y_axis_regulation_threshold->set_memorized();
	y_axis_regulation_threshold->set_memorized_init(true);
	att_list.push_back(y_axis_regulation_threshold);

	//	Attribute : fixedMode
	fixedModeAttrib	*fixed_mode = new fixedModeAttrib();
	Tango::UserDefaultAttrProp	fixed_mode_prop;
	fixed_mode_prop.set_description("If true, then Target and warning zone will be define in properties. User will not be able to change those values in runtime using attributes.(To be set using property FixMode)");
	fixed_mode->set_default_properties(fixed_mode_prop);
	att_list.push_back(fixed_mode);

	//	Attribute : thresholdedImage
	thresholdedImageAttrib	*thresholded_image = new thresholdedImageAttrib();
	Tango::UserDefaultAttrProp	thresholded_image_prop;
	thresholded_image_prop.set_description("Thresholded Image : This image will be estimate returned by the sensor plugin in use. \nNote that with LimaPlugin : a dynamic attribute will be generated (ImageThreshold) to estimate image binarisation threshold");
	thresholded_image->set_default_properties(thresholded_image_prop);
	att_list.push_back(thresholded_image);

	//	End of Automatic code generation
	//-------------------------------------------------------------
				}

//+----------------------------------------------------------------------------
//
// method : 		BeamPositionTrackingClass::get_class_property()
// 
// description : 	Read the class properties from database.
//
//-----------------------------------------------------------------------------
				void BeamPositionTrackingClass::get_class_property()
				{
	//	Initialize your default values here (if not done with  POGO).
	//------------------------------------------------------------------

	//	Read class properties from database.(Automatic code generation)
	//------------------------------------------------------------------

	//	Call database and extract values
	//--------------------------------------------
	if (Tango::Util::instance()->_UseDb==true)
		get_db_class()->get_property(cl_prop);
	Tango::DbDatum	def_prop;
	int	i = -1;


	//	End of Automatic code generation
	//------------------------------------------------------------------

				}

//+----------------------------------------------------------------------------
//
// method : 	BeamPositionTrackingClass::set_default_property
// 
// description: Set default property (class and device) for wizard.
//              For each property, add to wizard property name and description
//              If default value has been set, add it to wizard property and
//              store it in a DbDatum.
//
//-----------------------------------------------------------------------------
void BeamPositionTrackingClass::set_default_property()
{
	string	prop_name;
	string	prop_desc;
	string	prop_def;

	vector<string>	vect_data;
	//	Set Default Class Properties
	//	Set Default Device Properties
	prop_name = "DeviceTaskPeriod";
	prop_desc = "Period of the Device task";
	prop_def  = "";
	vect_data.clear();
	if (prop_def.length()>0)
	{
		Tango::DbDatum	data(prop_name);
		data << vect_data ;
		dev_def_prop.push_back(data);
		add_wiz_dev_prop(prop_name, prop_desc,  prop_def);
	}
	else
		add_wiz_dev_prop(prop_name, prop_desc);

	prop_name = "NbImgToAlign";
	prop_desc = "Number of centroid values to acquire in the asservissment loop";
	prop_def  = "";
	vect_data.clear();
	if (prop_def.length()>0)
	{
		Tango::DbDatum	data(prop_name);
		data << vect_data ;
		dev_def_prop.push_back(data);
		add_wiz_dev_prop(prop_name, prop_desc,  prop_def);
	}
	else
		add_wiz_dev_prop(prop_name, prop_desc);

	prop_name = "SensorPluginType";
	prop_desc = "Sensor plugin type to use";
	prop_def  = "";
	vect_data.clear();
	if (prop_def.length()>0)
	{
		Tango::DbDatum	data(prop_name);
		data << vect_data ;
		dev_def_prop.push_back(data);
		add_wiz_dev_prop(prop_name, prop_desc,  prop_def);
	}
	else
		add_wiz_dev_prop(prop_name, prop_desc);

	prop_name = "ActuatorSystemDeviceAdress";
	prop_desc = "ActuatorSystemDevice adress";
	prop_def  = "";
	vect_data.clear();
	if (prop_def.length()>0)
	{
		Tango::DbDatum	data(prop_name);
		data << vect_data ;
		dev_def_prop.push_back(data);
		add_wiz_dev_prop(prop_name, prop_desc,  prop_def);
	}
	else
		add_wiz_dev_prop(prop_name, prop_desc);

	prop_name = "PluginPath";
	prop_desc = "Plugin library path";
	prop_def  = "";
	vect_data.clear();
	if (prop_def.length()>0)
	{
		Tango::DbDatum	data(prop_name);
		data << vect_data ;
		dev_def_prop.push_back(data);
		add_wiz_dev_prop(prop_name, prop_desc,  prop_def);
	}
	else
		add_wiz_dev_prop(prop_name, prop_desc);

	prop_name = "CalibrationStepNbXAxis";
	prop_desc = "Number of steps to calibrate X Axis. Only works if X is set as a linear axis on ActuatorSystemDevice.";
	prop_def  = "";
	vect_data.clear();
	if (prop_def.length()>0)
	{
		Tango::DbDatum	data(prop_name);
		data << vect_data ;
		dev_def_prop.push_back(data);
		add_wiz_dev_prop(prop_name, prop_desc,  prop_def);
	}
	else
		add_wiz_dev_prop(prop_name, prop_desc);

	prop_name = "CalibrationStepNbYAxis";
	prop_desc = "Number of steps to calibrate Y Axis. Only works if Y is set as a linear axis on ActuatorSystemDevice.";
	prop_def  = "";
	vect_data.clear();
	if (prop_def.length()>0)
	{
		Tango::DbDatum	data(prop_name);
		data << vect_data ;
		dev_def_prop.push_back(data);
		add_wiz_dev_prop(prop_name, prop_desc,  prop_def);
	}
	else
		add_wiz_dev_prop(prop_name, prop_desc);

	prop_name = "XPIDDefinition";
	prop_desc = "is Pid in use, P coefficient, I , D :\nIE : True, 1, 2, 3";
	prop_def  = "";
	vect_data.clear();
	if (prop_def.length()>0)
	{
		Tango::DbDatum	data(prop_name);
		data << vect_data ;
		dev_def_prop.push_back(data);
		add_wiz_dev_prop(prop_name, prop_desc,  prop_def);
	}
	else
		add_wiz_dev_prop(prop_name, prop_desc);

	prop_name = "YPIDDefinition";
	prop_desc = "is Pid in use, P coefficient, I , D :\nIE : True, 1, 2, 3";
	prop_def  = "";
	vect_data.clear();
	if (prop_def.length()>0)
	{
		Tango::DbDatum	data(prop_name);
		data << vect_data ;
		dev_def_prop.push_back(data);
		add_wiz_dev_prop(prop_name, prop_desc,  prop_def);
	}
	else
		add_wiz_dev_prop(prop_name, prop_desc);

	prop_name = "DeviceMode";
	prop_desc = "Either : NORMAL, SIMULATED";
	prop_def  = "";
	vect_data.clear();
	if (prop_def.length()>0)
	{
		Tango::DbDatum	data(prop_name);
		data << vect_data ;
		dev_def_prop.push_back(data);
		add_wiz_dev_prop(prop_name, prop_desc,  prop_def);
	}
	else
		add_wiz_dev_prop(prop_name, prop_desc);

	prop_name = "FixedXTarget";
	prop_desc = "Used as x axis target when FixMode = true";
	prop_def  = "";
	vect_data.clear();
	if (prop_def.length()>0)
	{
		Tango::DbDatum	data(prop_name);
		data << vect_data ;
		dev_def_prop.push_back(data);
		add_wiz_dev_prop(prop_name, prop_desc,  prop_def);
	}
	else
		add_wiz_dev_prop(prop_name, prop_desc);

	prop_name = "FixedYTarget";
	prop_desc = "Used as y axis target when FixMode = true";
	prop_def  = "";
	vect_data.clear();
	if (prop_def.length()>0)
	{
		Tango::DbDatum	data(prop_name);
		data << vect_data ;
		dev_def_prop.push_back(data);
		add_wiz_dev_prop(prop_name, prop_desc,  prop_def);
	}
	else
		add_wiz_dev_prop(prop_name, prop_desc);

	prop_name = "FixedMode";
	prop_desc = "If true, then Target and warning zone will be define in properties. User will not be able to change those values in runtime using attributes...";
	prop_def  = "";
	vect_data.clear();
	if (prop_def.length()>0)
	{
		Tango::DbDatum	data(prop_name);
		data << vect_data ;
		dev_def_prop.push_back(data);
		add_wiz_dev_prop(prop_name, prop_desc,  prop_def);
	}
	else
		add_wiz_dev_prop(prop_name, prop_desc);

	prop_name = "FixedXWarningZoneCenter";
	prop_desc = "Used as x warning zone center when FixMode = true";
	prop_def  = "";
	vect_data.clear();
	if (prop_def.length()>0)
	{
		Tango::DbDatum	data(prop_name);
		data << vect_data ;
		dev_def_prop.push_back(data);
		add_wiz_dev_prop(prop_name, prop_desc,  prop_def);
	}
	else
		add_wiz_dev_prop(prop_name, prop_desc);

	prop_name = "FixedYWarningZoneCenter";
	prop_desc = "Used as y warning zone center when FixMode = true";
	prop_def  = "";
	vect_data.clear();
	if (prop_def.length()>0)
	{
		Tango::DbDatum	data(prop_name);
		data << vect_data ;
		dev_def_prop.push_back(data);
		add_wiz_dev_prop(prop_name, prop_desc,  prop_def);
	}
	else
		add_wiz_dev_prop(prop_name, prop_desc);

	prop_name = "FixedWarningZoneRadius";
	prop_desc = "Used as warning zone radius when FixMode = true";
	prop_def  = "";
	vect_data.clear();
	if (prop_def.length()>0)
	{
		Tango::DbDatum	data(prop_name);
		data << vect_data ;
		dev_def_prop.push_back(data);
		add_wiz_dev_prop(prop_name, prop_desc,  prop_def);
	}
	else
		add_wiz_dev_prop(prop_name, prop_desc);

	prop_name = "AxesAliases";
	prop_desc = "Set alias for X and Y axes here :\nx, y";
	prop_def  = "";
	vect_data.clear();
	if (prop_def.length()>0)
	{
		Tango::DbDatum	data(prop_name);
		data << vect_data ;
		dev_def_prop.push_back(data);
		add_wiz_dev_prop(prop_name, prop_desc,  prop_def);
	}
	else
		add_wiz_dev_prop(prop_name, prop_desc);

}
//+----------------------------------------------------------------------------
//
// method : 		BeamPositionTrackingClass::write_class_property
// 
// description : 	Set class description as property in database
//
//-----------------------------------------------------------------------------
void BeamPositionTrackingClass::write_class_property()
{
	//	First time, check if database used
	//--------------------------------------------
	if (Tango::Util::_UseDb == false)
		return;

	Tango::DbData	data;
	string	classname = get_name();
	string	header;
	string::size_type	start, end;

	//	Put title
	Tango::DbDatum	title("ProjectTitle");
	string	str_title("BeamPositionTracking");
	title << str_title;
	data.push_back(title);

	//	Put Description
	Tango::DbDatum	description("Description");
	vector<string>	str_desc;
	str_desc.push_back("BeamPositionTracking is a device that will be used on beamlines to enslave the beam position on a specific target. In order to move the beam centroid,");
	str_desc.push_back("BeamPositionTracking device will need to work with ActuatorSystem (please refer to AS documentation).");
	str_desc.push_back("Asservissement loop is based on a PID algorithm (on per axis).");
	str_desc.push_back("It will be able to work with differents types of axes devices, and several detectors such as Lima detector and XBPM using Yat4Tango::Plugin technology.");
	description << str_desc;
	data.push_back(description);
		
	//	put cvs or svn location
	string	filename(classname);
	filename += "Class.cpp";
	
	// Create a string with the class ID to
	// get the string into the binary
	string	class_id(ClassId);
	
	// check for cvs information
	string	src_path(CvsPath);
	start = src_path.find("/");
	if (start!=string::npos)
	{
		end   = src_path.find(filename);
		if (end>start)
		{
			string	strloc = src_path.substr(start, end-start);
			//	Check if specific repository
			start = strloc.find("/cvsroot/");
			if (start!=string::npos && start>0)
			{
				string	repository = strloc.substr(0, start);
				if (repository.find("/segfs/")!=string::npos)
					strloc = "ESRF:" + strloc.substr(start, strloc.length()-start);
			}
			Tango::DbDatum	cvs_loc("cvs_location");
			cvs_loc << strloc;
			data.push_back(cvs_loc);
		}
	}
	// check for svn information
	else
	{
		string	src_path(SvnPath);
		start = src_path.find("://");
		if (start!=string::npos)
		{
			end = src_path.find(filename);
			if (end>start)
			{
				header = "$HeadURL: ";
				start = header.length();
				string	strloc = src_path.substr(start, (end-start));
				
				Tango::DbDatum	svn_loc("svn_location");
				svn_loc << strloc;
				data.push_back(svn_loc);
			}
		}
	}

	//	Get CVS or SVN revision tag
	
	// CVS tag
	string	tagname(TagName);
	header = "$Name: ";
	start = header.length();
	string	endstr(" $");
	
	end   = tagname.find(endstr);
	if (end!=string::npos && end>start)
	{
		string	strtag = tagname.substr(start, end-start);
		Tango::DbDatum	cvs_tag("cvs_tag");
		cvs_tag << strtag;
		data.push_back(cvs_tag);
	}
	
	// SVN tag
	string	svnpath(SvnPath);
	header = "$HeadURL: ";
	start = header.length();
	
	end   = svnpath.find(endstr);
	if (end!=string::npos && end>start)
	{
		string	strloc = svnpath.substr(start, end-start);
		
		string tagstr ("/tags/");
		start = strloc.find(tagstr);
		if ( start!=string::npos )
		{
			start = start + tagstr.length();
			end   = strloc.find(filename);
			string	strtag = strloc.substr(start, end-start-1);
			
			Tango::DbDatum	svn_tag("svn_tag");
			svn_tag << strtag;
			data.push_back(svn_tag);
		}
	}

	//	Get URL location
	string	httpServ(HttpServer);
	if (httpServ.length()>0)
	{
		Tango::DbDatum	db_doc_url("doc_url");
		db_doc_url << httpServ;
		data.push_back(db_doc_url);
	}

	//  Put inheritance
	Tango::DbDatum	inher_datum("InheritedFrom");
	vector<string> inheritance;
	inheritance.push_back("Device_4Impl");
	inher_datum << inheritance;
	data.push_back(inher_datum);

	//	Call database and and values
	//--------------------------------------------
	get_db_class()->put_property(data);
}

}	// namespace
