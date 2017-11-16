
#include "YTE/Core/Engine.hpp"
#include "YTE/Graphics/Camera.hpp"
#include "YTE/Physics/PhysicsSystem.hpp"

#include "YTE/Graphics/Model.hpp"

#include "YTEditor/Gizmos/Axis.hpp"
#include "YTEditor/Gizmos/Gizmo.hpp"
#include "YTEditor/Gizmos/Translate.hpp"
#include "YTEditor/Gizmos/Scale.hpp"
#include "YTEditor/Gizmos/Rotate.hpp"
#include "YTEditor/MainWindow/MainWindow.hpp"
#include "YTEditor/Physics/PhysicsHandler.hpp"
#include "YTEditor/ObjectBrowser/ObjectBrowser.hpp"

namespace YTEditor
{

  Gizmo::Gizmo(MainWindow * aMainWindow)
    : mMainWindow(aMainWindow), mMode(Select), mFirstClickMousePos(glm::vec3())
  {
  }

  void Gizmo::SetMode(int aMode)
  {
    YTE::CompositionMap *axes = this->mGizmoObj->GetCompositions();

    switch (aMode)
    {
    case Gizmo::Select:
    {
      mMode = Gizmo::Select;

      for (auto a = axes->begin(); a != axes->end(); ++a)
      {
        std::string mesh = "empty.fbx";

        a->second->GetComponent<YTE::Model>()->SetMesh(mesh);
      }

      break;
    }

    case Gizmo::Translate:
    {
      mMode = Gizmo::Translate;

      for (auto a = axes->begin(); a != axes->end(); ++a)
      {
        int dir = a->second->GetComponent<YTEditor::Translate>()->GetDirection();

        std::string mesh;

        switch (dir)
        {
        case Axis::Dir::X:
        {
          mesh = "Move_X.fbx";
          break;
        }

        case Axis::Dir::Y:
        {
          mesh = "Move_Y.fbx";
          break;
        }

        case Axis::Dir::Z:
        {
          mesh = "Move_Z.fbx";
          break;
        }
        }

        a->second->GetComponent<YTE::Model>()->SetMesh(mesh);

        // orient the translate gizmo with the world axes
        mMainWindow->GetGizmo()->mGizmoObj->GetComponent<YTE::Transform>()->SetWorldRotation(glm::vec3());
      }

      break;
    }

    case Gizmo::Scale:
    {
      mMode = Gizmo::Scale;

      for (auto a = axes->begin(); a != axes->end(); ++a)
      {
        int dir = a->second->GetComponent<YTEditor::Scale>()->GetDirection();

        std::string mesh;

        switch (dir)
        {
        case Axis::Dir::X:
        {
          mesh = "Scale_X.fbx";
          break;
        }

        case Axis::Dir::Y:
        {
          mesh = "Scale_Y.fbx";
          break;
        }

        case Axis::Dir::Z:
        {
          mesh = "Scale_Z.fbx";
          break;
        }
        }

        a->second->GetComponent<YTE::Model>()->SetMesh(mesh);

        // orient the scale gizmo with the current object axes
        YTE::Composition *currObj = mMainWindow->GetObjectBrowser().GetCurrentObject();
        glm::vec3 rot = currObj->GetComponent<YTE::Transform>()->GetWorldRotationAsEuler();
        mMainWindow->GetGizmo()->mGizmoObj->GetComponent<YTE::Transform>()->SetWorldRotation(rot);
      }

      break;
    }

    case Gizmo::Rotate:
    {
      mMode = Gizmo::Rotate;
      for (auto a = axes->begin(); a != axes->end(); ++a)
      {
        int dir = a->second->GetComponent<YTEditor::Scale>()->GetDirection();

        std::string mesh;

        switch (dir)
        {
        case Axis::Dir::X:
        {
          mesh = "Rotate_X.fbx";
          break;
        }

        case Axis::Dir::Y:
        {
          mesh = "Rotate_Y.fbx";
          break;
        }

        case Axis::Dir::Z:
        {
          mesh = "Rotate_Z.fbx";
          break;
        }
        }

        a->second->GetComponent<YTE::Model>()->SetMesh(mesh);
        
        // orient the rotate gizmo with the current object axes
        YTE::Composition *currObj = mMainWindow->GetObjectBrowser().GetCurrentObject();
        glm::vec3 rot = currObj->GetComponent<YTE::Transform>()->GetWorldRotationAsEuler();
        mMainWindow->GetGizmo()->mGizmoObj->GetComponent<YTE::Transform>()->SetWorldRotation(rot);
      }
      break;
    }
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

    mFirstClickMousePos = YTE::BtToOurVec3(mouseWorld);

    mPrevMousePos = mFirstClickMousePos;
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

    YTE::Composition *currObj = mMainWindow->GetObjectBrowser().GetCurrentObject();

    switch (GetCurrentMode())
    {
    case Gizmo::Select:
    {
      break;
    }

    case Gizmo::Translate:
    {
      YTEditor::Translate *translate = axis->GetComponent<YTEditor::Translate>();
      
      translate->MoveObject(currObj, realDelta);
      break;
    }

    case Gizmo::Scale:
    {
      YTEditor::Scale *scale = axis->GetComponent<YTEditor::Scale>();
      
      scale->ScaleObject(currObj, realDelta);
      break;
    }

    case Gizmo::Rotate:
    {
      // need to change to send amount object should be rotated
      YTEditor::Rotate *rotate = axis->GetComponent<YTEditor::Rotate>();

      rotate->RotateObject(currObj, mFirstClickMousePos, realDelta);

      break;
    }
    }

    mPrevMousePos = YTE::BtToOurVec3(mouseWorld);

  }

  void Gizmo::OnMouseRelease(YTE::MouseButtonEvent * aEvent)
  {
    
  }

  MainWindow* Gizmo::GetMainWindow()
  {
    return mMainWindow;
  }

  glm::vec3 Gizmo::GetFirstClickMousePos()
  {
    return mFirstClickMousePos;
  }

}
