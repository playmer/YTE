#include "YTE/Core/ComponentFactory.hpp"

#include "YTE/Platform/SharedObject.hpp"

#include "YTEditor/EditorPlugin.hpp"

namespace YTEditor
{
  EditorPlugin::~EditorPlugin()
  {

  }

  EditorPluginWrapper::EditorPluginWrapper(YTEditorMainWindow* aMainWindow, std::string const& aPluginFileName)
    : mSharedObject{ aPluginFileName }
    , mPlugin{ nullptr, YTE::GenericDoNothing }
    , mMainWindow{ aMainWindow }
  {
    LoadPlugin();
  }


  EditorPluginWrapper::~EditorPluginWrapper()
  {
    mPlugin->Unload();
    mPlugin.reset();
  }


  void EditorPluginWrapper::LoadPlugin()
  {
    if (mSharedObject.GetLoaded())
    {
      auto loader = mSharedObject.GetFunction<PluginNew>("PluginNew");
      auto deleter = mSharedObject.GetFunction<PluginDelete>("PluginDelete");

      auto plugin = static_cast<EditorPlugin*>(loader());

      PluginDeleter pluginDeleter{ deleter };

      mPlugin = std::unique_ptr<EditorPlugin, PluginDeleter>(plugin, pluginDeleter);
      mPlugin->Load(mMainWindow);
    }
  }

  void EditorPluginWrapper::Reload()
  {
    mPlugin->Unload();
    mPlugin.reset();

    mSharedObject.ReloadLibrary();

    LoadPlugin();
  }
  
  bool EditorPluginWrapper::IsLoaded()
  {
    return mSharedObject.GetLoaded();
  }
}
