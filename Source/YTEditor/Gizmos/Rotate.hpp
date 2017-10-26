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

    void RotateObject(glm::vec3 aDelta);

    void SetDirection(int aDir) { mDir = aDir; }
    int GetDirection() { return mDir; }

  private:
    int mDir;
  };
}
