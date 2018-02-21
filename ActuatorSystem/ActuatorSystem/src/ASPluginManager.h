//=============================================================================
// ASPluginManager.h
//=============================================================================
// abstraction.......Generic power supply (plugin manager)
// class.............ASPluginManager
// original author...THIAM
//=============================================================================

#ifndef _AS_PLUGIN_MANAGER_H_
#define _AS_PLUGIN_MANAGER_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/utils/Singleton.h>
#include <yat/plugin/PlugInManager.h>

// ============================================================================
// class: ASPluginManager
//
// Plugin manager for the GMID Device (singleton)
// ============================================================================
class ASPluginManager : public yat::Singleton<ASPluginManager>
{
  public:

    //- Loads a device by its name
    //- returns the plugin info structure & factory
    std::pair<yat::IPlugInInfo*, yat::IPlugInFactory*> load (const std::string &library_file_name);

  private:
    //- the plugin manager
    yat::PlugInManager plugin_manager;
};


#endif // _AS_PLUGIN_MANAGER_H_
