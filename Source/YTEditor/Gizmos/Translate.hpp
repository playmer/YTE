#pragma once

#include <glm/vec3.hpp>

#include "../../YTE/Core/Component.hpp"

class YTEditorMainWindow;

namespace YTE
{
  class Translate : public Component
  {
  public:

    YTEDeclareType(Translate);

    Translate(int aDir, Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void MoveObject(glm::vec3 aDelta);

  private:
    int mDir;

  };
}

