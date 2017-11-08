#pragma once

#include "YTE/Core/Component.hpp"

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
    virtual ~LuaSystem();

  private:
    lua_State *mL;
  };
}