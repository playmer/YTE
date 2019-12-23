#pragma once

#include <string>

#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/Platform/SharedObject.hpp"

#include "YTEditor/YTEditorMeta.hpp"
#include "YTEditor/ForwardDeclarations.hpp"

namespace YTEditor
{
  class EditorPlugin
  {
    public:
    EditorPlugin() {}

    YTEditor_Shared virtual ~EditorPlugin();

    virtual void Load(YTEditorMainWindow*) {}
    virtual void Unload() {}

    protected:
    std::unordered_map<std::string, YTE::Type*> mTypes;
    YTE::FactoryMap mFactories;
    std::string mFileName;
  };


  class EditorPluginWrapper
  {
    using PluginNew = void* (*)();
    using PluginDelete = void(*)(void*);

    struct PluginDeleter
    {
      PluginDeleter(PluginDelete aDeleter)
        : mDeleter{ aDeleter }
      {

      }

      void operator()(EditorPlugin* aPlugin)
      {
        mDeleter(static_cast<void*>(aPlugin));
      }

      private:
      PluginDelete mDeleter;
    };

    public:
    YTEditor_Shared EditorPluginWrapper(YTEditorMainWindow* aMainWindow, std::string const& aPluginFileName);
    YTEditor_Shared ~EditorPluginWrapper();

    YTEditor_Shared void Reload();

    private:
    void LoadPlugin();

    std::unique_ptr<EditorPlugin, PluginDeleter> mPlugin;
    YTE::SharedObject mSharedObject;
    YTEditorMainWindow* mMainWindow;
  };
}