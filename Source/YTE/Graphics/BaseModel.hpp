#pragma once 

#ifndef YTE_Graphics_BaseModel_hpp 
#define YTE_Graphics_BaseModel_hpp 

#include "YTE/Core/Component.hpp"
#include <vector>

#include "YTE/Graphics/Generics/InstantiatedModel.hpp"

namespace YTE
{
  YTEDeclareEvent(UpdateVisibilityEvent);

  class UpdateVisibilityEvent : public Event
  {
  public:
    YTEDeclareType(UpdateVisibilityEvent);

    UpdateVisibilityEvent(bool aVisibility) { Visibility = aVisibility; }

    bool Visibility;
  };

  class BaseModel : public Component
  {
  public:
    YTEDeclareType(BaseModel);

    BaseModel(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    virtual std::vector<InstantiatedModel*> GetInstantiatedModel()
    {
      return { };
    }

    void SetVisibility(bool aVisibility)
    {
      auto models = GetInstantiatedModel();

      for (auto &model : models)
      {
        model->SetVisibility(aVisibility);
      }

      mVisibility = aVisibility;
    }

    bool GetVisibility()
    {
      auto models = GetInstantiatedModel();

      for (auto &model : models)
      {
        if (false == model->GetVisibility())
        {
          return false;
        }
      }

      return true;
    }

  protected:

    bool mVisibility = true;
  };
}

#endif