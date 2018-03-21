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

    FlybyCamera(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    ~FlybyCamera() override;
    void Initialize() override;

    void Update(LogicUpdate *aEvent);

    void MousePress(MouseButtonEvent *aEvent);
    void MouseScroll(MouseWheelEvent *aEvent);
    void MouseMove(MouseMoveEvent *aEvent);
    void MousePersist(MouseButtonEvent *aEvent);
    void MouseRelease(MouseButtonEvent *aEvent);
    void KeyboardPersist(KeyboardEvent *aEvent);

  private:
    Transform *mTransform;
    Orientation *mOrientation;
    glm::vec3 mMovementDirection;
    glm::ivec2 mMouseInitialPosition{0, 0};
    glm::ivec2 mMouseDelta;
    Window *mWindow;
    Mouse *mMouse;
    float mMovementSpeed = 10.0f;
    bool mFasterMovement = false;
    bool mMouseHeld = false;
  };
}
