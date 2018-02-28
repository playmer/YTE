#pragma once 

#ifndef YTE_Graphics_BaseModel_hpp 
#define YTE_Graphics_BaseModel_hpp 

#include "YTE/Core/Component.hpp"

#include "YTE/Graphics/Generics/InstantiatedModel.hpp"

namespace YTE
{
  class BaseModel : public Component
  {
  public:
    YTEDeclareType(BaseModel);

    BaseModel(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    virtual InstantiatedModel* GetInstantiatedModel()
    {
      return nullptr;
    }
  };
}

#endif