
#include "YTE/Core/Engine.hpp"
#include "YTE/Graphics/Camera.hpp"
#include "YTE/Physics/PhysicsSystem.hpp"

#include "YTE/Graphics/Model.hpp"

#include "YTEditor/Gizmos/Gizmo.hpp"
#include "YTEditor/Gizmos/Translate.hpp"
#include "YTEditor/Gizmos/Scale.hpp"
#include "YTEditor/Gizmos/Rotate.hpp"
#include "YTEditor/MainWindow/MainWindow.hpp"
#include "YTEditor/Physics/PhysicsHandler.hpp"
#include "YTEditor/ObjectBrowser/ObjectBrowser.hpp"
#include "YTEditor/UndoRedo/Commands.hpp"

namespace YTEditor
{

  Gizmo::Gizmo(MainWindow * aMainWindow)
    : mMainWindow(aMainWindow)
    , mMode(Select)
    , mFirstClickMousePos(glm::vec3())
    , mGizmoObj{ nullptr }
    , mActiveAxis{ nullptr }
  {
  }

  static std::string cNone = "None";

  static std::string cTranslateX{ "Move_X.fbx" };
  static std::string cTranslateY{ "Move_Y.fbx" };
  static std::string cTranslateZ{ "Move_Z.fbx" };

  static std::string cScaleX{ "Scale_X.fbx" };
  static std::string cScaleY{ "Scale_Y.fbx" };
  static std::string cScaleZ{ "Scale_Z.fbx" };

  static std::string cRotateX{ "Rotate_X.fbx" };
  static std::string cRotateY{ "Rotate_Y.fbx" };
  static std::string cRotateZ{ "Rotate_Z.fbx" };

  static std::string& TranslateMesh(Gizmo::Dir aDirection)
  {
    switch (aDirection)
    {
      case Gizmo::Dir::X:
      {
        return cTranslateX;
      }
      case Gizmo::Dir::Y:
      {
        return cTranslateY;
      }
      case Gizmo::Dir::Z:
      {
        return cTranslateZ;
      }
    }

    return cNone;
  }

  
  static std::string& ScaleMesh(Gizmo::Dir aDirection)
  {
    switch (aDirection)
    {
      case Gizmo::Dir::X:
      {
        return cScaleX;
      }
      case Gizmo::Dir::Y:
      {
        return cScaleY;
      }
      case Gizmo::Dir::Z:
      {
        return cScaleZ;
      }
    }

    return cNone;
  }
  
  static std::string& RotateMesh(Gizmo::Dir aDirection)
  {
    switch (aDirection)
    {
      case Gizmo::Dir::X:
      {
        return cRotateX;
      }
      case Gizmo::Dir::Y:
      {
        return cRotateY;
      }
      case Gizmo::Dir::Z:
      {
        return cRotateZ;
      }
    }

    return cNone;
  }

