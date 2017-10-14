#pragma once

#include <glm/vec3.hpp>

#include "../../YTE/Core/Composition.hpp"

class YTEditorMainWindow;

class Axis;

class Gizmo
{
public:

  Gizmo(YTEditorMainWindow *aMainWindow);

  bool IsAxis(YTE::Composition *aObject);

  void SetMode(int aMode);
  int GetCurrentMode();

protected:

  YTEditorMainWindow *mMainWindow;

  YTE::Composition *mCurrObj;

  Axis *mX;
  Axis *mY;
  Axis *mZ;

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

