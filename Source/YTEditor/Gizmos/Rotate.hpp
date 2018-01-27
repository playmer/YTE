#pragma once

#include <glm/vec3.hpp>

#include "YTE/Core/Component.hpp"

#include "YTEditor/Gizmos/Gizmo.hpp"


namespace YTEditor
{

  class MainWindow;

  class Rotate : public YTE::Component
  {
  public:

    YTEDeclareType(Rotate);

    Rotate(YTE::Composition *aOwner, YTE::Space *aSpace, YTE::RSValue *aProperties);

    void RotateObject(YTE::Composition *aObj, glm::vec3 aFirstMousePos, glm::vec3 aDelta);

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
