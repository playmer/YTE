#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

#include "YTE/Core/Engine.hpp" 
#include "YTE/Core/Space.hpp" 
#include "YTE/Core/Utilities.hpp" 
 
#include "YTE/Graphics/Camera.hpp" 
#include "YTE/Graphics/GraphicsView.hpp" 
#include "YTE/Graphics/GraphicsSystem.hpp" 
#include "YTE/Graphics/UBOs.hpp"
 
#include "YTE/Physics/Orientation.hpp"
#include "YTE/Physics/Transform.hpp"


/*
 *  Transform
 *                ^
 *                |
 *             Y  |
 *                |
 *                |
 *                |       X
 *                /---------------->
 *               /
 *        Z     /
 *             /
 *            V
 *
 *
 *  Rotation
 *                ^
 *                |
 *          Y    <|-
 *              | | |
 *               -|-           ^
 *                |       X    |
 *                /---------------->
 *            |--/--           |
 *        Z   | /
 *            -/--->
 *            V
 *  
 *    Roll is on Z axis and rotates the cameras view
 *    Pitch is the X axis and rotates the view up and down
 *    Yaw is the Y axis and rotates the view right and left
 *    
 *  Mouse Position Deltas
 *  
 *             ^    ^  -Y
 *              \   |
 *            Z  \  |
 *                \ |
 *      -X         \|
 *      <------------------------->  X
 *                  |\
 *                  | \
 *                  |  \  -Z
 *                  |   \
 *                  V    V
 *                  
 *                  Y
 *                  
 *    Scrolling is the Z axis, -Z is scroll out (toward your body), Z is scroll in (towards screen)
 */

 // NOTE: Spin = x
 //       Tilt = y
 //       Twist = z

namespace YTE 
{ 
  static std::vector<std::string> PopulateDropDownList(Component *aComponent) 
  { 
    YTEUnusedArgument(aComponent);
    return { "TargetObject", "TargetPoint", "CameraOrientation", "Flyby", "Gameplay" }; 
  }

  YTEDefineType(Camera) 
  { 
    YTERegisterType(Camera);
    GetStaticType()->AddAttribute<RunInEditor>();

    std::vector<std::vector<Type*>> deps = { { Transform::GetStaticType() }, 
                                             { Orientation::GetStaticType() } };

    Camera::GetStaticType()->AddAttribute<ComponentDependencies>(deps);
 
    YTEBindProperty(&Camera::GetTargetObject, &Camera::SetTargetObject, "TargetObject") 
      .SetDocumentation("Object the camera will point at if it's type is set to \"TargetObject\"."); 
 
    YTEBindProperty(&Camera::GetTargetPoint, &Camera::SetTargetPoint, "TargetPoint") 
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .SetDocumentation("Point the camera will point at if it's type is set to \"TargetPoint\"."); 
 
    YTEBindProperty(&Camera::GetFarPlane, &Camera::SetFarPlane, "FarPlane") 
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .SetDocumentation("The far plane the view will be rendered with."); 
 
    YTEBindProperty(&Camera::GetNearPlane, &Camera::SetNearPlane, "NearPlane") 
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .SetDocumentation("The near plane the view will be rendered with."); 
 
    YTEBindProperty(&Camera::GetFieldOfViewY, &Camera::SetFieldOfViewY, "FieldOfViewY") 
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .SetDocumentation("The field of view (y) the view will be rendered with."); 

    YTEBindProperty(&Camera::GetZoomingMaxAndMin, &Camera::SetZoomingMaxAndMin, "Zoom Planes")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .SetDocumentation("The min (x) and max (y) zoom of the camera.");
 
    YTEBindProperty(&Camera::GetCameraType, &Camera::SetCameraType, "CameraType") 
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<DropDownStrings>(PopulateDropDownList) 
      .SetDocumentation("The type of camera we'd like to have. Options include:\n" 
                        " - TargetObject: The camera will always face the chosen target object.\n" 
                        " - TargetPoint: The camera will always face the chosen target point.\n" 
                        " - CameraOrientation: The camera will always face in the direction the orientation component suggests.\n"
                        " - Flyby: The camera behaves like an FPS, where it uses WASD.\n"
                        " - Gameplay: The camera for the game.\n");
    
    YTEBindProperty(&Camera::GetPanSpeed, &Camera::SetPanSpeed, "Pan Speed")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .SetDocumentation("Adjusts the pan speed of the camera");

    YTEBindProperty(&Camera::GetScrollSpeed, &Camera::SetScrollSpeed, "Scroll Speed")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .SetDocumentation("Adjusts the Scroll speed of the camera");

    YTEBindProperty(&Camera::GetMoveSpeed, &Camera::SetMoveSpeed, "Move Speed")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .SetDocumentation("Adjusts the Move speed of the camera");

    YTEBindProperty(&Camera::GetRotateSpeed, &Camera::SetRotateSpeed, "Rotate Speed")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .SetDocumentation("Adjusts the Rotate speed of the camera");

    YTEBindProperty(&Camera::GetGlobalIlluminaton, &Camera::SetGlobalIlluminaton, "Global Illumination")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .SetDocumentation("Adjusts the illumination of the world");

    YTEBindProperty(&Camera::GetFogColor, &Camera::SetFogColor, "Fog Color")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .SetDocumentation("Adjusts the color of the fog");

    YTEBindProperty(&Camera::GetFogPlanes, &Camera::SetFogPlanes, "Fog Near/Far")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .SetDocumentation("Adjusts Fog near and far planes (start of fog, full fog)");

    YTEBindProperty(&Camera::GetFogCoeffs, &Camera::SetFogCoeffs, "Fog Coefficients")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .SetDocumentation("Adjusts the fog coefficients of the world");
  } 
  
