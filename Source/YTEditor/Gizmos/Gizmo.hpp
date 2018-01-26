#pragma once

#include <glm/vec3.hpp>

#include "YTE/Core/EventHandler.hpp"

#include "YTE/Core/Composition.hpp"
#include "YTE/Platform/ForwardDeclarations.hpp"

namespace YTEditor
{
  class MainWindow;
  class Axis;

  class Gizmo : public YTE::EventHandler
  {
  public:

    Gizmo(MainWindow *aMainWindow);

    void SetMode(int aMode);
    int GetCurrentMode();

    void SetRenderingWindow(YTE::Window *aWindow);

    void OnMousePressed(YTE::MouseButtonEvent *aEvent, YTE::Space *aSpace, YTE::Composition *aAxis, float aPickedDistance);

    void OnMousePersist(YTE::MouseButtonEvent *aEvent, YTE::Space *aSpace, float aPickedDistance);
    
    void OnMouseRelease(YTE::MouseButtonEvent *aEvent);

    YTE::Composition *mGizmoObj;

    YTE::Composition *mActiveAxis;

    MainWindow* GetMainWindow();

    glm::vec3 GetFirstClickMousePos();

    void SelectedObjectTransformChanged(YTE::TransformChanged *aEvent);

    // hack fix for bug
    void RefreshAxesInPhysicsHandler();

  protected:

    MainWindow *mMainWindow;
    YTE::Window *mWindow;

    glm::vec3 mPrevMousePos;

    glm::vec3 mFirstClickMousePos;

    int mMode;

  public:
    enum Mode
    {
      Select,
      Translate,
      Rotate,
      Scale
    };

    enum Dir
    {
      X,
      Y,
      Z
    };

  };

}
