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



static const float moveSpeed = 25.0f;
static const float rotationSpeed = 0.25f;
static const float zoomSpeed = 100.0f;
static const float panSpeed = 5.0f;


 
namespace YTE 
{ 
  static std::vector<std::string> PopulateDropDownList(Component *aComponent) 
  { 
    return { "TargetObject", "TargetPoint", "CameraOrientation", "Flyby" }; 
  } 
   
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

    YTEBindField(&Camera::mZoom, "Zoom", PropertyBinding::GetSet)
      .AddAttribute<Serializable>();

    YTEBindField(&Camera::mMoveUp, "MoveUp", PropertyBinding::GetSet)
      .AddAttribute<Serializable>();
    YTEBindField(&Camera::mMoveRight, "MoveRight", PropertyBinding::GetSet)
      .AddAttribute<Serializable>();
  } 
   
  // NOTE: Spin = x
  //       Tilt = y
  //       Twist = z


  Camera::Camera(Composition *aOwner, 
                 Space *aSpace,  
                 RSValue *aProperties) 
    : Component(aOwner, aSpace)
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
    , mDt(0.0f)
    , mConstructing(true)
    , mType(CameraType::CameraOrientation) 
  { 
    auto engine = aSpace->GetEngine(); 
    DeserializeByType<Camera*>(aProperties, this, Camera::GetStaticType()); 
 
    mGraphicsView = mSpace->GetComponent<GraphicsView>(); 
 
    mWindow = mGraphicsView->GetWindow(); 
    mWindow->YTERegister(Events::RendererResize, this, &Camera::RendererResize); 
    mConstructing = false; 

    mTargetPoint = glm::vec3(0.0f, 0.0f, 0.0f);
  } 
 
  void Camera::Initialize() 
  { 
    mOwner->YTERegister(Events::PositionChanged, this, &Camera::PositionEvent); 
    mOwner->YTERegister(Events::OrientationChanged, this, &Camera::OrientationEvent); 

    mMouse = &mWindow->mMouse;
    mKeyboard = &mWindow->mKeyboard;

    mMouse->YTERegister(Events::MouseMove, this, &Camera::MouseMove);
    mMouse->YTERegister(Events::MouseScroll, this, &Camera::MouseScroll);
    mMouse->YTERegister(Events::MousePress, this, &Camera::MousePress);
    mMouse->YTERegister(Events::MousePersist, this, &Camera::MousePersist);
    mSpace->YTERegister(Events::LogicUpdate, this, &Camera::Update);
 
    mCameraTransform = mOwner->GetComponent<Transform>(); 
    mCameraOrientation = mOwner->GetComponent<Orientation>(); 

    mTargetPoint = glm::vec3(0.0f, 0.0f, 0.0f);
  } 


  void Camera::Update(LogicUpdate* aEvent)
  {
    mDt = aEvent->Dt;
  }


  void Camera::SetCameraType(std::string &aCameraType)
  {
    if ((false == (aCameraType != "TargetObject")) &&
        (false == (aCameraType != "TargetPoint")) &&
        (false == (aCameraType != "CameraOrientation")) &&
        (false == (aCameraType != "Flyby")))
    {
      return;
    }

    mCameraType = aCameraType;


    if (aCameraType == "TargetObject")
    {
      if (nullptr == mTargetObject)
      {
        printf("TargetObject is not set on Camera named \"%s\" \n", 
               mOwner->GetName().c_str());
        return;
      }

      mType = CameraType::TargetObject;
    }
    else if (aCameraType == "TargetPoint")
    {
      // only do this if we were a flyby camera
      if (mType == CameraType::Flyby)
      {
        glm::vec3 rot = mCameraTransform->GetRotationAsEuler();
        glm::mat4 rotationMatrix = glm::yawPitchRoll(rot.y, rot.x, rot.z);
        glm::vec4 unitVector(0.0f, 0.0f, mZoomMin, 1.0f); // used to translate away from target point by 1
        unitVector = rotationMatrix * unitVector;

        // apply the new target position based on the camera translation
        mTargetPoint = mCameraTransform->GetTranslation() - glm::vec3(unitVector);

        mZoom = mZoomMin;
        mMoveUp = 0.0f;
        mMoveRight = 0.0f;
      }

      mType = CameraType::TargetPoint;
    }
    else if (aCameraType == "CameraOrientation")
    {
      mCameraOrientation = mOwner->GetComponent<Orientation>();

      if (nullptr == mCameraOrientation)
      {
        return;
      }

      mType = CameraType::CameraOrientation;
    }
    else if (aCameraType == "Flyby")
    {
      mZoom = 0.0f;
      mMoveUp = 0.0f;
      mMoveRight = 0.0f;

      mType = CameraType::Flyby;
    }

    if (false == mConstructing)
    {
      UpdateView();
    }
  }


  // Handle the moment a mouse button is pressed
  void Camera::MousePress(MouseButtonEvent *aEvent)
  {
    //const float zoomSpeed = 1.06f;
    mMouseInitialPosition = aEvent->WorldCoordinates;
  }

  void Camera::MousePersist(MouseButtonEvent *aEvent)
  {
    if (CameraType::Flyby == mType && mMouse->IsButtonDown(Mouse_Buttons::Right))
    {
      if (mKeyboard->IsKeyDown(Keys::W))
      {
        mZoom = -moveSpeed * mDt;
      }
      else if (mKeyboard->IsKeyDown(Keys::S))
      {
        mZoom = moveSpeed * mDt;
      }
      if (mKeyboard->IsKeyDown(Keys::A))
      {
        mMoveRight = -moveSpeed * mDt;
      }
      else if (mKeyboard->IsKeyDown(Keys::D))
      {
        mMoveRight = moveSpeed * mDt;
      }
      if (mKeyboard->IsKeyDown(Keys::Space))
      {
        mMoveUp = moveSpeed * mDt;
      }
      else if (mKeyboard->IsKeyDown(Keys::Control))
      {
        mMoveUp = -moveSpeed * mDt;
      }

      // speed mode
      if (mKeyboard->IsKeyDown(Keys::Shift))
      {
        mMoveRight *= 2.0f;
        mMoveUp *= 2.0f;
        mZoom *= 2.0f;
      }
  
      UpdateView();
    }
  }

  // Handle the moment a mouse button is pressed
  void Camera::MouseScroll(MouseWheelEvent *aEvent)
  {
    switch (mType)
    {
      case CameraType::TargetObject:
      case CameraType::TargetPoint:
      {
        UpdateZoom(aEvent->ScrollMovement.y * zoomSpeed * mDt);
        break;
      }
      case CameraType::Flyby:
      {
        // modify mZoom directly since this is a movement, not a zooming motion
        // negate movement since scroll in is positive value
        mZoom = zoomSpeed * -(aEvent->ScrollMovement.y) * mDt;
        break;
      }
    }

    UpdateView();
  }

  // Handle movement of the mouse. Should be fairly clear by code alone.
  void Camera::MouseMove(MouseMoveEvent *aEvent)
  {

    auto dx = aEvent->WorldCoordinates.x - mMouseInitialPosition.x;
    auto dy = aEvent->WorldCoordinates.y - mMouseInitialPosition.y;

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
          UpdateCameraRotation(-dy * rotationSpeed * mDt, dx * rotationSpeed * mDt, 0.0f);
        }

        if (mMouse->IsButtonDown(Mouse_Buttons::Right))
        {
          // Right Mouse does Zoom
          UpdateZoom(dy * zoomSpeed * mDt);
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

          mMoveUp = dy * panSpeed * mDt;
          mMoveRight = dx * panSpeed * mDt;
        }

        break;
      }
      case CameraType::Flyby:
      {
        // allows look and move with WASD
        if (mMouse->IsButtonDown(Mouse_Buttons::Right))
        {
          UpdateCameraRotation(-dy * rotationSpeed * mDt, -dx * rotationSpeed * mDt, 0.0f);
        }

        // allows panning
        if (mMouse->IsButtonDown(Mouse_Buttons::Middle))
        {
          // mouse moving up is a negative number, but it should drag the view up, which is translate down
          mMoveUp = dy * panSpeed * mDt;
          mMoveRight = -dx * panSpeed * mDt;
        }

        break;
      }
    }


    mMouseInitialPosition.x = aEvent->WorldCoordinates.x;
    mMouseInitialPosition.y = aEvent->WorldCoordinates.y;

    UpdateView();
  }
 
  void Camera::OrientationEvent(OrientationChanged *aEvent) 
  { 
    UpdateView(); 
  } 
 
  void Camera::PositionEvent(TransformChanged *aEvent)
  { 
    //UpdateView();  no need and other functions need to be calling it anyway
  } 
 
  void Camera::RendererResize(WindowResize *aEvent) 
  { 
    UpdateView(); 
  } 
 
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


  glm::mat4 Rotate(glm::vec4 aRotation)
  {
    float xCos = cos(aRotation.x);
    float xSin = sin(aRotation.x);
    float yCos = cos(aRotation.y);
    float ySin = sin(aRotation.y);
    float zCos = cos(aRotation.z);
    float zSin = sin(aRotation.z);

    float ySin_zCos = ySin * zCos;

    glm::mat4 rotation;
    rotation[0][0] = yCos * zCos;
    rotation[0][1] = -yCos * zSin;
    rotation[0][2] = ySin;

    rotation[1][0] = xCos * zSin + xSin * ySin_zCos;
    rotation[1][1] = xCos * zCos - xSin * ySin * zSin;
    rotation[1][2] = -xSin * yCos;

    rotation[2][0] = xSin * zSin - xCos * ySin_zCos;
    rotation[2][1] = xCos * ySin * zSin + xSin * zCos;
    rotation[2][2] = xCos * yCos;

    return rotation;
  }
 
  void Camera::UpdateView() 
  { 
    auto window = GetSpace()->GetComponent<GraphicsView>()->GetWindow(); 
 
    auto height = static_cast<float>(mWindow->GetHeight()); 
    auto width  = static_cast<float>(mWindow->GetWidth()); 
 
    UBOView view; 
 
    view.mProjectionMatrix = glm::perspective(mFieldOfViewY, 
                                              width / height, 
                                              mNearPlane, 
                                              mFarPlane); 

    view.mProjectionMatrix[1][1] *= -1;   // flips vulkan y axis up, since it defaults down
 
    glm::vec3 position;
 
    if (mCameraTransform) 
    { 
      position = mCameraTransform->GetTranslation();
    }


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
 
        view.mViewMatrix = CreateViewMatrix(right, up, forward, position); 
        break; 
      } 
      case CameraType::TargetObject:
      {
        // there is no break here since the math is the same, it just uses a different target pos
        if (mTargetObject)
        {
          mTargetPoint = mTargetObject->GetTranslation();
        }
      }
      case CameraType::TargetPoint:
      {
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
      case CameraType::Flyby:
      {
        glm::vec3 rot = mCameraTransform->GetRotationAsEuler();
        glm::mat4 rotationMatrix = glm::yawPitchRoll(rot.y, rot.x, rot.z);    // yaw pitch roll
        
        glm::vec4 up4(0.0f, 1.0f, 0.0f, 1.0f);
        up4 = rotationMatrix * up4;
        //up4 = glm::normalize(up4);

        glm::vec4 transVector(mMoveRight, mMoveUp, mZoom, 1.0f);
        transVector = rotationMatrix * transVector;

        // update camera translation
        mCameraTransform->SetTranslation(mCameraTransform->GetTranslation() + glm::vec3(transVector));

        glm::vec4 unitVector(0.0f, 0.0f, 1.0f, 1.0f); // used to translate away from target point by 1
        unitVector = rotationMatrix * unitVector;
        unitVector = glm::normalize(unitVector);

        // apply the new target position based on the camera translation
        mTargetPoint = mCameraTransform->GetTranslation() - glm::vec3(unitVector);

        view.mViewMatrix = glm::lookAt(mCameraTransform->GetTranslation(), mTargetPoint, glm::vec3(up4));

        mMoveUp = 0.0f;
        mMoveRight = 0.0f;
        mZoom = 0.0f;

        break;
      }
    } 
 
    mGraphicsView->UpdateView(this, view); 
  } 

  void Camera::UpdateCameraRotation(float aPitch, float aYaw, float aRoll)
  {
    // 0.1f is a generic number, we don't want it to be exactly half of pi, so we take some
    float pidiv2 = glm::half_pi<float>() - 0.1f; // used for later
    glm::vec3 rot = mCameraTransform->GetRotationAsEuler();

    // apply rotation
    rot.x += aPitch;
    rot.y += aYaw;
    rot.z += aRoll;

    // clamp pitch to half pi (nearly straight up and down)
    //if (rot.x >= pidiv2)
    //{
    //  rot.x = pidiv2;
    //}
    //else if (rot.x <= -pidiv2)
    //{
    //  rot.x = -pidiv2;
    //}

    // Note that the other two axis are free to move without issues, y axis is the only issue 

    mCameraTransform->SetRotation(rot);
  }

  void Camera::UpdateZoom(float aZoom)
  {
    mZoom += aZoom;

    if (mZoom >= mZoomMax)
    {
      mZoom = mZoomMax;
    }
    else if (mZoom <= mZoomMin)
    {
      mZoom = mZoomMin;
    }
  }
}
