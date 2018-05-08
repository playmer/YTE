#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Graphics/FlybyCamera.hpp"
#include "YTE/Graphics/GraphicsView.hpp"

#include "YTE/Platform/Keyboard.hpp"
#include "YTE/Platform/Mouse.hpp"
#include "YTE/Platform/Window.hpp"

namespace YTE
{
  YTEDefineType(FlybyCamera)
  {
    RegisterType<FlybyCamera>();
    TypeBuilder<FlybyCamera> builder;
    GetStaticType()->AddAttribute<RunInEditor>();

    std::vector<std::vector<Type*>> deps = { { TypeId<Transform>() }, 
                                             { TypeId<Orientation>() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);

    builder.Field<&FlybyCamera::mMovementSpeed>( "MovementSpeed", PropertyBinding::GetSet)
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .SetDocumentation("The near plane the view will be rendered with.");
  }

  FlybyCamera::FlybyCamera(Composition *aOwner, 
                           Space *aSpace, 
                           RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    YTEUnusedArgument(aProperties);
  }

  FlybyCamera::~FlybyCamera()
  {
  }

  void FlybyCamera::Initialize()
  {
    auto graphicsView = mSpace->GetComponent<GraphicsView>();
    mWindow = graphicsView->GetWindow();
    auto engine = mSpace->GetEngine();

    mTransform = mOwner->GetComponent<Transform>();
    mOrientation = mOwner->GetComponent<Orientation>();

    mMouse = &mWindow->mMouse;

    mMouse->RegisterEvent<&FlybyCamera::MouseMove>(Events::MouseMove,    this);
    mMouse->RegisterEvent<&FlybyCamera::MousePress>(Events::MousePress,   this);
    mMouse->RegisterEvent<&FlybyCamera::MouseRelease>(Events::MouseRelease, this);
    mWindow->mKeyboard.RegisterEvent<&FlybyCamera::KeyboardPersist>(Events::KeyPersist, this);

    engine->RegisterEvent<&FlybyCamera::Update>(Events::LogicUpdate, this);
  }

  void FlybyCamera::Update(LogicUpdate *aEvent)
  {
    // Keyboard
    if (0 != glm::length(mMovementDirection))
    {
      mMovementDirection = glm::normalize(mMovementDirection);
    }

    auto speed = mMovementSpeed;

    if (mFasterMovement)
    {
      speed *= 10.f;
    }

    mMovementDirection = mMovementDirection 
                       * speed
                       * static_cast<float>(aEvent->Dt);

    mTransform->SetWorldTranslation(mTransform->GetWorldTranslation() + 
                                    mMovementDirection);
    mMovementDirection = glm::vec3{ 0.0f, 0.0f, 0.0f };

    mFasterMovement = false;

    // Mouse, reference: http://in2gpu.com/2016/03/14/opengl-fps-camera-quaternion/
    if (0 == mMouseDelta.x &&
        0 == mMouseDelta.y)
    {
      return;
    }

    ////notice that we reduce the sensitivity
    const float mouseX_Sensitivity = 0.0020f;
    const float mouseY_Sensitivity = 0.0020f;

    float pitch{ 0.0f };
    float yaw{ 0.0f };

    yaw = mouseX_Sensitivity * -mMouseDelta.x;
    pitch = mouseY_Sensitivity * mMouseDelta.y;

    //order matters,update camera_quat
    auto cameraRotation = mTransform->GetRotation();

    glm::quat yawRotation = AroundAxis(glm::vec3{ 0.f, 1.0f, 0.f }, yaw);
    cameraRotation = yawRotation * cameraRotation;
    cameraRotation = glm::normalize(cameraRotation);

    mTransform->SetRotation(cameraRotation);

    glm::quat pitchRotation = AroundAxis(mOrientation->GetRightVector(), pitch);
    cameraRotation = pitchRotation * cameraRotation;
    cameraRotation = glm::normalize(cameraRotation);

    mTransform->SetRotation(cameraRotation);

    mMouseDelta = glm::ivec2{ 0, 0 };
  }

  void FlybyCamera::MousePress(MouseButtonEvent *aEvent)
  {
    if (MouseButtons::Right == aEvent->Button)
    {
      mMouseInitialPosition = aEvent->WorldCoordinates + mWindow->GetPosition();
      mMouseHeld = true;
      mWindow->SetCursorVisibility(false);
    }
  }

  void FlybyCamera::MouseMove(MouseMoveEvent *aEvent)
  {
    if (mMouseHeld)
    {
      auto screenCoordinates = mWindow->GetPosition() + aEvent->WorldCoordinates;

      mMouseDelta = screenCoordinates - mMouseInitialPosition;
      mMouse->SetCursorPosition(mMouseInitialPosition);
    }
  }

  void FlybyCamera::MouseRelease(MouseButtonEvent *aEvent)
  {
    if (MouseButtons::Right == aEvent->Button)
    {
      mMouseInitialPosition = glm::ivec2{ 0, 0 };
      mMouseDelta = mMouseInitialPosition;
      mMouseHeld = false;
      mWindow->SetCursorVisibility(true);
    }
  }

  void FlybyCamera::KeyboardPersist(KeyboardEvent *aEvent)
  {
    if (false == mMouseHeld)
    {
      return;
    }

    // WASD Controls are flipped, because the camera looks down the -Z axis.
    switch (aEvent->Key)
    {
      case Keys::W:
      {
        mMovementDirection += -mOrientation->GetForwardVector();
        break;
      }
      case Keys::S:
      {
        mMovementDirection += mOrientation->GetForwardVector();
        break;
      }
      case Keys::A:
      {
        mMovementDirection += mOrientation->GetRightVector();
        break;
      }
      case Keys::Shift:
      {
        mFasterMovement = true;
        break;
      }
      case Keys::D:
      {
        mMovementDirection += -mOrientation->GetRightVector();
        break;
      }
      case Keys::E:
      case Keys::Space:
      {
        mMovementDirection.y += 1.0f;
        break;
      }
      case Keys::C:
      case Keys::Q:
      case Keys::Control:
      {
        mMovementDirection.y += -1.0f;
        break;
      }
    }
  }
}