  void Gizmo::SetMode(int aMode)
  {
    YTE::Composition *currObj = mMainWindow->GetObjectBrowser().GetCurrentObject();

    if (!currObj)
    {
      aMode = Gizmo::Select;
    }

    YTE::CompositionMap &axes = *this->mGizmoObj->GetCompositions();

    auto gizmoTransform = mMainWindow->GetGizmo()->mGizmoObj->GetComponent<YTE::Transform>();

    for (auto &axis : axes)
    {
      auto model = axis.second->GetComponent<YTE::Model>();

      switch (aMode)
      {
        case Gizmo::Select:
        {
          mMode = Gizmo::Select;
          axis.second->GetComponent<YTE::Model>()->SetMesh(cNone);
          break;
        }
        case Gizmo::Translate:
        {
          mMode = Gizmo::Translate;
          auto dir = axis.second->GetComponent<YTEditor::Translate>()->GetDirection();

          model->SetMesh(TranslateMesh(dir));

          // orient the translate gizmo with the world axes
          mMainWindow->GetGizmo()->mGizmoObj->GetComponent<YTE::Transform>()->SetWorldRotation(glm::vec3());
          break;
        }
        case Gizmo::Scale:
        {
          mMode = Gizmo::Scale;

          // orient the scale gizmo with the current object axes
          glm::vec3 objRotation = currObj->GetComponent<YTE::Transform>()->GetWorldRotationAsEuler();
          auto dir = axis.second->GetComponent<YTEditor::Scale>()->GetDirection();

          model->SetMesh(ScaleMesh(dir));

          gizmoTransform->SetWorldRotation(objRotation);
          break;
        }
        case Gizmo::Rotate:
        {
          mMode = Gizmo::Rotate;

          // orient the rotate gizmo with the current object axes
          glm::vec3 objRotation = currObj->GetComponent<YTE::Transform>()->GetWorldRotationAsEuler();
          auto dir = axis.second->GetComponent<YTEditor::Rotate>()->GetDirection();

          model->SetMesh(RotateMesh(dir));

          gizmoTransform->SetWorldRotation(objRotation);
          break;
        }
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

    ObjectBrowser &objBrowser = mMainWindow->GetObjectBrowser();

    YTE::Composition *curr = objBrowser.GetCurrentObject();

    YTE::Transform *transform = curr->GetComponent<YTE::Transform>();

    switch (mMode)
    {
    case Gizmo::Select:
    {
      break;
    }

    case Gizmo::Translate:
    {
      YTE::Property *prop = transform->GetProperty("Translation", transform->GetStaticType());

      mStartValue = prop->GetGetter()->Invoke(transform);
      break;
    }

    case Gizmo::Scale:
    {
      YTE::Property *prop = transform->GetProperty("Scale", transform->GetStaticType());

      mStartValue = prop->GetGetter()->Invoke(transform);
      break;
    }

    case Gizmo::Rotate:
    {
      YTE::Property *prop = transform->GetProperty("Rotation", transform->GetStaticType());

      mStartValue = prop->GetGetter()->Invoke(transform);
      break;
    }
    }
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

  void Gizmo::OnMouseRelease(YTE::MouseButtonEvent *aEvent)
  {
    YTEUnusedArgument(aEvent);

    UndoRedo *undo = mMainWindow->GetUndoRedo();
    ObjectBrowser &objBrowser = mMainWindow->GetObjectBrowser();

    YTE::Composition *curr = objBrowser.GetCurrentObject();

    YTE::Transform *transform = curr->GetComponent<YTE::Transform>();

    YTE::Property *prop = nullptr;

    switch (mMode)
    {
    case Gizmo::Translate:
    {
      prop = transform->GetProperty("Translation", transform->GetStaticType());
      break;
    }

    case Gizmo::Scale:
    {
      prop = transform->GetProperty("Scale", transform->GetStaticType());
      break;
    }

    case Gizmo::Rotate:
    {
      prop = transform->GetProperty("Rotation", transform->GetStaticType());
      break;
    }
    }

    if (prop)
    {
      // Add command to main window undo redo
      std::string name = prop->GetName();
      YTE::Any modVal = prop->GetGetter()->Invoke(transform);

      auto cmd = std::make_unique<ChangePropValCmd>(name,
        transform->GetGUID(),
        mStartValue,
        modVal,
        mMainWindow);

      undo->InsertCommand(std::move(cmd));
    }
  }

  MainWindow* Gizmo::GetMainWindow()
  {
    return mMainWindow;
  }

  glm::vec3 Gizmo::GetFirstClickMousePos()
  {
    return mFirstClickMousePos;
  }

  void Gizmo::SelectedObjectTransformChanged(YTE::TransformChanged * aEvent)
  {
    auto transform = mGizmoObj->GetComponent<YTE::Transform>();
    glm::vec3 objPos = aEvent->WorldPosition;
    transform->SetWorldTranslation(objPos);
    
    if (mMode != Translate)
    {
      glm::quat objRot = aEvent->WorldRotation;
      transform->SetWorldRotation(objRot);
    }
  }

  void Gizmo::SnapToCurrentObject()
  {
    YTE::Composition *currObj = mMainWindow->GetObjectBrowser().GetCurrentObject();
    YTE::Transform *objTransform = currObj->GetComponent<YTE::Transform>();
    YTE::Transform *gizTransform = mGizmoObj->GetComponent<YTE::Transform>();

    if (mMode == Gizmo::Scale || mMode == Gizmo::Rotate)
    {
      glm::vec3 rot = objTransform->GetWorldRotationAsEuler();
      gizTransform->SetWorldRotation(rot);
    }

    glm::vec3 pos = objTransform->GetWorldTranslation();
    gizTransform->SetWorldTranslation(pos);

  }

  void Gizmo::RefreshAxesInPhysicsHandler()
  {
    YTE::CompositionMap *axes = mGizmoObj->GetCompositions();

    for (auto a = axes->begin(); a != axes->end(); a++)
    {
      YTE::Composition *axis = a->second.get();

      mMainWindow->GetPhysicsHandler().Remove(axis);
      mMainWindow->GetPhysicsHandler().Add(axis);
    }
  }

}
