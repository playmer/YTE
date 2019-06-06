#pragma once

#include <glm/vec3.hpp>

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/EventHandler.hpp"
#include "YTE/Graphics/Camera.hpp"
#include "YTE/Graphics/ImguiLayer.hpp"
#include "YTE/Physics/Transform.hpp"
#include "YTE/Platform/ForwardDeclarations.hpp"

#include "YTEditor/YTELevelEditor/ForwardDeclarations.hpp"


namespace YTEditor
{
  class Gizmo : public YTE::EventHandler
  {
  public:
    enum class Operation
    {
      Select,
      Translate,
      Rotate,
      Scale
    };

    enum class Mode
    {
      Local,
      World
    };

    Gizmo(YTELevelEditor* aLevelEditor, YTE::ImguiLayer* aLayer, YTE::Camera* aCamera);

    void Update(YTE::LogicUpdate *aEvent);
    void SnapToCurrentObject();
    void SetCurrentComposition(YTE::Composition *mComposition);

    void SetOperation(Operation aMode);
    Operation GetOperation();

    void SetMode(Mode aMode);

  protected:
    YTELevelEditor* mLevelEditor;
    YTE::Camera *mCamera;
    YTE::ImguiLayer *mLayer;
    YTE::Transform *mCurrentComposition;
    Operation mOperation;
    Mode mMode;
  };
}
