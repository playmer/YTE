#include "YTE/Lua/Class.hpp"
#include "LuaJIT/src/luajit.h"
#include "LuaJIT/src/lauxlib.h"

namespace YTE
{
  namespace LuaBinding
  {
    Class::Class(Type *type, lua_State *L) : mL(L), mType(type)
    {
      lua_createtable(L, 3, 0);
      createClassTable(0);
      createStaticTable(1);
      createConstTable(2);
      lua_setglobal(mL, mType->GetName().c_str());
    }

    void Class::createStaticTable(int index)
    {
      lua_createtable(mL, 0, 0);
      lua_pushvalue(mL, -1);
      mStaticRef = luaL_ref(mL, LUA_REGISTRYINDEX);
      lua_rawseti(mL, -2, index);
    }
    void Class::createClassTable(int index)
    {
      lua_createtable(mL, 0, 0);
      lua_pushvalue(mL, -1);
      mClassRef = luaL_ref(mL, LUA_REGISTRYINDEX);
      lua_rawseti(mL, -2, index);
    }
    void Class::createConstTable(int index)
    {
      lua_createtable(mL, 0, 0);
      lua_pushvalue(mL, -1);
      mConstRef = luaL_ref(mL, LUA_REGISTRYINDEX);
      lua_rawseti(mL, -2, index);
    }

    int Class::getStaticTable()
    {
    }
    int Class::getClassTable()
    {
    }
    int Class::getConstTable()
    {
    }

    void Class::addMethod(Function function)
    {
    }
    void Class::addStaticFunction(Function function)
    {
    }
    void Class::addConstMethod(Function function)
    {
    }
  }
}