//=============================================================================
// ASPluginManager.cpp
//=============================================================================
// abstraction.......Actuator System (plugin manager)
// class.............ASPluginManager
// original author...THIAM
//=============================================================================
#include <ASPluginManager.h>

// ============================================================================
// ASPluginManager::load ()
// ============================================================================ 
std::pair<yat::IPlugInInfo*, yat::IPlugInFactory*> 
ASPluginManager::load( const std::string &library_file_name )
{
return (plugin_manager.load(library_file_name));
}
