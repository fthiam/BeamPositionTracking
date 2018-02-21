//=============================================================================
// BPTPluginManager.h
//=============================================================================
// abstraction.......Generic power supply (plugin manager)
// class.............BPTPluginManager
// original author...THIAM
//=============================================================================

#ifndef _BA_PLUGIN_MANAGER_H_
#define _BA_PLUGIN_MANAGER_H_

// ============================================================================
// DEPENDENCIES
// ============================================================================
#include <yat/utils/Singleton.h>
#include <yat/plugin/PlugInManager.h>

// ============================================================================
// class: BPTPluginManager
//
// Plugin manager for the GMID Device (singleton)
// ============================================================================
class BPTPluginManager : public yat::Singleton<BPTPluginManager>
{
  public:

    //- Loads a device by its name
    //- returns the plugin info structure & factory
    std::pair<yat::IPlugInInfo*, yat::IPlugInFactory*> load (const std::string &library_file_name);

  private:
    //- the plugin manager
    yat::PlugInManager plugin_manager;
};


#endif // _BA_PLUGIN_MANAGER_H_
