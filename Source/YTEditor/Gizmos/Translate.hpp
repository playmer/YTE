#pragma once

#include <glm/vec3.hpp>

#include "YTE/Core/Component.hpp"

class YTEditorMainWindow;

namespace YTEditor
{
  class Translate : public YTE::Component
  {
  public:

    YTEDeclareType(Translate);

    Translate(YTE::Composition *aOwner, YTE::Space *aSpace, YTE::RSValue *aProperties);

    void MoveObject(YTE::Composition *aObj, glm::vec3 aDelta);

    void SetDirection(int aDir) { mDir = aDir; }
    int GetDirection() { return mDir; }

  private:
    int mDir;

  };
}

