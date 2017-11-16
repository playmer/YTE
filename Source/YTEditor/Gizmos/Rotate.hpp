#pragma once

#include <glm/vec3.hpp>

#include "YTE/Core/Component.hpp"


namespace YTEditor
{

  class MainWindow;

  class Rotate : public YTE::Component
  {
  public:

    YTEDeclareType(Rotate);

    Rotate(YTE::Composition *aOwner, YTE::Space *aSpace, YTE::RSValue *aProperties);

    void RotateObject(YTE::Composition *aObj, glm::vec3 aFirstMousePos, glm::vec3 aDelta);

    void SetDirection(int aDir) { mDir = aDir; }
    int GetDirection() { return mDir; }

  private:
    int mDir;
  };
}
