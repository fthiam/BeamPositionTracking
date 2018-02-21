static const char *RcsId = "$Id $";
//+=============================================================================
//
// file :         ActuatorSystemStateMachine.cpp
//
// description :  C++ source for the ActuatorSystem and its alowed 
//                methods for commands and attributes
//
// project :      TANGO Device Server
//
// $Author:  $
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
// copyleft :     European Synchrotron Radiation Facility
//                BP 220, Grenoble 38043
//                FRANCE
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

/*====================================================================
 *	This file contains the methods to allow commands and attributes
 * read or write execution.
 *
 * If you wand to add your own code, add it between 
 * the "End/Re-Start of Generated Code" comments.
 *
 * If you want, you can also add your own methods.
 *====================================================================
 */

namespace ActuatorSystem_ns
{

//=================================================
//		Attributes Allowed Methods
//=================================================

//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::is_xPosition_allowed
// 
// description : 	Read/Write allowed for xPosition attribute.
//
//-----------------------------------------------------------------------------
bool ActuatorSystem::is_xPosition_allowed(Tango::AttReqType type)
{
	if (get_state() == Tango::FAULT	||
		get_state() == Tango::INIT)
	{
		//	End of Generated Code

		//	Re-Start of Generated Code
		return false;
	}
	return true;
}
//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::is_yPosition_allowed
// 
// description : 	Read/Write allowed for yPosition attribute.
//
//-----------------------------------------------------------------------------
bool ActuatorSystem::is_yPosition_allowed(Tango::AttReqType type)
{
	if (get_state() == Tango::FAULT	||
		get_state() == Tango::INIT)
	{
		//	End of Generated Code

		//	Re-Start of Generated Code
		return false;
	}
	return true;
}
//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::is_xState_allowed
// 
// description : 	Read/Write allowed for xState attribute.
//
//-----------------------------------------------------------------------------
bool ActuatorSystem::is_xState_allowed(Tango::AttReqType type)
{
		//	End of Generated Code

		//	Re-Start of Generated Code
	return true;
}
//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::is_yState_allowed
// 
// description : 	Read/Write allowed for yState attribute.
//
//-----------------------------------------------------------------------------
bool ActuatorSystem::is_yState_allowed(Tango::AttReqType type)
{
		//	End of Generated Code

		//	Re-Start of Generated Code
	return true;
}
//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::is_isXLinear_allowed
// 
// description : 	Read/Write allowed for isXLinear attribute.
//
//-----------------------------------------------------------------------------
bool ActuatorSystem::is_isXLinear_allowed(Tango::AttReqType type)
{
	if (get_state() == Tango::FAULT	||
		get_state() == Tango::INIT)
	{
		//	End of Generated Code

		//	Re-Start of Generated Code
		return false;
	}
	return true;
}
//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::is_isYLinear_allowed
// 
// description : 	Read/Write allowed for isYLinear attribute.
//
//-----------------------------------------------------------------------------
bool ActuatorSystem::is_isYLinear_allowed(Tango::AttReqType type)
{
	if (get_state() == Tango::FAULT	||
		get_state() == Tango::INIT)
	{
		//	End of Generated Code

		//	Re-Start of Generated Code
		return false;
	}
	return true;
}
//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::is_isXCalibrated_allowed
// 
// description : 	Read/Write allowed for isXCalibrated attribute.
//
//-----------------------------------------------------------------------------
bool ActuatorSystem::is_isXCalibrated_allowed(Tango::AttReqType type)
{
	if (get_state() == Tango::FAULT	||
		get_state() == Tango::INIT)
	{
		//	End of Generated Code

		//	Re-Start of Generated Code
		return false;
	}
	return true;
}
//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::is_isYCalibrated_allowed
// 
// description : 	Read/Write allowed for isYCalibrated attribute.
//
//-----------------------------------------------------------------------------
bool ActuatorSystem::is_isYCalibrated_allowed(Tango::AttReqType type)
{
	if (get_state() == Tango::FAULT	||
		get_state() == Tango::INIT)
	{
		//	End of Generated Code

		//	Re-Start of Generated Code
		return false;
	}
	return true;
}
//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::is_xLinearRatio_allowed
// 
// description : 	Read/Write allowed for xLinearRatio attribute.
//
//-----------------------------------------------------------------------------
bool ActuatorSystem::is_xLinearRatio_allowed(Tango::AttReqType type)
{
		//	End of Generated Code

		//	Re-Start of Generated Code
	return true;
}
//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::is_yLinearRatio_allowed
// 
// description : 	Read/Write allowed for yLinearRatio attribute.
//
//-----------------------------------------------------------------------------
bool ActuatorSystem::is_yLinearRatio_allowed(Tango::AttReqType type)
{
		//	End of Generated Code

		//	Re-Start of Generated Code
	return true;
}
//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::is_isSystemReady_allowed
// 
// description : 	Read/Write allowed for isSystemReady attribute.
//
//-----------------------------------------------------------------------------
bool ActuatorSystem::is_isSystemReady_allowed(Tango::AttReqType type)
{
	if (get_state() == Tango::INIT)
	{
		//	End of Generated Code

		//	Re-Start of Generated Code
		return false;
	}
	return true;
}

//=================================================
//		Commands Allowed Methods
//=================================================

//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::is_StopAxes_allowed
// 
// description : 	Execution allowed for StopAxes command.
//
//-----------------------------------------------------------------------------
bool ActuatorSystem::is_StopAxes_allowed(const CORBA::Any &any)
{
		//	End of Generated Code

		//	Re-Start of Generated Code
	return true;
}
//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::is_MoveXAxisRelative_allowed
// 
// description : 	Execution allowed for MoveXAxisRelative command.
//
//-----------------------------------------------------------------------------
bool ActuatorSystem::is_MoveXAxisRelative_allowed(const CORBA::Any &any)
{
	if (get_state() == Tango::MOVING	||
		get_state() == Tango::FAULT	||
		get_state() == Tango::INIT)
	{
		//	End of Generated Code

		//	Re-Start of Generated Code
		return false;
	}
	return true;
}
//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::is_MoveYAxisRelative_allowed
// 
// description : 	Execution allowed for MoveYAxisRelative command.
//
//-----------------------------------------------------------------------------
bool ActuatorSystem::is_MoveYAxisRelative_allowed(const CORBA::Any &any)
{
	if (get_state() == Tango::MOVING	||
		get_state() == Tango::FAULT	||
		get_state() == Tango::INIT)
	{
		//	End of Generated Code

		//	Re-Start of Generated Code
		return false;
	}
	return true;
}
//+----------------------------------------------------------------------------
//
// method : 		ActuatorSystem::is_ApplyRelativeMovementOnAxes_allowed
// 
// description : 	Execution allowed for ApplyRelativeMovementOnAxes command.
//
//-----------------------------------------------------------------------------
bool ActuatorSystem::is_ApplyRelativeMovementOnAxes_allowed(const CORBA::Any &any)
{
	if (get_state() == Tango::MOVING	||
		get_state() == Tango::FAULT	||
		get_state() == Tango::INIT	||
		get_state() == Tango::STANDBY	||
		get_state() == Tango::ALARM)
	{
		//	End of Generated Code

		//	Re-Start of Generated Code
		return false;
	}
	return true;
}

}	// namespace ActuatorSystem_ns