  Camera::Camera(Composition *aOwner,
                 Space *aSpace,
                 RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mEngine(nullptr)
    , mCameraTransform(nullptr)
    , mCameraOrientation(nullptr)
    , mTargetObject(nullptr)
    , mFieldOfViewY(glm::radians(45.0f))
    , mNearPlane(0.1f)
    , mFarPlane(9999.9f)
    , mZoom(5.0f)
    , mZoomMin(5.0f)
    , mZoomMax(75.0f)
    , mMoveUp(0.0f)
    , mMoveRight(0.0f)
    , mPitch(0.0f)
    , mYaw(0.0f)
    , mRoll(0.0f)
    , mDt(0.0f)
    , mConstructing(true)
    , mType(CameraType::Flyby)
    , mChanged(true)
    , mPanSpeed(2.0f)
    , mMoveSpeed(50.0f)
    , mScrollSpeed(200.0f)
    , mRotateSpeed(0.80f)
    , mSpeedLimiter(1.0f)
  { 
    DeserializeByType(aProperties, this, GetStaticType()); 
 
    mConstructing = false; 

    mTargetPoint = glm::vec3(0.0f, 0.0f, 0.0f);
  } 
 
  void Camera::Initialize()
  {
    mGraphicsView = mSpace->GetComponent<GraphicsView>();
    mWindow = mGraphicsView->GetWindow();
    mEngine = mSpace->GetEngine();

    mMouse = &mWindow->mMouse;
    mKeyboard = &mWindow->mKeyboard;

    mGraphicsView->YTERegister(Events::RendererResize, this, &Camera::RendererResize);
    mGraphicsView->YTERegister(Events::SurfaceGained, this, &Camera::SurfaceGainedEvent);
    mMouse->YTERegister(Events::MouseMove, this, &Camera::MouseMove);
    mMouse->YTERegister(Events::MouseScroll, this, &Camera::MouseScroll);
    mMouse->YTERegister(Events::MousePress, this, &Camera::MousePress);
    mMouse->YTERegister(Events::MouseRelease, this, &Camera::MouseRelease);
    mMouse->YTERegister(Events::MousePersist, this, &Camera::MousePersist);
    mSpace->YTERegister(Events::FrameUpdate, this, &Camera::Update);
    mOwner->YTERegister(Events::OrientationChanged, this, &Camera::OrientationEvent);
 
    mCameraTransform = mOwner->GetComponent<Transform>(); 
    mCameraOrientation = mOwner->GetComponent<Orientation>(); 

    mIllumination.mCameraPosition = glm::vec4(mCameraTransform->GetTranslation(), 1.0f);
    mIllumination.mFogCoefficients = glm::vec4(1.0f, 0.1f, 0.0f, 0.0f);
    mIllumination.mFogColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    mIllumination.mFogPlanes = glm::vec2(mFarPlane / 2.0f, mFarPlane);
    mIllumination.mGlobalIllumination = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
    mIllumination.mTime = 0.0f;

    mConstructing = false;
    RendererResize(nullptr);
    UpdateView();
  }

  // generic view matrix creation
  static glm::mat4 CreateViewMatrix(const glm::vec3 &aRight,
                                    const glm::vec3 &aUp,
                                    const glm::vec3 &aForward,
                                    const glm::vec3 &aPosition)
  {
    return { glm::vec4{ aRight.x, aRight.y, aRight.z, 0.0f },
             glm::vec4{ aUp.x, aUp.y, aUp.z, 0.0f },
             glm::vec4{ aForward.x, aForward.y, aForward.z, 0.0f },
             glm::vec4{ aPosition.x, aPosition.y, aPosition.z, 1.0f } };
  }

  UBOView Camera::ConstructUBOView()
  {
    auto height = static_cast<float>(mWindow->GetHeight());
    auto width = static_cast<float>(mWindow->GetWidth());

    UBOView view;

    // projection matrix (since its an easy calculation, Ill leave it here for now, but
    // it really doesn't need to happen every view update
    view.mProjectionMatrix = glm::perspective(mFieldOfViewY,
    
                                          width / height,
                                              mNearPlane,
                                              mFarPlane);



    // matrix does perspective divide and flips vulkan y axis
    const glm::mat4 clip(1.0f, 0.0f, 0.0f, 0.0f,
                         0.0f, -1.0f, 0.0f, 0.0f,
                         0.0f, 0.0f, 0.5f, 0.0f,
                         0.0f, 0.0f, 0.5f, 1.0f);

   view.mProjectionMatrix = clip * view.mProjectionMatrix;
    //view.mProjectionMatrix[1][1] *= -1;   // flips vulkan y axis up, since it defaults down

    switch (mType)
    {
      case CameraType::CameraOrientation:
      {
        glm::vec3 right{ 1.0f, 0.0f, 0.0f };
        glm::vec3 up{ 0.0f, 1.0f, 0.0f };
        glm::vec3 forward{ 0.0f, 0.0f, 1.0f };

        if (mCameraTransform)
        {
          right = mCameraOrientation->GetRightVector();
          up = mCameraOrientation->GetUpVector();
          forward = mCameraOrientation->GetForwardVector();
        }

        view.mViewMatrix = CreateViewMatrix(right, up, forward, mCameraTransform->GetTranslation());
        break;
      }
      case CameraType::TargetObject:
      {
        if (mTargetObject)
        {
          mTargetPoint = mTargetObject->GetTranslation();
        }

        glm::quat rot = mCameraTransform->GetRotation();
        glm::vec4 up4(0.0f, 1.0f, 0.0f, 1.0f);
        up4 = glm::rotate(rot, up4);

        glm::vec4 zoomVector(0.0f, 0.0f, mZoom, 1.0f);
        zoomVector = glm::rotate(rot, zoomVector);

        // NOTE: No translation vector is allowed, our translation comes from the target object

        glm::vec3 camTrans = mTargetPoint + glm::vec3(zoomVector);
        mCameraTransform->SetTranslation(camTrans);

        view.mViewMatrix = glm::lookAt(camTrans, mTargetPoint, glm::vec3(up4));
        break;
      }
      case CameraType::TargetPoint:
      {
        glm::quat rot = mCameraTransform->GetRotation();
        glm::vec4 up4(0.0f, 1.0f, 0.0f, 1.0f);
        up4 = glm::rotate(rot, up4);

        glm::vec4 transVector(mMoveRight, mMoveUp, 0.0f, 1.0f);
        transVector = glm::rotate(rot, transVector);

        glm::vec4 zoomVector(0.0f, 0.0f, mZoom, 1.0f);
        zoomVector = glm::rotate(rot, zoomVector);

        mTargetPoint += glm::vec3(transVector);

        glm::vec3 camTrans = mTargetPoint + glm::vec3(zoomVector);
        mCameraTransform->SetTranslation(camTrans);

        view.mViewMatrix = glm::lookAt(camTrans, mTargetPoint, glm::vec3(up4));

        mMoveRight = 0.0f;
        mMoveUp = 0.0f;

        break;
      }
      case CameraType::Gameplay:
      {
        glm::vec3 right{ 1.0f, 0.0f, 0.0f };
        glm::vec3 up{ 0.0f, 1.0f, 0.0f };
        glm::vec3 forward{ 0.0f, 0.0f, 1.0f };

        right = mCameraOrientation->GetRightVector();
        up = mCameraOrientation->GetUpVector();
        forward = mCameraOrientation->GetForwardVector();

        glm::quat rot = mCameraTransform->GetWorldRotation();
        glm::vec4 up4(0.0f, 1.0f, 0.0f, 1.0f);
        up4 = glm::rotate(rot, up4);

        glm::vec4 offsetVector(0.0f, 0.0f, 1.0f, 1.0f);
        offsetVector = glm::rotate(rot, offsetVector);

        view.mViewMatrix = glm::lookAt(mCameraTransform->GetWorldTranslation(), mCameraTransform->GetWorldTranslation() + glm::vec3(offsetVector), glm::vec3(up4));
        //view.mViewMatrix = CreateViewMatrix(right, up, forward, mCameraTransform->GetTranslation());
        break;
      }
      case CameraType::Flyby:
      {
        glm::quat rot = mCameraTransform->GetRotation();
        glm::vec4 up4(0.0f, 1.0f, 0.0f, 1.0f);
        up4 = glm::rotate(rot, up4);

        glm::vec4 transVector(mMoveRight, mMoveUp, mZoom, 1.0f);
        transVector = glm::rotate(rot, transVector);

        // update camera translation
        mCameraTransform->SetTranslation(mCameraTransform->GetTranslation() + glm::vec3(transVector));

        glm::vec4 unitVector(0.0f, 0.0f, 1.0f, 1.0f); // used to translate away from target point by 1
        unitVector = glm::rotate(rot, unitVector);
        unitVector = glm::normalize(unitVector);

        // apply the new target position based on the camera translation
        mTargetPoint = mCameraTransform->GetTranslation() - glm::vec3(unitVector);

        view.mViewMatrix = glm::lookAt(mCameraTransform->GetTranslation(), mTargetPoint, glm::vec3(up4));

        // reset for next frame
        mMoveUp = 0.0f;
        mMoveRight = 0.0f;
        mZoom = 0.0f;

        break;
      }
    }

    view.mCameraPosition = glm::vec4(mCameraTransform->GetTranslation(), 1.0f);

    return view;
  }


  UBOView Camera::InvertAcrossPlane(float aWorldGroundHeight)
  {
    auto height = static_cast<float>(mWindow->GetHeight());
    auto width = static_cast<float>(mWindow->GetWidth());

    UBOView view;

    // projection matrix (since its an easy calculation, Ill leave it here for now, but
    // it really doesn't need to happen every view update
    view.mProjectionMatrix = glm::perspective(mFieldOfViewY,
                                              width / height,
                                              mNearPlane,
                                              mFarPlane);



    // matrix does perspective divide and flips vulkan y axis
    const glm::mat4 clip(1.0f, 0.0f, 0.0f, 0.0f,
                         0.0f, -1.0f, 0.0f, 0.0f,
                         0.0f, 0.0f, 0.5f, 0.0f,
                         0.0f, 0.0f, 0.5f, 1.0f);

    view.mProjectionMatrix = clip * view.mProjectionMatrix;

    glm::quat rot = mCameraTransform->GetRotation();

    glm::vec3 euler = mCameraTransform->GetRotationAsEuler();
    euler.x *= -1.0f;
    mCameraTransform->SetRotation(euler);
    glm::quat rot2 = mCameraTransform->GetRotation();
    glm::vec4 up4(0.0f, 1.0f, 0.0f, 1.0f);
    up4 = glm::rotate(rot2, up4);
    glm::vec3 trans = mCameraTransform->GetTranslation();
    float amount = trans.y - aWorldGroundHeight;
    trans.y -= amount;
    view.mViewMatrix = glm::lookAt(trans, mTargetPoint, glm::vec3(up4));
    view.mCameraPosition = glm::vec4(trans, 1.0f);

    mCameraTransform->SetRotation(rot);
    return view;
  }

  UBOView Camera::ResetInversion()
  {
    return ConstructUBOView();
  }


  void Camera::SurfaceGainedEvent(ViewChanged *aEvent)
  {
    mGraphicsView = aEvent->View;
    mWindow = aEvent->Window;
  }

  void Camera::UpdateView()
  {
    if (mChanged == false)
    {
      return;
    }

    UBOView view = ConstructUBOView();

    mChanged = false;
    mGraphicsView->UpdateView(this, view);
    mIllumination.mCameraPosition = glm::vec4(mCameraTransform->GetTranslation(), 1.0f);
    mIllumination.mTime += static_cast<float>(mDt);
    mGraphicsView->UpdateIllumination(mIllumination);
  }

  void Camera::SetCameraAsActive()
  {
    UBOView view = ConstructUBOView();
    mChanged = false;
    mGraphicsView->UpdateView(this, view);
    mIllumination.mCameraPosition = glm::vec4(mCameraTransform->GetTranslation(), 1.0f);
    mIllumination.mTime += static_cast<float>(mDt);
    mGraphicsView->UpdateIllumination(mIllumination);
  }

  void Camera::SetUBOView(UBOView &aView)
  {
    mChanged = false;
    mGraphicsView->UpdateView(this, aView);
    mIllumination.mCameraPosition = glm::vec4(mCameraTransform->GetTranslation(), 1.0f);
    mIllumination.mTime += static_cast<float>(mDt);
    mGraphicsView->UpdateIllumination(mIllumination);
  }

  // Handle the moment a mouse button is pressed
  void Camera::MousePress(MouseButtonEvent *aEvent)
  {
    // Get the mouse position
    mMouseInitialPosition = aEvent->WorldCoordinates;

    if (Mouse_Buttons::Right == aEvent->Button && CameraType::Flyby == mType)
    {
      mWindow->SetCursorVisibility(false);
    }
  }

  // Handle the moment a mouse button is released
  void Camera::MouseRelease(MouseButtonEvent *aEvent)
  {
    if (Mouse_Buttons::Right == aEvent->Button && CameraType::Flyby == mType)
    {
      mWindow->SetCursorVisibility(true);
    }
  }

  // Handle the moment a mouse button is pressed
  void Camera::MouseScroll(MouseWheelEvent *aEvent)
  {
    switch (mType)
    {
      // both of these act the same
    case CameraType::TargetObject:
    case CameraType::TargetPoint:
    {
      UpdateZoom(aEvent->ScrollMovement.y * -mScrollSpeed * static_cast<float>(mDt));  // Zooms in and out
      mChanged = true;
      break;
    }
    case CameraType::Flyby:
    {
      // modify mZoom directly since this is a movement, not a zooming motion
      // negate movement since scroll in is positive value
      mZoom = mScrollSpeed * -(aEvent->ScrollMovement.y) * static_cast<float>(mDt);   // Moves in and out
      mChanged = true;
      break;
    }
    }
  }

  // Handle movement of the mouse. Should be fairly clear by code alone.
  void Camera::MouseMove(MouseMoveEvent *aEvent)
  {
    auto dt = static_cast<float>(mDt);

    // finds the delta of mouse movement from the last frame
    float dx = static_cast<float>(aEvent->WorldCoordinates.x - mMouseInitialPosition.x);
    float dy = static_cast<float>(aEvent->WorldCoordinates.y - mMouseInitialPosition.y);

    switch (mType)
    {
    case CameraType::TargetObject:
    case CameraType::TargetPoint:
    {
      // if the alt key is not being pressed, then there is no camera movement
      // note this only works because the scroll event is a different function
      // the scroll wheel must zoom without pressing the alt key
      if (false == mKeyboard->IsKeyDown(Keys::Alt))
      {
        return;
      }

      if (mMouse->IsButtonDown(Mouse_Buttons::Left))
      {
        // Left Mouse does Arc ball rotation
        UpdateCameraRotation(-dy * mRotateSpeed * dt, -dx * mRotateSpeed * dt, 0.0f);
      }

      if (mMouse->IsButtonDown(Mouse_Buttons::Right))
      {
        // Right Mouse does Zoom
        UpdateZoom(dy * mRotateSpeed * dt);
      }

      if (mMouse->IsButtonDown(Mouse_Buttons::Middle))
      {
        // middle mouse does camera panning

        // we change the camera type since we were following an object and we just
        // jumped off to walk on our own
        mMoveUp = dy * mPanSpeed * dt;
        mMoveRight = -dx * mPanSpeed * dt;
        mChanged = true;
      }

      break;
    }
    case CameraType::Flyby:
    {
      // allows look and move with WASD
      if (mMouse->IsButtonDown(Mouse_Buttons::Right))
      {
        UpdateCameraRotation(-dy * mRotateSpeed * dt, -dx * mRotateSpeed * dt, 0.0f);
      }

      // allows panning
      if (mMouse->IsButtonDown(Mouse_Buttons::Middle))
      {
        // mouse moving up is a negative number, but it should drag the view up, which is translate down
        mMoveUp = dy * mPanSpeed * dt;
        mMoveRight = -dx * mPanSpeed * dt;
        mChanged = true;
      }

      break;
    }
    }

    // Set mouse position for next frame
    mMouseInitialPosition.x = aEvent->WorldCoordinates.x;
    mMouseInitialPosition.y = aEvent->WorldCoordinates.y;
  }

  void Camera::MousePersist(MouseButtonEvent *aEvent)
  {
    auto dt = static_cast<float>(mDt);

    YTEUnusedArgument(aEvent);

    if (CameraType::Flyby == mType && Mouse_Buttons::Right == aEvent->Button)
    {
      if (mKeyboard->IsKeyDown(Keys::W))                                      // forward movement
      {
        mZoom = -mMoveSpeed * dt;
        mChanged = true;
      }
      else if (mKeyboard->IsKeyDown(Keys::S))                                 // backward movement
      {
        mZoom = mMoveSpeed * dt;
        mChanged = true;
      }
      if (mKeyboard->IsKeyDown(Keys::A))                                      // Left Movement
      {
        mMoveRight = -mMoveSpeed * dt;
        mChanged = true;
      }
      else if (mKeyboard->IsKeyDown(Keys::D))                                 // Right Movement
      {
        mMoveRight = mMoveSpeed * dt;
        mChanged = true;
      }
      if (mKeyboard->IsKeyDown(Keys::Space) || mKeyboard->IsKeyDown(Keys::E)) // Up Movement
      {
        mMoveUp = mMoveSpeed * dt;
        mChanged = true;
      }
      else if (mKeyboard->IsKeyDown(Keys::Control) || mKeyboard->IsKeyDown(Keys::Q)) // Down Movement
      {
        mMoveUp = -mMoveSpeed * dt;
        mChanged = true;
      }

      // speed mode
      if (mKeyboard->IsKeyDown(Keys::Shift))          // Speed increase
      {
        mMoveRight *= 2.0f;
        mMoveUp *= 2.0f;
        mZoom *= 2.0f;
      }
    }
  }

  void Camera::OrientationEvent(OrientationChanged *aEvent)
  {
    YTEUnusedArgument(aEvent);
    mChanged = true;
  }

  void Camera::Update(LogicUpdate *aEvent)
  {
    YTEProfileFunction(profiler::colors::Red);
    if (mEngine->IsEditor())
    {
      if (mType == CameraType::Gameplay)
      {
        if (mSpace->GetName() != "YTEditor Play Space")
        {
          return;
        }
      }
      else if (mType != CameraType::Gameplay)
      {
        //if (mSpace->GetName() == "YTEditor Play Space")
        //{
        //  return;
        //}
      }
    }
    else if (mType != CameraType::Gameplay)
    {
      return;
    }


    //if (mType == CameraType::Gameplay && mEngine->IsEditor() == true)
    //{
    //  return;
    //}
    //else if (mType != CameraType::Gameplay && mEngine->IsEditor() == false)
    //{
    //  return;
    //}

    mDt = aEvent->Dt * mSpeedLimiter;
    if (mChanged)
    {
      UpdateView();
    }
    else
    {
      mIllumination.mTime += static_cast<float>(mDt);
      mGraphicsView->UpdateIllumination(mIllumination);
    }
  }

  void Camera::RendererResize(WindowResize *aEvent)
  {
    YTEUnusedArgument(aEvent);

    float height = static_cast<float>(mWindow->GetHeight());
    float width = static_cast<float>(mWindow->GetWidth());

    // 1920 used to keep normalized movement over a 1080p screen to all other types
    mSpeedLimiter = ((1.0f / (width / height)) / 2.0f);

    mChanged = true;
  }


  
  void Camera::SetCameraType(std::string &aCameraType)
  {
    // make sure we use the correct types
    if ((false == (aCameraType != "TargetObject")) &&
      (false == (aCameraType != "TargetPoint")) &&
      (false == (aCameraType != "CameraOrientation")) &&
      (false == (aCameraType != "Flyby")) &&
      (false == (aCameraType != "Gameplay")))
    {
      return;
    }


    // Set the string type
    std::string oldCameraType = mCameraType;
    mCameraType = aCameraType;


    if (aCameraType == "Gameplay")
    {
      mType = CameraType::Gameplay;
    }
    else if (aCameraType == "TargetObject")
    {
      // validate we can be a target object
      if (nullptr == mTargetObject)
      {
        printf("TargetObject is not set on Camera named \"%s\" \n",
          mOwner->GetName().c_str());
        mCameraType = oldCameraType;
        return;
      }

      ToTargetCamera(true);
      mType = CameraType::TargetObject;
    }
    else if (aCameraType == "TargetPoint")
    {
      ToTargetCamera(false);
      mType = CameraType::TargetPoint;
    }
    else if (aCameraType == "CameraOrientation")
    {
      mCameraOrientation = mOwner->GetComponent<Orientation>();

      if (nullptr == mCameraOrientation)
      {
        //mCameraType = oldCameraType;
        return;
      }

      mType = CameraType::CameraOrientation;
    }
    else if (aCameraType == "Flyby")
    {
      ToFlybyCamera();

      mType = CameraType::Flyby;
    }

    // Update view for the new camera type
    if (false == mConstructing)
    {
      mChanged = true;
    }
  }

  void Camera::UpdateCameraRotation(float aPitch, float aYaw, float aRoll)
  {
    // apply rotation
    mRoll += aRoll;
    mPitch += aPitch;
    mYaw += aYaw;

    glm::quat rot = mCameraTransform->GetRotation();

    mCameraTransform->SetRotation(glm::quat(glm::vec3(mPitch, mYaw, mRoll)));
    rot = mCameraTransform->GetRotation();

    mChanged = true;
  }

  void Camera::UpdateZoom(float aZoom)
  {
    mZoom += aZoom;

    // constrains zoom
    if (mZoom >= mZoomMax)
    {
      mZoom = mZoomMax;
    }
    else if (mZoom <= mZoomMin)
    {
      mZoom = mZoomMin;
    }
    mChanged = true;
  }

  void Camera::ToTargetCamera(bool aUseObject)
  {
    if (true == mConstructing)
    {
      return;
    }

    if (aUseObject)
    {
      // Find the distance between the target object and the camera current position
      glm::vec3 fromCamToObj = mCameraTransform->GetTranslation() - mTargetObject->GetTranslation();
      float dist = glm::length(fromCamToObj);

      // Set the zoom min or max to this value if it is out of bounds
      if (dist >= mZoomMax)
      {
        mZoomMax = dist;
      }
      else if(dist <= mZoomMin)
      {
        mZoomMin = dist;
      }
      mZoom = dist;
      mTargetPoint = mTargetObject->GetTranslation();
    }
    else
    {
      // Set the target point to be the point at the camera position + min zoom(rotated)
      glm::quat rot = mCameraTransform->GetRotation();
      glm::vec4 zoomFac(0.0f, 0.0f, mZoomMin, 1.0f);
      zoomFac = glm::rotate(rot, zoomFac);

      mZoom = mZoomMin;
      mTargetPoint = mCameraTransform->GetTranslation() - glm::vec3(zoomFac);
    }
    mMoveRight = 0.0f;
    mMoveUp = 0.0f;
  }

  void Camera::ToFlybyCamera()
  {
    if (true == mConstructing)
    {
      return;
    }

    // Set Target point to be 1 distance away from from the camera current position
    glm::quat rot = mCameraTransform->GetRotation();
    glm::vec4 unit(0.0f, 0.0f, 1.0f, 0.0f);
    unit = glm::rotate(rot, unit);
    mTargetPoint = mCameraTransform->GetTranslation() + glm::vec3(unit);

    // reset zoom
    mZoom = 0.0f;
    mMoveRight = 0.0f;
    mMoveUp = 0.0f;
  }
}
