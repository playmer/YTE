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

  private:
    int mDir;
  };
}
