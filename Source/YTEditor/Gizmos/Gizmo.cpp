
#include "YTE/Core/Engine.hpp"
#include "YTE/Graphics/Camera.hpp"
#include "YTE/Physics/PhysicsSystem.hpp"


#include "YTEditor/Gizmos/Axis.hpp"
#include "YTEditor/Gizmos/Gizmo.hpp"
#include "YTEditor/Gizmos/Translate.hpp"
#include "YTEditor/Gizmos/Scale.hpp"
#include "YTEditor/Gizmos/Rotate.hpp"
#include "YTEditor/MainWindow/MainWindow.hpp"
#include "YTEditor/Physics/PhysicsHandler.hpp"

namespace YTEditor
{

  Gizmo::Gizmo(MainWindow * aMainWindow) : mMainWindow(aMainWindow), mMode(Scale)
  {
  }

  void Gizmo::SetMode(int aMode)
  {
    if (Select <= aMode && aMode <= Rotate)
    {
      mMode = aMode;
    }
  }

  int Gizmo::GetCurrentMode()
  {
    return mMode;
  }

  void Gizmo::SetRenderingWindow(YTE::Window * aWindow)
  {
    mWindow = aWindow;
  }

  void Gizmo::OnMousePressed(YTE::MouseButtonEvent *aEvent, YTE::Space *aSpace, YTE::Composition *aAxis, float aPickedDistance)
  {
    mActiveAxis = aAxis;

    // calculate and store the previous mouse position in world coordinates
    glm::i32vec2 pos = aEvent->Mouse->GetCursorPosition();
    glm::vec4 pos4 = glm::vec4(pos.x, pos.y, 0.0, 1.0);

    // convert the two positions to world coordinates
    auto view = aSpace->GetComponent<YTE::GraphicsView>();
    auto camera = view->GetLastCamera();
    YTE::UBOView uboView = camera->ConstructUBOView();

    auto camPos = camera->GetOwner()->GetComponent<YTE::Transform>()->GetWorldTranslation();

    auto btCamPos = YTE::OurVec3ToBt(camPos);

    auto mouseWorld = YTEditor::getRayTo(uboView, btCamPos, aEvent->WorldCoordinates, mWindow->GetWidth(), mWindow->GetHeight(), aPickedDistance);

    mPrevMousePos = YTE::BtToOurVec3(mouseWorld);
  }

  void Gizmo::OnMousePersist(YTE::MouseButtonEvent *aEvent, YTE::Space *aSpace, float aPickedDistance)
  {
    // get the change in position this frame for the mouse
    glm::i32vec2 pos = aEvent->Mouse->GetCursorPosition();
    glm::vec4 pos4 = glm::vec4(pos.x, pos.y, 0.0, 1.0);

    // convert the two positions to world coordinates
    auto view = aSpace->GetComponent<YTE::GraphicsView>();
    auto camera = view->GetLastCamera();
    YTE::UBOView uboView = camera->ConstructUBOView();

    auto camPos = camera->GetOwner()->GetComponent<YTE::Transform>()->GetWorldTranslation();

    auto btCamPos = YTE::OurVec3ToBt(camPos);

    auto mouseWorld = YTEditor::getRayTo(uboView, 
                                                btCamPos, 
                                                aEvent->WorldCoordinates, 
                                                mWindow->GetWidth(), 
                                                mWindow->GetHeight(), 
                                                aPickedDistance);

    auto realDelta = YTE::BtToOurVec3(mouseWorld) - mPrevMousePos;

    YTE::Composition *axis = mGizmoObj->FindFirstCompositionByName(mActiveAxis->GetName());

    switch (GetCurrentMode())
    {
    case Gizmo::Select:
    {
      break;
    }

    case Gizmo::Translate:
    {
      YTEditor::Translate *translate = axis->GetComponent<YTEditor::Translate>();
      
      translate->MoveObject(realDelta);
      break;
    }

    case Gizmo::Scale:
    {
      YTEditor::Scale *scale = axis->GetComponent<YTEditor::Scale>();
      
      scale->ScaleObject(realDelta);
      break;
    }

    case Gizmo::Rotate:
    {
      // need to change to send amount object should be rotated
      YTEditor::Rotate *rotate = axis->GetComponent<YTEditor::Rotate>();
      
      rotate->RotateObject(realDelta);
      break;
    }
    }

    mPrevMousePos = YTE::BtToOurVec3(mouseWorld);

  }

  void Gizmo::OnMouseRelease(YTE::MouseButtonEvent * aEvent)
  {
    
  }

}
