#include <glm/gtc/type_ptr.hpp>

#include "YTE/Core/Engine.hpp"
#include "YTE/Graphics/Camera.hpp"
#include "YTE/Physics/PhysicsSystem.hpp"
#include "YTE/Meta/Type.hpp"

#include "YTE/Graphics/Model.hpp"

#include "YTEditor/Gizmos/Gizmo.hpp"
#include "YTEditor/MainWindow/MainWindow.hpp"
#include "YTEditor/Physics/PhysicsHandler.hpp"
#include "YTEditor/ObjectBrowser/ObjectBrowser.hpp"
#include "YTEditor/UndoRedo/Commands.hpp"

namespace YTEditor
{

  Gizmo::Gizmo(MainWindow *aMainWindow, YTE::ImguiLayer *aLayer, YTE::Camera *aCamera)
    : mMainWindow{ aMainWindow }
    , mCamera{ aCamera }
    , mLayer{aLayer}
    , mOperation{ Operation::Select }
  {
    aMainWindow->GetRunningEngine()->YTERegister(YTE::Events::FrameUpdate, this, &Gizmo::Update);
  }

  void Gizmo::Update(YTE::LogicUpdate *aEvent)
  {
    if ((Operation::Select != mOperation) && (nullptr != mCurrentComposition))
    {
      ImGuiIO& io = mLayer->GetIO();
      mLayer->SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

      auto cameraUBO = mCamera->ConstructUBOView();
      auto view = glm::value_ptr(cameraUBO.mViewMatrix);
      auto projection = glm::value_ptr(cameraUBO.mProjectionMatrix);

      auto compositionMatrix = mCurrentComposition->GetTransformMatrix();
      auto matrix = glm::value_ptr(compositionMatrix);

      ImGuizmo::OPERATION operation;

      switch (mOperation)
      {
        case Operation::Translate: operation = ImGuizmo::OPERATION::TRANSLATE; break;
        case Operation::Scale: operation = ImGuizmo::OPERATION::SCALE; break;
        case Operation::Rotate: operation = ImGuizmo::OPERATION::ROTATE; break;
      }

      ImGuizmo::MODE mode = ImGuizmo::MODE::WORLD;

      //mLayer->SetDrawlist();
      //
      //mLayer->Manipulate(view,          // const float *view, 
      //                   projection,    // const float *projection, 
      //                   operation,      // OPERATION operation, 
      //                   mode,          // MODE mode, 
      //                   matrix,        // float *matrix, 
      //                   nullptr,       // float *deltaMatrix = 0, 
      //                   nullptr,       // float *snap = 0, 
      //                   nullptr,       // float *localBounds = NULL, 
      //                   nullptr);      // float *boundsSnap = NULL
    }
  }

  void Gizmo::SnapToCurrentObject()
  {
    SetCurrentComposition(mMainWindow->GetObjectBrowser().GetCurrentObject());
  }

  void Gizmo::SetCurrentComposition(YTE::Composition *aComposition)
  {
    if (nullptr != aComposition)
    {
      mCurrentComposition = aComposition->GetComponent<YTE::Transform>();
    }
  }

  void Gizmo::SetOperation(Operation aOperation)
  {
    mOperation = aOperation;
  }

  Gizmo::Operation Gizmo::GetOperation()
  {
    return mOperation;
  }
}
