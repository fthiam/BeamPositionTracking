//=============================================================================
// BPTPluginManager.cpp
//=============================================================================
// abstraction.......Beam position tracking (plugin manager)
// class.............BPTPluginManager
// original author...THIAM
//=============================================================================
#include <BPTPluginManager.h>

// ============================================================================
// BPTPluginManager::load ()
// ============================================================================ 
std::pair<yat::IPlugInInfo*, yat::IPlugInFactory*> 
BPTPluginManager::load( const std::string &library_file_name )
{
return (plugin_manager.load(library_file_name));
}
