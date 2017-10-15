#pragma once

#include <glm/vec3.hpp>

#include "../../YTE/Core/Composition.hpp"

class YTEditorMainWindow;

class Axis;

class Gizmo
{
public:

  Gizmo(YTEditorMainWindow *aMainWindow);

  void SetMode(int aMode);
  int GetCurrentMode();

  YTE::Composition *mGizmoObj;

protected:

  YTEditorMainWindow *mMainWindow;


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

