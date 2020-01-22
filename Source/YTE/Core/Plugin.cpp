#include "YTE/Core/ComponentFactory.hpp"
#include "YTE/Core/Plugin.hpp"

#include "YTE/Platform/SharedObject.hpp"

namespace YTE
{

  Plugin::~Plugin()
  {

  }

  PluginWrapper::PluginWrapper(Engine* aEngine, std::string const& aPluginFileName)
    : mSharedObject{ aPluginFileName }
    , mPlugin{ nullptr, GenericDoNothing }
    , mEngine{ aEngine }
  {
    LoadPlugin();
  }


  PluginWrapper::~PluginWrapper()
  {
    if (mPlugin)
    {
      mPlugin->Unload();
    }

    mPlugin.reset();
  }


  void PluginWrapper::LoadPlugin()
  {
    if (mSharedObject.GetLoaded())
    {
      auto loader = mSharedObject.GetFunction<PluginNew>("PluginNew");
      auto deleter = mSharedObject.GetFunction<PluginDelete>("PluginDelete");

      auto plugin = static_cast<Plugin*>(loader());

      PluginDeleter pluginDeleter{ deleter };

      mPlugin = std::unique_ptr<Plugin, PluginDeleter>(plugin, pluginDeleter);
      mPlugin->Load(mEngine);
    }
  }

  void PluginWrapper::Reload()
  {
    mPlugin->Unload();
    mPlugin.reset();

    mSharedObject.ReloadLibrary();

    LoadPlugin();
  }

  bool PluginWrapper::IsLoaded()
  {
    return mSharedObject.GetLoaded();
  }
}
