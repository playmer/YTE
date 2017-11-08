#include "YTE/Lua/LuaComponent.hpp"
#include "LuaJIT/src/luajit.h"
#include "LuaJIT/src/lauxlib.h"

const std::string INITIALIZE_FUNCTION = "Initialize";

namespace YTE
{
  YTEDefineType(LuaComponent)
  {
    YTERegisterType(LuaComponent);
  }

  LuaComponent::LuaComponent(Composition *aOwner, Space *aSpace, RSValue *) : Component(aOwner, aSpace)
  {
  }

  void LuaComponent::Initialize()
  {
    int result = luaL_loadfile(mL, "");

    switch (result)
    {
      case LUA_ERRFILE:
      case LUA_ERRSYNTAX:
      case LUA_ERRMEM:
    }

    //Function environment
    lua_createtable(mL, 0, 1);

    //Environment metatable
    lua_createtable(mL, 0, 1);
    lua_getglobal(mL, "_G");
    lua_setfield(mL, -2, "__index");
    lua_setmetatable(mL, -2);
    lua_pushvalue(mL, -1);

    mScriptTable = luaL_ref(mL, LUA_REGISTRYINDEX);

    /* set the environment for the loaded script and execute it */
    lua_setfenv(mL, -2);
    result = lua_pcall(mL, 0, 0, 0);

    if (result)
    {
      std::string errorString = lua_tostring(mL, -1);
      lua_pop(mL, 1);
    }

    invokeLuaFunction(INITIALIZE_FUNCTION);
  }

  void LuaComponent::invokeLuaFunction(const std::string& function)
  {
    lua_rawgeti(mL, LUA_REGISTRYINDEX, mScriptTable);

    if (lua_isnil(mL, -1))
    {
      //PROBLEM!
    }

    lua_getfield(mL, -1, function.c_str());

    if (lua_isnil(mL, -1))
    {
      //Problem!
    }

    // Make sure to push args!

    // Make sure to add args and returns, as well as a good error callback so we can write out stack traces :)
    int result = lua_pcall(mL, 0, 0, 0);

    if (result)
    {
      //Problem!
    }

    lua_pop(mL, 1);

    // Pop args here
  }

  void LuaComponent::OnLogicUpdate(LogicUpdate *aLogicUpdate)
  {
  }
}