static const char *ClassId    = "$Id:  $";
static const char *TagName    = "$Name:  $";
static const char *CvsPath    = "$Source:  $";
static const char *SvnPath    = "$HeadURL: $";
static const char *HttpServer = "http://www.esrf.fr/computing/cs/tango/tango_doc/ds_doc/";
//+=============================================================================
//
// file :        ActuatorSystemClass.cpp
//
// description : C++ source for the ActuatorSystemClass. A singleton
//               class derived from DeviceClass. It implements the
//               command list and all properties and methods required
//               by the ActuatorSystem once per process.
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

#include <ActuatorSystem.h>
#include <ActuatorSystemClass.h>


//+----------------------------------------------------------------------------
/**
 *	Create ActuatorSystemClass singleton and return it in a C function for Python usage
 */
//+----------------------------------------------------------------------------
extern "C" {
#ifdef WIN32

__declspec(dllexport)

#endif

	Tango::DeviceClass *_create_ActuatorSystem_class(const char *name) {
		return ActuatorSystem_ns::ActuatorSystemClass::init(name);
	}
}


namespace ActuatorSystem_ns
{
//+----------------------------------------------------------------------------
//
// method : 		ApplyRelativeMovementOnAxesCmd::execute()
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
CORBA::Any *ApplyRelativeMovementOnAxesCmd::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{

	cout2 << "ApplyRelativeMovementOnAxesCmd::execute(): arrived" << endl;

	const Tango::DevVarDoubleArray	*argin;
	extract(in_any, argin);

	((static_cast<ActuatorSystem *>(device))->apply_relative_movement_on_axes(argin));
	return new CORBA::Any();
}


//+----------------------------------------------------------------------------
//
// method : 		StopAxesClass::execute()
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
CORBA::Any *StopAxesClass::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{

	cout2 << "StopAxesClass::execute(): arrived" << endl;

	((static_cast<ActuatorSystem *>(device))->stop_axes());
	return new CORBA::Any();
}

//+----------------------------------------------------------------------------
//
// method : 		MoveXAxisRelativeClass::execute()
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
CORBA::Any *MoveXAxisRelativeClass::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{

	cout2 << "MoveXAxisRelativeClass::execute(): arrived" << endl;

	Tango::DevDouble	argin;
	extract(in_any, argin);

	((static_cast<ActuatorSystem *>(device))->move_xaxis_relative(argin));
	return new CORBA::Any();
}

//+----------------------------------------------------------------------------
//
// method : 		MoveYAxisRelativeClass::execute()
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
CORBA::Any *MoveYAxisRelativeClass::execute(Tango::DeviceImpl *device,const CORBA::Any &in_any)
{

	cout2 << "MoveYAxisRelativeClass::execute(): arrived" << endl;

	Tango::DevDouble	argin;
	extract(in_any, argin);

	((static_cast<ActuatorSystem *>(device))->move_yaxis_relative(argin));
	return new CORBA::Any();
}



//
//----------------------------------------------------------------
//	Initialize pointer for singleton pattern
//----------------------------------------------------------------
//
ActuatorSystemClass *ActuatorSystemClass::_instance = NULL;

//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystemClass::ActuatorSystemClass(string &s)
// 
// description : 	constructor for the ActuatorSystemClass
//
// in : - s : The class name
//
//-----------------------------------------------------------------------------
ActuatorSystemClass::ActuatorSystemClass(string &s):DeviceClass(s)
{

	cout2 << "Entering ActuatorSystemClass constructor" << endl;
	set_default_property();
	get_class_property();
	write_class_property();
	
	cout2 << "Leaving ActuatorSystemClass constructor" << endl;

}
//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystemClass::~ActuatorSystemClass()
// 
// description : 	destructor for the ActuatorSystemClass
//
//-----------------------------------------------------------------------------
ActuatorSystemClass::~ActuatorSystemClass()
{
	_instance = NULL;
}

//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystemClass::instance
// 
// description : 	Create the object if not already done. Otherwise, just
//			return a pointer to the object
//
// in : - name : The class name
//
//-----------------------------------------------------------------------------
ActuatorSystemClass *ActuatorSystemClass::init(const char *name)
{
	if (_instance == NULL)
	{
		try
		{
			string s(name);
			_instance = new ActuatorSystemClass(s);
		}
		catch (bad_alloc)
		{
			throw;
		}		
	}		
	return _instance;
}

ActuatorSystemClass *ActuatorSystemClass::instance()
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
// method : 		ActuatorSystemClass::command_factory
// 
// description : 	Create the command object(s) and store them in the 
//			command list
//
//-----------------------------------------------------------------------------
void ActuatorSystemClass::command_factory()
{
	command_list.push_back(new StopAxesClass("StopAxes",
		Tango::DEV_VOID, Tango::DEV_VOID,
		"",
		"",
		Tango::OPERATOR));
	command_list.push_back(new MoveXAxisRelativeClass("MoveXAxisRelative",
		Tango::DEV_DOUBLE, Tango::DEV_VOID,
		"",
		"",
		Tango::OPERATOR));
	command_list.push_back(new MoveYAxisRelativeClass("MoveYAxisRelative",
		Tango::DEV_DOUBLE, Tango::DEV_VOID,
		"",
		"",
		Tango::OPERATOR));
	command_list.push_back(new ApplyRelativeMovementOnAxesCmd("ApplyRelativeMovementOnAxes",
		Tango::DEVVAR_DOUBLEARRAY, Tango::DEV_VOID,
		"",
		"",
		Tango::OPERATOR));

	//	add polling if any
	for (unsigned int i=0 ; i<command_list.size(); i++)
	{
	}
}

//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystemClass::get_class_property
// 
// description : 	Get the class property for specified name.
//
// in :		string	name : The property name
//
//+----------------------------------------------------------------------------
Tango::DbDatum ActuatorSystemClass::get_class_property(string &prop_name)
{
	for (unsigned int i=0 ; i<cl_prop.size() ; i++)
		if (cl_prop[i].name == prop_name)
			return cl_prop[i];
	//	if not found, return  an empty DbDatum
	return Tango::DbDatum(prop_name);
}
//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystemClass::get_default_device_property()
// 
// description : 	Return the default value for device property.
//
//-----------------------------------------------------------------------------
Tango::DbDatum ActuatorSystemClass::get_default_device_property(string &prop_name)
{
	for (unsigned int i=0 ; i<dev_def_prop.size() ; i++)
		if (dev_def_prop[i].name == prop_name)
			return dev_def_prop[i];
	//	if not found, return  an empty DbDatum
	return Tango::DbDatum(prop_name);
}

//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystemClass::get_default_class_property()
// 
// description : 	Return the default value for class property.
//
//-----------------------------------------------------------------------------
Tango::DbDatum ActuatorSystemClass::get_default_class_property(string &prop_name)
{
	for (unsigned int i=0 ; i<cl_def_prop.size() ; i++)
		if (cl_def_prop[i].name == prop_name)
			return cl_def_prop[i];
	//	if not found, return  an empty DbDatum
	return Tango::DbDatum(prop_name);
}
//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystemClass::device_factory
// 
// description : 	Create the device object(s) and store them in the 
//			device list
//
// in :		Tango::DevVarStringArray *devlist_ptr : The device name list
//
//-----------------------------------------------------------------------------
void ActuatorSystemClass::device_factory(const Tango::DevVarStringArray *devlist_ptr)
{

	//	Create all devices.(Automatic code generation)
	//-------------------------------------------------------------
	for (unsigned long i=0 ; i < devlist_ptr->length() ; i++)
	{
		cout4 << "Device name : " << (*devlist_ptr)[i].in() << endl;
						
		// Create devices and add it into the device list
		//----------------------------------------------------
		device_list.push_back(new ActuatorSystem(this, (*devlist_ptr)[i]));							 

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
//	Method: ActuatorSystemClass::attribute_factory(vector<Tango::Attr *> &att_list)
//-----------------------------------------------------------------------------
void ActuatorSystemClass::attribute_factory(vector<Tango::Attr *> &att_list)
{
	//	Attribute : xPosition
	xPositionAttrib	*x_position = new xPositionAttrib();
	att_list.push_back(x_position);

	//	Attribute : yPosition
	yPositionAttrib	*y_position = new yPositionAttrib();
	att_list.push_back(y_position);

	//	Attribute : xState
	xStateAttrib	*x_state = new xStateAttrib();
	att_list.push_back(x_state);

	//	Attribute : yState
	yStateAttrib	*y_state = new yStateAttrib();
	att_list.push_back(y_state);

	//	Attribute : isXLinear
	isXLinearAttrib	*is_xlinear = new isXLinearAttrib();
	att_list.push_back(is_xlinear);

	//	Attribute : isYLinear
	isYLinearAttrib	*is_ylinear = new isYLinearAttrib();
	att_list.push_back(is_ylinear);

	//	Attribute : isXCalibrated
	isXCalibratedAttrib	*is_xcalibrated = new isXCalibratedAttrib();
	att_list.push_back(is_xcalibrated);

	//	Attribute : isYCalibrated
	isYCalibratedAttrib	*is_ycalibrated = new isYCalibratedAttrib();
	att_list.push_back(is_ycalibrated);

	//	Attribute : xLinearRatio
	xLinearRatioAttrib	*x_linear_ratio = new xLinearRatioAttrib();
	Tango::UserDefaultAttrProp	x_linear_ratio_prop;
	x_linear_ratio_prop.set_unit(" ");
	x_linear_ratio->set_default_properties(x_linear_ratio_prop);
	x_linear_ratio->set_memorized();
	x_linear_ratio->set_memorized_init(true);
	att_list.push_back(x_linear_ratio);

	//	Attribute : yLinearRatio
	yLinearRatioAttrib	*y_linear_ratio = new yLinearRatioAttrib();
	Tango::UserDefaultAttrProp	y_linear_ratio_prop;
	y_linear_ratio_prop.set_unit(" ");
	y_linear_ratio->set_default_properties(y_linear_ratio_prop);
	y_linear_ratio->set_memorized();
	y_linear_ratio->set_memorized_init(true);
	att_list.push_back(y_linear_ratio);

	//	Attribute : isSystemReady
	isSystemReadyAttrib	*is_system_ready = new isSystemReadyAttrib();
	att_list.push_back(is_system_ready);

	//	End of Automatic code generation
	//-------------------------------------------------------------
}

//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystemClass::get_class_property()
// 
// description : 	Read the class properties from database.
//
//-----------------------------------------------------------------------------
void ActuatorSystemClass::get_class_property()
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
// method : 	ActuatorSystemClass::set_default_property
// 
// description: Set default property (class and device) for wizard.
//              For each property, add to wizard property name and description
//              If default value has been set, add it to wizard property and
//              store it in a DbDatum.
//
//-----------------------------------------------------------------------------
void ActuatorSystemClass::set_default_property()
{
	string	prop_name;
	string	prop_desc;
	string	prop_def;

	vector<string>	vect_data;
	//	Set Default Class Properties
	//	Set Default Device Properties
	prop_name = "XAxisPluginPath";
	prop_desc = "";
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

	prop_name = "XAxisPluginType";
	prop_desc = "";
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

	prop_name = "YAxisPluginType";
	prop_desc = "";
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

	prop_name = "YAxisPluginPath";
	prop_desc = "";
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

	prop_name = "XAxisDeviceAdress";
	prop_desc = "";
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

	prop_name = "YAxisDeviceAdress";
	prop_desc = "";
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

	prop_name = "XAxisMaxPosition";
	prop_desc = "Set maximum position of X axis, in axis position's unit...";
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

	prop_name = "XAxisMinPosition";
	prop_desc = "Set minimum position of X axis, in axis position's unit...";
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

	prop_name = "YAxisMaxPosition";
	prop_desc = "Set maximum position of Y axis, in axis position's unit...";
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

	prop_name = "YAxisMinPosition";
	prop_desc = "Set minimum position of Y axis, in axis position's unit...";
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
	prop_desc = "Either : NORMAL, SIMULATED\nIn simulated mode, this device will only read state/status and positions from axes, it will not send any cmds....";
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
// method : 		ActuatorSystemClass::write_class_property
// 
// description : 	Set class description as property in database
//
//-----------------------------------------------------------------------------
void ActuatorSystemClass::write_class_property()
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
	string	str_title("ActuatorSystem");
	title << str_title;
	data.push_back(title);

	//	Put Description
	Tango::DbDatum	description("Description");
	vector<string>	str_desc;
	str_desc.push_back("This device should be used for BeamPositionTracking application only.");
	str_desc.push_back("This device's aim is to organize translations (X and Y axis).");
	str_desc.push_back("To use a specific translation (or a rotation) on this device, make sure that a plugin exists already...");
	str_desc.push_back("Will also apply ratios (calibration results) on those axes.");
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
