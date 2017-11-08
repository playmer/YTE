#pragma once

#include "YTE/Core/Component.hpp"

struct lua_State;

namespace YTE
{
  class LuaComponent : public Component
  {
    public: 
      YTEDeclareType(LuaComponent);
      LuaComponent(Composition *aOwner, Space *aSpace, RSValue *aProperties);
      void Initialize() override;

      void invokeLuaFunction(const std::string& function);

      void OnLogicUpdate(LogicUpdate *aLogicUpdate);

    private:
      lua_State *mL;
      int mScriptTable;
      std::string mFileName;
  };
}