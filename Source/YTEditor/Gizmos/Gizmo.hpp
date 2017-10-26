#pragma once

#include <glm/vec3.hpp>

#include "YTE/Core/Composition.hpp"

namespace YTEditor
{

  class MainWindow;

  class Axis;

  class Gizmo
  {
  public:

    Gizmo(MainWindow *aMainWindow);

    void SetMode(int aMode);
    int GetCurrentMode();

    YTE::Composition *mGizmoObj;

  protected:

    MainWindow *mMainWindow;


    int mMode;

  public:
    enum Mode
    {
      Select,
      Translate,
      Scale,
      Rotate
    };

  };

}
