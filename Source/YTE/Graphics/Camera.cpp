#include <glm/gtx/euler_angles.hpp>

#include "YTE/Core/Engine.hpp" 
#include "YTE/Core/Space.hpp" 
#include "YTE/Core/Utilities.hpp" 
 
#include "YTE/Graphics/Camera.hpp" 
#include "YTE/Graphics/GraphicsView.hpp" 
#include "YTE/Graphics/GraphicsSystem.hpp" 
 
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
    return { "TargetObject", "TargetPoint", "CameraOrientation", "Flyby" }; 
  }



  // ------------------------------------ 
  YTEDefineType(Camera) 
  { 
    YTERegisterType(Camera); 

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
                        " - Flyby: The camera behaves like an FPS, where it uses WASD."); 
    
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


    // TODO: Do these need to be here?
    YTEBindField(&Camera::mZoom, "Zoom", PropertyBinding::GetSet)
      .AddAttribute<Serializable>();
    YTEBindField(&Camera::mMoveUp, "MoveUp", PropertyBinding::GetSet)
      .AddAttribute<Serializable>();
    YTEBindField(&Camera::mMoveRight, "MoveRight", PropertyBinding::GetSet)
      .AddAttribute<Serializable>();
  } 
   
  
  // ------------------------------------ 
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
    , mFarPlane(256.0f)
    , mZoom(5.0f)
    , mZoomMin(10.0f)
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
    , mPanSpeed(1.0f)
    , mMoveSpeed(25.0f)
    , mScrollSpeed(100.0f)
    , mRotateSpeed(0.20f)
  { 
    DeserializeByType<Camera*>(aProperties, this, Camera::GetStaticType()); 
 
    mGraphicsView = mSpace->GetComponent<GraphicsView>(); 
 
    mWindow = mGraphicsView->GetWindow(); 
    mEngine = mSpace->GetEngine();
    mWindow->YTERegister(Events::RendererResize, this, &Camera::RendererResize); 
    mConstructing = false; 

    mTargetPoint = glm::vec3(0.0f, 0.0f, 0.0f);
  } 
 

  // ------------------------------------ 
  void Camera::Initialize()
  { 
    mMouse = &mWindow->mMouse;
    mKeyboard = &mWindow->mKeyboard;

    mMouse->YTERegister(Events::MouseMove, this, &Camera::MouseMove);
    mMouse->YTERegister(Events::MouseScroll, this, &Camera::MouseScroll);
    mMouse->YTERegister(Events::MousePress, this, &Camera::MousePress);
    mMouse->YTERegister(Events::MousePersist, this, &Camera::MousePersist);
    mEngine->YTERegister(Events::GraphicsDataUpdate, this, &Camera::Update);
    mOwner->YTERegister(Events::OrientationChanged, this, &Camera::OrientationEvent);
 
    mCameraTransform = mOwner->GetComponent<Transform>(); 
    mCameraOrientation = mOwner->GetComponent<Orientation>(); 

    glm::vec3 rotFromFile = mCameraTransform->GetRotationAsEuler();
    mPitch = rotFromFile.x;
    mYaw = rotFromFile.y;
    mRoll = rotFromFile.z;
  } 


  // ------------------------------------ 
  // TODO: This event is currently in Engine.hpp and Space.cpp and should be called from 
  void Camera::Update(GraphicsDataUpdate* aEvent)
  {
    mDt = aEvent->Dt;
    if (mChanged)
    {
      UpdateView();
    }
  }


  // ------------------------------------ 
  void Camera::SetCameraType(std::string &aCameraType)
  {
    // make sure we use the correct types
    if ((false == (aCameraType != "TargetObject")) &&
        (false == (aCameraType != "TargetPoint")) &&
        (false == (aCameraType != "CameraOrientation")) &&
        (false == (aCameraType != "Flyby")))
    {
      return;
    }


    // Set the string type
    std::string oldCameraType = mCameraType;
    mCameraType = aCameraType;

    if (aCameraType == "TargetObject")
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
    // --------------------------------------
    else if (aCameraType == "TargetPoint")
    {
      ToTargetCamera(false);
      mType = CameraType::TargetPoint;
    }
    // --------------------------------------
    else if (aCameraType == "CameraOrientation")
    {
      mCameraOrientation = mOwner->GetComponent<Orientation>();

      if (nullptr == mCameraOrientation)
      {
        return;
        mCameraType = oldCameraType;
      }

      mType = CameraType::CameraOrientation;
    }
    // --------------------------------------
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


  // ------------------------------------ 
  void Camera::ToTargetCamera(bool aUseObject)
  {
    if (aUseObject)
    {
      // TODO: Find the distance between the target object and the camera current position
      // TODO: Set the zoom min or max to this value if it is out of bounds
    }
    else
    {
      // TODO: Set the target point to be the point at the camera position + min zoom(rotated)
    }
    // TODO: Set target point to be the target object translation


    //glm::vec3 rot = mCameraTransform->GetRotationAsEuler();
    //glm::mat4 rotationMatrix = glm::yawPitchRoll(rot.y, rot.x, rot.z);
    //glm::vec4 unitVector(0.0f, 0.0f, mZoomMin, 1.0f); // used to translate away from target point by 1
    //unitVector = rotationMatrix * unitVector;
    // // apply the new target position based on the camera translation
    //mTargetPoint = mCameraTransform->GetTranslation() - glm::vec3(unitVector);
    //mZoom = mZoomMin;
    //mMoveUp = 0.0f;
    //mMoveRight = 0.0f;
  }


  // ------------------------------------
  void Camera::ToFlybyCamera()
  {
    // TODO: Set Target point to be 1 distance away from (1 rotated zoom) from the camera current position

    // reset zoom
    mZoom = 0.0f;
  }


  // ------------------------------------ 
  // Handle the moment a mouse button is pressed
  void Camera::MousePress(MouseButtonEvent *aEvent)
  {
    // Get the mouse position
    mMouseInitialPosition = aEvent->WorldCoordinates;
  }


  // ------------------------------------ 
  void Camera::MousePersist(MouseButtonEvent *aEvent)
  {
    YTEUnusedArgument(aEvent);

    if (CameraType::Flyby == mType && mMouse->IsButtonDown(Mouse_Buttons::Right))
    {
      if (mKeyboard->IsKeyDown(Keys::W))              // forward movement
      {
        mZoom = -mMoveSpeed * mDt;
        mChanged = true;
      }
      else if (mKeyboard->IsKeyDown(Keys::S))         // backward movmenet
      {
        mZoom = mMoveSpeed * mDt;
        mChanged = true;
      }
      if (mKeyboard->IsKeyDown(Keys::A))              // Left Movement
      {
        mMoveRight = -mMoveSpeed * mDt;
        mChanged = true;
      }
      else if (mKeyboard->IsKeyDown(Keys::D))         // Right Movement
      {
        mMoveRight = mMoveSpeed * mDt;
        mChanged = true;
      }
      if (mKeyboard->IsKeyDown(Keys::Space))          // Up Movement
      {
        mMoveUp = mMoveSpeed * mDt;
        mChanged = true;
      }
      else if (mKeyboard->IsKeyDown(Keys::Control))   // Down Movement
      {
        mMoveUp = -mMoveSpeed * mDt;
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


  // ------------------------------------ 
  // Handle the moment a mouse button is pressed
  void Camera::MouseScroll(MouseWheelEvent *aEvent)
  {
    switch (mType)
    {
      // both of these act the same
      case CameraType::TargetObject:
      case CameraType::TargetPoint:
      {
        UpdateZoom(aEvent->ScrollMovement.y * mScrollSpeed * mDt);  // Zooms in and out
        mChanged = true;
        break;
      }
      case CameraType::Flyby:
      {
        // modify mZoom directly since this is a movement, not a zooming motion
        // negate movement since scroll in is positive value
        mZoom = mScrollSpeed * -(aEvent->ScrollMovement.y) * mDt;   // Moves in and out
        mChanged = true;
        break;
      }
    }
  }


  // ------------------------------------ 
  // Handle movement of the mouse. Should be fairly clear by code alone.
  void Camera::MouseMove(MouseMoveEvent *aEvent)
  {
    // finds the delta of mouse movement from the last frame
    float dx = aEvent->WorldCoordinates.x - mMouseInitialPosition.x;
    float dy = aEvent->WorldCoordinates.y - mMouseInitialPosition.y;

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
          UpdateCameraRotation(-dy * mRotateSpeed * mDt, -dx * mRotateSpeed * mDt, 0.0f);
        }

        if (mMouse->IsButtonDown(Mouse_Buttons::Right))
        {
          // Right Mouse does Zoom
          UpdateZoom(dy * mScrollSpeed * mDt);
        }

        if (mMouse->IsButtonDown(Mouse_Buttons::Middle))
        {
          // middle mouse does camera panning

          // we change the camera type since we were following an object and we just
          // jumped off to walk on our own
          if (mType == CameraType::TargetObject)
          {
            mType = CameraType::TargetPoint;
            mCameraType = "TargetPoint";
          }

          mMoveUp = dy * mPanSpeed * mDt;
          mMoveRight = dx * mPanSpeed * mDt;
        }

        break;
      }
      // --------------------------------------
      case CameraType::Flyby:
      {
        // allows look and move with WASD
        if (mMouse->IsButtonDown(Mouse_Buttons::Right))
        {
          UpdateCameraRotation(-dy * mRotateSpeed * mDt, dx * mRotateSpeed * mDt, 0.0f);
        }

        // allows panning
        if (mMouse->IsButtonDown(Mouse_Buttons::Middle))
        {
          // mouse moving up is a negative number, but it should drag the view up, which is translate down
          mMoveUp = dy * mPanSpeed * mDt;
          mMoveRight = -dx * mPanSpeed * mDt;
          mChanged = true;
        }

        break;
      }
    }

    // Set mouse position for next frame
    mMouseInitialPosition.x = aEvent->WorldCoordinates.x;
    mMouseInitialPosition.y = aEvent->WorldCoordinates.y;
  }
 

  // ------------------------------------ 
  void Camera::OrientationEvent(OrientationChanged *aEvent)
  {
    YTEUnusedArgument(aEvent);
    mChanged = true;
  } 

 
  // ------------------------------------ 
  void Camera::RendererResize(WindowResize *aEvent)
  {
    YTEUnusedArgument(aEvent);
    mChanged = true;
  } 
 

  // ------------------------------------ 
  // generic view matrix creation
  static glm::mat4 CreateViewMatrix(const glm::vec3 &aRight,
                                    const glm::vec3 &aUp,  
                                    const glm::vec3 &aForward,  
                                    const glm::vec3 &aPosition) 
  { 
    return {glm::vec4{aRight.x, aRight.y, aRight.z, 0.0f}, 
            glm::vec4{aUp.x, aUp.y, aUp.z, 0.0f }, 
            glm::vec4{aForward.x, aForward.y, aForward.z, 0.0f}, 
            glm::vec4{aPosition.x, aPosition.y, aPosition.z, 1.0f}}; 
  } 


  // TODO: Is this even needed?
  // ------------------------------------ 
  //glm::mat4 Rotate(glm::vec4 aRotation)
  //{
  //  float xCos = cos(aRotation.x);
  //  float xSin = sin(aRotation.x);
  //  float yCos = cos(aRotation.y);
  //  float ySin = sin(aRotation.y);
  //  float zCos = cos(aRotation.z);
  //  float zSin = sin(aRotation.z);
  //
  //  float ySin_zCos = ySin * zCos;
  //
  //  glm::mat4 rotation;
  //  rotation[0][0] = yCos * zCos;
  //  rotation[0][1] = -yCos * zSin;
  //  rotation[0][2] = ySin;
  //
  //  rotation[1][0] = xCos * zSin + xSin * ySin_zCos;
  //  rotation[1][1] = xCos * zCos - xSin * ySin * zSin;
  //  rotation[1][2] = -xSin * yCos;
  //
  //  rotation[2][0] = xSin * zSin - xCos * ySin_zCos;
  //  rotation[2][1] = xCos * ySin * zSin + xSin * zCos;
  //  rotation[2][2] = xCos * yCos;
  //
  //  return rotation;
  //}
 

  // ------------------------------------ 
  void Camera::UpdateView()
  { 
    if (mChanged == false)
    {
      return;
    }

    auto height = static_cast<float>(mWindow->GetHeight()); 
    auto width  = static_cast<float>(mWindow->GetWidth()); 
 
    UBOView view;
 
    // projection matrix (since its an easy calculation, Ill leave it here for now, but
    // it really doesnt need to happen every view update
    view.mProjectionMatrix = glm::perspective(mFieldOfViewY, 
                                              width / height, 
                                              mNearPlane, 
                                              mFarPlane); 

    view.mProjectionMatrix[1][1] *= -1;   // flips vulkan y axis up, since it defaults down
 

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
      // --------------------------------------
      case CameraType::TargetObject:
      {
        // there is no break here since the math is the same, it just uses a different target pos
        if (mTargetObject)
        {
          mTargetPoint = mTargetObject->GetTranslation();
        }
      }
      // --------------------------------------
      case CameraType::TargetPoint:
      {
        //TODO: 
        // NOTE this math doesn't allow for 3 axis rotation, only 2 axis

        // create the rotation matrix, note that glm::yawpitchroll does the same thing
        glm::vec3 rot = mCameraTransform->GetRotationAsEuler();
        glm::mat4 rotationMatrix = glm::yawPitchRoll(rot.y, rot.x, rot.z);
        
        // create the up vector, and then rotate it. Note that it is -1 to flip the world right side up
        glm::vec4 upReal(0.0f, 1.0f, 0.0f, 1.0f);
        upReal = rotationMatrix * upReal;

        // This is the vector that will translate the camera
        glm::vec4 transVector(mMoveRight, mMoveUp, 0.0f, 1.0f);
        transVector = rotationMatrix * transVector;

        // now we need a rotated zoom vector
        glm::vec4 rotatedZoom = rotationMatrix * glm::vec4(0.0f, 0.0f, mZoom, 1.0f);

        // change the target we are looking at to be in line with the new translation
        mTargetPoint = mTargetPoint + glm::vec3(transVector);

        // apply the zoom to the target point to get the camera location
        glm::vec4 cameraPosition4 = glm::vec4(mTargetPoint, 1.0f) + rotatedZoom;

        // reset the data to the transform component
        // note that the camera position is not saved to the transform, rather where the camera
        // is rotating around is saved. This is more intuitive
        mCameraTransform->SetTranslation(glm::vec3(cameraPosition4));
        
        // finally get the view matrix
        view.mViewMatrix = glm::lookAt(glm::vec3(cameraPosition4), mTargetPoint, glm::vec3(upReal));

        // reset for no translation next frame
        mMoveUp = 0.0f;
        mMoveRight = 0.0f;

        break;
      }
      // --------------------------------------
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

    mChanged = false;
    mGraphicsView->UpdateView(this, view); 
  } 


  // ------------------------------------ 
  void Camera::UpdateCameraRotation(float aPitch, float aYaw, float aRoll)
  {
    // apply rotation
    mRoll += aRoll;
    mPitch += aPitch;
    mYaw += aYaw;

    glm::quat rot = mCameraTransform->GetRotation();
    std::cout << "Previous Rotation: (" << rot.x << ", " << rot.y << ", " << rot.z << ", " << rot.w << ") Now Is: (";

    mCameraTransform->SetRotation(glm::quat(glm::vec3(mPitch, mYaw, mRoll)));
    rot = mCameraTransform->GetRotation();
    std::cout << rot.x << ", " << rot.y << ", " << rot.z << ", " << rot.w << std::endl;

    mChanged = true;
  }


  // ------------------------------------ 
  void Camera::UpdateZoom(float aZoom)
  {
    mZoom += aZoom;

    // constrains zoom
    // TODO: Useful for in game, not so much useful for editor
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
}
