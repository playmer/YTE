#pragma once

#include "YTE/Core/Component.hpp"
#include "YTE/Lua/LuaScriptLoader.hpp"
#include "YTE/Meta/Type.hpp"

struct lua_State;

namespace YTE
{
  class LuaSystem : public Component
  {
  public:
    YTEDeclareType(LuaSystem);
    LuaSystem(Composition *aOwner, RSValue *aProperties);

    virtual void Initialize() override;
    void Update(float);
    lua_State *GetLuaState();
    int LoadLuaScript(const std::string& aFilename);
    virtual ~LuaSystem();

  private:
    lua_State *mL;
    LuaScriptLoader mScriptLoader;
  };
}

#include <iostream>

__declspec(dllexport) void PrintIndent();

__declspec(dllexport)  void Lua_PrintTable(lua_State *L, int index);

__declspec(dllexport) void Lua_PrintIndex(lua_State *L, int index);

__declspec(dllexport) void Lua_stackDump(lua_State *L);

__declspec(dllexport) void Lua_stackTrace(lua_State *L);

void LuaStackTrace(lua_State* L);

void LuaStackDump(lua_State* L);