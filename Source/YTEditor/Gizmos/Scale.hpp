#pragma once

#include <glm/vec3.hpp>

#include "../../YTE/Core/Component.hpp"

class YTEditorMainWindow;

namespace YTE
{
  class Scale : public Component
  {
  public:

    YTEDeclareType(Scale);

    Scale(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void ScaleObject(glm::vec3 aDelta);

  private:
    int mDir;
  };
}
