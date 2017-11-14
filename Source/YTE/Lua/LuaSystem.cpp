#include "YTE/Lua/LuaSystem.hpp"

#include "YTE/Lua/LuaLibraryInclude.hpp"

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