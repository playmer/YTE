#include <glm/gtx/euler_angles.hpp>

#include "YTE/Core/Engine.hpp" 
#include "YTE/Core/Space.hpp" 
#include "YTE/Core/Utilities.hpp" 
 
#include "YTE/Graphics/Camera.hpp" 
#include "YTE/Graphics/GraphicsView.hpp" 
#include "YTE/Graphics/GraphicsSystem.hpp" 
 
#include "YTE/Physics/Orientation.hpp"
#include "YTE/Physics/Transform.hpp"

 
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
      .SetDocumentation("Point the camera will point at if it's type is set to \"TargetPoint\"."); 
 
    YTEBindProperty(&Camera::GetFarPlane, &Camera::SetFarPlane, "FarPlane") 
      .AddAttribute<EditorProperty>() 
      .SetDocumentation("The far plane the view will be rendered with."); 
 
    YTEBindProperty(&Camera::GetNearPlane, &Camera::SetNearPlane, "NearPlane") 
      .AddAttribute<EditorProperty>() 
      .SetDocumentation("The near plane the view will be rendered with."); 
 
    YTEBindProperty(&Camera::GetFieldOfViewY, &Camera::SetFieldOfViewY, "FieldOfViewY") 
      .AddAttribute<EditorProperty>() 
      .SetDocumentation("The field of view (y) the view will be rendered with."); 

    YTEBindProperty(&Camera::GetZoomingMaxAndMin, &Camera::SetZoomingMaxAndMin, "Zoom Planes")
      .AddAttribute<EditorProperty>()
      .SetDocumentation("The min (x) and max (y) zoom of the camera.");
 
    YTEBindProperty(&Camera::GetCameraType, &Camera::SetCameraType, "CameraType") 
      .AddAttribute<EditorProperty>() 
      .AddAttribute<DropDownStrings>(PopulateDropDownList) 
      .SetDocumentation("The type of camera we'd like to have. Options include:\n" 
                        " - TargetObject: The camera will always face the chosen target object.\n" 
                        " - TargetPoint: The camera will always face the chosen target point.\n" 
                        " - CameraOrientation: The camera will always face in the direction the orientation component suggests.\n"
                        " - Flyby: The camera behaves like an FPS, where it uses WASD."); 
  } 
   
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
    , mTilt(0.0f)
    , mTwist(0.0f)
    , mSpin(0.0f)
    , mZoom(5.0f)
    , mZoomMin(2.0f)
    , mZoomMax(30.0f)
    , mMoveUp(0.0f)
    , mMoveRight(0.0f)
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
 
    mCameraTransform = mOwner->GetComponent<Transform>(); 
    mCameraOrientation = mOwner->GetComponent<Orientation>(); 

    mTargetPoint = glm::vec3(0.0f, 0.0f, 0.0f);
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
        glm::mat4 rotationMatrix = glm::yawPitchRoll(-mSpin, -mTilt, 0.0f);
        glm::vec4 transVector(0.0f, 0.0f, mZoomMin, 1.0f);
        transVector = rotationMatrix * transVector;
        mTargetPoint = mCameraTransform->GetWorldTranslation() - glm::vec3(transVector);

        mSpin += glm::pi<float>();
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
      if (mType == CameraType::TargetObject || mType == CameraType::TargetPoint)
      {
        glm::mat4 rotationMatrix = glm::yawPitchRoll(-mSpin, mTilt, 0.0f);

        glm::vec4 transVector(0.0f, 0.0f, mZoom, 1.0f); // move forward one unit
        transVector = rotationMatrix * transVector;

        mTargetPoint = mCameraTransform->GetWorldTranslation() - glm::vec3(transVector);

        mZoom = 0.0f;
        mMoveUp = 0.0f;
        mMoveRight = 0.0f;
        mSpin -= glm::pi<float>();
      }

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
      const float moveSpeed = 0.05f;

      if (mKeyboard->IsKeyDown(Keys::W))
      {
        mZoom = -moveSpeed;
      }
      else if (mKeyboard->IsKeyDown(Keys::S))
      {
        mZoom = moveSpeed;
      }
      if (mKeyboard->IsKeyDown(Keys::A))
      {
        mMoveRight = moveSpeed;
      }
      else if (mKeyboard->IsKeyDown(Keys::D))
      {
        mMoveRight = -moveSpeed;
      }
  
      UpdateView();
    }
  }

  // Handle the moment a mouse button is pressed
  void Camera::MouseScroll(MouseWheelEvent *aEvent)
  {
    const float zoomSpeed = 1.06f;

    switch (mType)
    {
      case CameraType::TargetObject:
      case CameraType::TargetPoint:
      {
        UpdateZoom(-(aEvent->ScrollMovement.y * zoomSpeed));
        break;
      }
      case CameraType::Flyby:
      {
        // modify mZoom directly since this is a movement, not a zooming motion
        mZoom = -(aEvent->ScrollMovement.y * zoomSpeed);
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

        const float rotationSpeed = 0.01f;

        if (mMouse->IsButtonDown(Mouse_Buttons::Left))
        {
          // Left Mouse does Arc ball rotation
          UpdateCameraRotation(dy * rotationSpeed, 0.0f, -(dx * rotationSpeed));
        }

        if (mMouse->IsButtonDown(Mouse_Buttons::Right))
        {
          // Right Mouse does Zoom
          const float zoomSpeed = 0.01f;  // note different zoom speed since the mouse move is faster
          UpdateZoom(dy * zoomSpeed);
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

          mMoveUp = dy * rotationSpeed;
          mMoveRight = dx * rotationSpeed;
        }

        break;
      }
      case CameraType::Flyby:
      {
        const float rotationSpeed = 0.005f; // movement here is too fast, so it has a different speed

        // allows look and move with WASD
        if (mMouse->IsButtonDown(Mouse_Buttons::Right))
        {
          UpdateCameraRotation(dy * rotationSpeed, 0.0f, -(dx * rotationSpeed));
        }

        // allows panning
        if (mMouse->IsButtonDown(Mouse_Buttons::Middle))
        {
          mMoveUp = dy * rotationSpeed * 2.0f;
          mMoveRight = dx * rotationSpeed * 2.0f;
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
    UpdateView(); 
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
        // NOTE this math doesnt allow for 3 axis rotation, only 2 axis

        // create the rotation matrix, note that glm::yawpitchroll does the same thing
        //glm::mat4 rotationMatrix = glm::mat4(1.0);
        //rotationMatrix = glm::rotate(rotationMatrix, -mSpin, glm::vec3(0.0, 1.0, 0.0));
        //rotationMatrix = glm::rotate(rotationMatrix, mTilt, glm::vec3(1.0, 0.0, 0.0));
        glm::mat4 rotationMatrix = glm::yawPitchRoll(-mSpin, mTilt, 0.0f);

        // create the up vector, and then rotate it. Note that it is -1 to flip the world right side up
        glm::vec4 upReal(0.0f, -1.0f, 0.0f, 1.0f);
        upReal = rotationMatrix * upReal;

        // This is the vector that will transplate the camera
        glm::vec4 transVector(-mMoveRight, mMoveUp, 0.0f, 1.0f);
        transVector = rotationMatrix * transVector;

        // now we need a rotated zoom vector
        glm::vec4 rotatedZoom = rotationMatrix * glm::vec4(0.0f, 0.0f, -mZoom, 1.0f);

        // change the target we are looking at to be in line with the new translation
        mTargetPoint = mTargetPoint + glm::vec3(transVector);

        // apply the zoom to the target point to get the camera location
        glm::vec4 cameraPosition4 = glm::vec4(mTargetPoint, 1.0f) + rotatedZoom;

        // reset the data to the transform component
        // note that the camera position is not saved to the transform, rather where the camera
        // is rotating around is saved. This is more intuitive
        mCameraTransform->SetWorldTranslation(mTargetPoint);
        
        // finally get the view matrix
        view.mViewMatrix = glm::lookAt(glm::vec3(cameraPosition4), mTargetPoint, glm::vec3(upReal));

        // reset for no translation next frame
        mMoveUp = 0.0f;
        mMoveRight = 0.0f;

        break;
      }
      case CameraType::Flyby:
      {
        glm::mat4 rotationMatrix = glm::yawPitchRoll(-mSpin, -mTilt, 0.0f);
        
        glm::vec4 up4(0.0f, -1.0f, 0.0f, 1.0f);
        up4 = rotationMatrix * up4;

        glm::vec4 transVector(mMoveRight, mMoveUp, mZoom, 1.0f);
        transVector = rotationMatrix * transVector;

        mTargetPoint = mTargetPoint + glm::vec3(transVector);

        glm::vec4 unitVector(0.0f, 0.0f, 1.0f, 1.0f); // used to translate away from target point by 1
        unitVector = rotationMatrix * unitVector;
        unitVector = glm::normalize(unitVector);

        glm::vec4 cameraPos4 = glm::vec4(mTargetPoint, 1.0f) + unitVector;
        
        mCameraTransform->SetWorldTranslation(glm::vec3(cameraPos4));

        view.mViewMatrix = glm::lookAt(glm::vec3(cameraPos4), mTargetPoint, glm::vec3(up4));

        mMoveUp = 0.0f;
        mMoveRight = 0.0f;
        mZoom = 0.0f;

        break;
      }
    } 
 
    mGraphicsView->UpdateView(view); 
  } 

  void Camera::UpdateCameraRotation(float aTilt, float aTwist, float aSpin)
  {
    // 0.1f is a generic number, we dont want it to be exactly half of pi, so we take some
    float pidiv2 = glm::half_pi<float>() - 0.1f; // used for later

    // apply rotation
    mTilt += aTilt;
    mTwist += aTwist;
    mSpin += aSpin;

    // clamp tilt (y axis) to half pi (nearly straight up and down)
    if (mTilt >= pidiv2)
    {
      mTilt = pidiv2;
    }
    else if (mTilt <= -pidiv2)
    {
      mTilt = -pidiv2;
    }

    // Note that the other two axis are free to move without issues, y axis is the only issue 
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
