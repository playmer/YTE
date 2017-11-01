#pragma once

#include "YTE/Meta/Function.hpp"
#include "YTE/StandardLibrary/Any.hpp"

struct lua_State;

namespace YTE
{
  namespace LuaBinding
  {
    class Class
    {
        int mStaticRef;
        int mClassRef;
        int mConstRef;

        Type *mType;
        lua_State *mL;

      public:
        Class(Type *type, lua_State *L);

        void createStaticTable(int index);
        void createClassTable(int index);
        void createConstTable(int index);

        int getStaticTable();
        int getClassTable();
        int getConstTable();

        void addMethod(Function function);
        void addStaticFunction(Function function);
        void addConstMethod(Function function);
    };
  }
}
