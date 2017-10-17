#pragma once

#include <glm/vec3.hpp>

#include "../../YTE/Core/Component.hpp"
class YTEditorMainWindow;

namespace YTE
{
  class Rotate : public Component
  {
  public:

    YTEDeclareType(Rotate);

    Rotate(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void RotateObject(glm::vec3 aDelta);

    void SetDirection(int aDir) { mDir = aDir; }
    int GetDirection() { return mDir; }

  private:
    int mDir;
  };
}
