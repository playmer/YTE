#pragma once

#include <glm/vec3.hpp>

#include "YTE/Core/Component.hpp"

#include "YTEditor/Gizmos/Gizmo.hpp"

class YTEditorMainWindow;

namespace YTEditor
{
  class Scale : public YTE::Component
  {
  public:

    YTEDeclareType(Scale);

    Scale(YTE::Composition *aOwner, YTE::Space *aSpace, YTE::RSValue *aProperties);

    void ScaleObject(YTE::Composition *aObj, glm::vec3 aDelta);

    void SetDirection(Gizmo::Dir aDir) 
    { 
      mDir = aDir; 
    }

    Gizmo::Dir GetDirection()
    { 
      return static_cast<Gizmo::Dir>(mDir);
    }

  private:
    int mDir;
  };
}
