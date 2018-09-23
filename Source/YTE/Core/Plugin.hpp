#pragma once

#include <string>

#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/Platform/SharedObject.hpp"

namespace YTE
{
  class Plugin
  {
    public:
    Plugin() {}

    YTE_Shared virtual ~Plugin();

    virtual void Load(Engine*) {}
    virtual void Unload() {}

    protected:
    std::unordered_map<std::string, Type*> mTypes;
    FactoryMap mFactories;
    std::string mFileName;
  };


  class PluginWrapper
  {
    using PluginNew = void* (*)();
    using PluginDelete = void(*)(void*);

    struct PluginDeleter
    {
      PluginDeleter(PluginDelete aDeleter)
        : mDeleter{ aDeleter }
      {

      }

      void operator()(Plugin* aPlugin)
      {
        mDeleter(static_cast<void*>(aPlugin));
      }

      private:
      PluginDelete mDeleter;
    };

    public:
    PluginWrapper(Engine* aEngine, std::string const& aPluginFileName);
    ~PluginWrapper();

    void Reload();

    private:
    void LoadPlugin();

    std::unique_ptr<Plugin, PluginDeleter> mPlugin;
    SharedObject mSharedObject;
    Engine* mEngine;
  };
}