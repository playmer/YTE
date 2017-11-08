#include "YTE/Lua/LuaSystem.hpp"

#include "LuaJIT/src/luajit.h"
#include "LuaJIT/src/lauxlib.h"
#include "LuaJIT/src/lualib.h"

namespace YTE
{
  YTEDefineType(LuaSystem)
  {
    YTERegisterType(LuaSystem);
  }

  LuaSystem::LuaSystem(Composition *aOwner, RSValue *aProperties) : Component(aOwner, nullptr)
  {
  }

  void LuaSystem::Initialize()
  {
    mL = luaL_newstate();
    luaL_openlibs(mL);
  }
  void LuaSystem::Update(float)
  {
  }
  LuaSystem::~LuaSystem()
  {
  }
}