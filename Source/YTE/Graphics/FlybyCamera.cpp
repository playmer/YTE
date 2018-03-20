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
    YTERegisterType(FlybyCamera);
    GetStaticType()->AddAttribute<RunInEditor>();

    std::vector<std::vector<Type*>> deps = { { TypeId<Transform>() }, 
                                             { TypeId<Orientation>() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);
  }

  FlybyCamera::FlybyCamera(Composition *aOwner, 
                           Space *aSpace, 
                           RSValue *aProperties)
    : Component(aOwner, aSpace)
  {

  }

  FlybyCamera::~FlybyCamera()
  {
  }

  void FlybyCamera::Initialize()
  {
    auto graphicsView = mSpace->GetComponent<GraphicsView>();
    auto window = graphicsView->GetWindow();
    auto engine = mSpace->GetEngine();

    mTransform = mOwner->GetComponent<Transform>();
    mOrientation = mOwner->GetComponent<Orientation>();

    window->mMouse.YTERegister(Events::MouseMove,    this, &FlybyCamera::MouseMove);
    window->mMouse.YTERegister(Events::MouseScroll,  this, &FlybyCamera::MouseScroll);
    window->mMouse.YTERegister(Events::MousePress,   this, &FlybyCamera::MousePress);
    window->mMouse.YTERegister(Events::MouseRelease, this, &FlybyCamera::MouseRelease);
    window->mMouse.YTERegister(Events::MousePersist, this, &FlybyCamera::MousePersist);
    window->mKeyboard.YTERegister(Events::KeyPersist, this, &FlybyCamera::KeyboardPersist);

    engine->YTERegister(Events::LogicUpdate, this, &FlybyCamera::Update);
  }

  void FlybyCamera::Update(LogicUpdate *aEvent)
  {
    // Keyboard
    if (0 != glm::length(mMovementDirection))
    {
      mMovementDirection = glm::normalize(mMovementDirection);
    }

    mMovementDirection = mMovementDirection 
                       * mMovementSpeed
                       * static_cast<float>(aEvent->Dt);

    mTransform->SetWorldTranslation(mTransform->GetWorldTranslation() + 
                                    mMovementDirection);
    mMovementDirection = glm::vec3{ 0.0f, 0.0f, 0.0f };

    std::cout << fmt::format("Move: x: {}, y: {}, z: {}, \n",
                             mTransform->GetWorldTranslation().x,
                             mTransform->GetWorldTranslation().y,
                             mTransform->GetWorldTranslation().z);
    // Mouse
  }

  void FlybyCamera::MousePress(MouseButtonEvent *aEvent)
  {

  }

  void FlybyCamera::MouseScroll(MouseWheelEvent *aEvent)
  {

  }

  void FlybyCamera::MouseMove(MouseMoveEvent *aEvent)
  {

  }

  void FlybyCamera::MousePersist(MouseButtonEvent *aEvent)
  {

  }

  void FlybyCamera::MouseRelease(MouseButtonEvent *aEvent)
  {

  }

  void FlybyCamera::KeyboardPersist(KeyboardEvent *aEvent)
  {
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
      case Keys::Q:
      case Keys::Control:
      {
        mMovementDirection.y += -1.0f;
        break;
      }
    }
  }
}
