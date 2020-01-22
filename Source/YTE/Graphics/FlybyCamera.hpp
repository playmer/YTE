#pragma once

#include <vector>

#include "glm/glm.hpp"

#include "YTE/Core/Component.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Platform/Mouse.hpp"
#include "YTE/Platform/Keyboard.hpp"

#include "YTE/Physics/Orientation.hpp"
#include "YTE/Physics/Transform.hpp"

namespace YTE
{
  class FlybyCamera : public Component
  {
  public:
    YTEDeclareType(FlybyCamera);

    YTE_Shared FlybyCamera(Composition *aOwner, Space *aSpace);
    YTE_Shared ~FlybyCamera() override;
    YTE_Shared void Initialize() override;

    YTE_Shared void Update(LogicUpdate *aEvent);

    YTE_Shared void MousePress(MouseButtonEvent *aEvent);
    YTE_Shared void MouseMove(MouseMoveEvent *aEvent);
    YTE_Shared void MouseRelease(MouseButtonEvent *aEvent);
    YTE_Shared void KeyboardPersist(KeyboardEvent *aEvent);

  private:
    Transform *mTransform;
    Orientation *mOrientation;
    glm::vec3 mMovementDirection;
    glm::ivec2 mMouseInitialPositionInScreenCoordinates{0, 0};
    glm::ivec2 mMouseDelta;
    Window *mWindow;
    Mouse *mMouse;
    float mMovementSpeed = 10.0f;
    bool mFasterMovement = false;
    bool mMouseHeld = false;
  };
}
