#include <glm/gtx/euler_angles.hpp>

#include "YTE/Core/Engine.hpp" 
#include "YTE/Core/Space.hpp" 
#include "YTE/Core/Utilities.hpp" 
 
#include "YTE/Event/StandardEvents.h" 
 
#include "YTE/Graphics/Camera.hpp" 
#include "YTE/Graphics/GraphicsView.hpp" 
#include "YTE/Graphics/GraphicsSystem.hpp" 
 
#include "YTE/Physics/Orientation.h" 
#include "YTE/Physics/Transform.h" 


// Andrew Griffin Notes
// 
 
namespace YTE 
{ 
  static std::vector<std::string> PopulateDropDownList(Component *aComponent) 
  { 
    return { "TargetObject", "TargetPoint", "CameraOrientation" }; 
  } 
   
  DefineType(Camera) 
  { 
    YTERegisterType(Camera); 

    std::vector<std::vector<Type*>> deps = { { Transform::GetStaticType() }, 
                                             { Orientation::GetStaticType() } };

    Camera::GetStaticType()->AddAttribute<ComponentDependencies>(deps);
 
    YTEBindProperty(&Camera::GetTargetObject, &Camera::SetTargetObject, "TargetObject") 
      ->SetDocumentation("Object the camera will point at if it's type is set to \"TargetObject\"."); 
 
    YTEBindProperty(&Camera::GetTargetPoint, &Camera::SetTargetPoint, "TargetPoint") 
      ->AddAttribute<EditorProperty>() 
      .SetDocumentation("Point the camera will point at if it's type is set to \"TargetPoint\"."); 
 
    YTEBindProperty(&Camera::GetFarPlane, &Camera::SetFarPlane, "FarPlane") 
      ->AddAttribute<EditorProperty>() 
      .SetDocumentation("The far plane the view will be rendered with."); 
 
    YTEBindProperty(&Camera::GetNearPlane, &Camera::SetNearPlane, "NearPlane") 
      ->AddAttribute<EditorProperty>() 
      .SetDocumentation("The near plane the view will be rendered with."); 
 
    YTEBindProperty(&Camera::GetFieldOfViewY, &Camera::SetFieldOfViewY, "FieldOfViewY") 
      ->AddAttribute<EditorProperty>() 
      .SetDocumentation("The field of view (y) the view will be rendered with."); 

    YTEBindProperty(&Camera::GetZoomingMaxAndMin, &Camera::SetZoomingMaxAndMin, "Zoom Planes")
      ->AddAttribute<EditorProperty>()
      .SetDocumentation("The min (x) and max (y) zoom of the camera.");
 
    YTEBindProperty(&Camera::GetCameraType, &Camera::SetCameraType, "CameraType") 
      ->AddAttribute<EditorProperty>() 
      .AddAttribute<DropDownStrings>(PopulateDropDownList) 
      .SetDocumentation("The type of camera we'd like to have. Options include:\n" 
                        " - TargetObject: The camera will always face the chosen target object.\n" 
                        " - TargetPoint: The camera will always face the chosen target point.\n" 
                        " - CameraOrientation: The camera will always face in the direction the orientation component suggests."); 
  } 
   
  Camera::Camera(Composition *aOwner, 
                 Space *aSpace,  
                 RSValue *aProperties) 
    : Component(aOwner, aSpace), 
      mCameraTransform(nullptr), 
      mCameraOrientation(nullptr), 
      mTargetObject(nullptr), 
      mFieldOfViewY(glm::radians(45.0f)), 
      mNearPlane(0.1f), 
      mFarPlane(256.0f), 
      mTilt(0.0f),
      mTwist(0.0f),
      mSpin(0.0f),
      mZoom(5.0f),
      mZoomMin(2.0f),
      mZoomMax(30.0f),
      mConstructing(true), 
      mType(CameraType::CameraOrientation) 
  { 
    auto engine = aSpace->GetEngine(); 
    DeserializeByType<Camera*>(aProperties, this, Camera::GetStaticType()); 
 
    mGraphicsView = mSpace->GetComponent<GraphicsView>(); 
 
    mWindow = mGraphicsView->GetWindow(); 
    mWindow->RegisterListener(Events::RendererResize, *this, &Camera::RendererResize); 
    mConstructing = false; 

    mTargetPoint = glm::vec3(0.0f, 0.0f, 0.0f);
  } 
 
  void Camera::Initialize() 
  { 
    mOwner->RegisterListener(Events::PositionChanged, *this, &Camera::PositionEvent); 
    mOwner->RegisterListener(Events::OrientationChanged, *this, &Camera::OrientationEvent); 

    mMouse = &mWindow->mMouse;
    mKeyboard = &mWindow->mKeyboard;

    mMouse->RegisterListener(Events::MouseMove, *this, &Camera::MouseMove);
    mMouse->RegisterListener(Events::MouseScroll, *this, &Camera::MouseScroll);
    mMouse->RegisterListener(Events::MousePress, *this, &Camera::MousePress);
 
    mCameraTransform = mOwner->GetComponent<Transform>(); 
    mCameraOrientation = mOwner->GetComponent<Orientation>(); 
  } 


  void Camera::SetCameraType(std::string &aCameraType)
  {
    if ((false == (aCameraType != "TargetObject")) &&
        (false == (aCameraType != "TargetPoint")) &&
        (false == (aCameraType != "CameraOrientation")))
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

  // Handle the moment a mouse button is pressed
  void Camera::MouseScroll(MouseWheelEvent *aEvent)
  {
    const float zoomSpeed = 1.06f;

    UpdateZoom(-(aEvent->ScrollMovement.y * zoomSpeed));

    UpdateView();
  }

  // Handle movement of the mouse. Should be fairly clear by code alone.
  void Camera::MouseMove(MouseMoveEvent *aEvent)
  {
    // if the alt key is not being pressed, then there is no camera movement
    // note this only works because the scroll event is a different function
    // the scroll wheel must zoom without pressing the alt key
    if (mKeyboard->IsKeyDown(Keys::Alt) == false)
    {
      return;
    }

    const float rotationSpeed = 0.01f;
    
    auto dx = aEvent->WorldCoordinates.x - mMouseInitialPosition.x;
    auto dy = aEvent->WorldCoordinates.y - mMouseInitialPosition.y;

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
    }


    mMouseInitialPosition.x = aEvent->WorldCoordinates.x;
    mMouseInitialPosition.y = aEvent->WorldCoordinates.y;

    UpdateView();
  }
 
  void Camera::OrientationEvent(OrientationChanged *aEvent) 
  { 
    UpdateView(); 
  } 
 
  void Camera::PositionEvent(PositionChanged *aEvent) 
  { 
    UpdateView(); 
  } 
 
  void Camera::RendererResize(WindowResizeEvent *aEvent) 
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

        glm::mat4 rotationMatrix = glm::mat4(1.0);

        //rotationMatrix = glm::yawPitchRoll(mSpin, mTilt, 0.0f);

        rotationMatrix = glm::rotate(rotationMatrix, -mSpin, glm::vec3(0.0, 1.0, 0.0));
        rotationMatrix = glm::rotate(rotationMatrix, mTilt, glm::vec3(1.0, 0.0, 0.0));

        glm::vec4 upReal(0.0f, -1.0f, 0.0f, 1.0f);
        upReal = rotationMatrix * upReal;

        glm::vec4 rotatedZoom = rotationMatrix * glm::vec4(0.0f, 0.0f, -mZoom, 1.0f);

        glm::vec4 cameraPosition4 = glm::vec4(mTargetPoint, 1.0f) + rotatedZoom;

        mCameraTransform->SetWorldTranslation(mTargetPoint);
        
        view.mViewMatrix = glm::lookAt(glm::vec3(cameraPosition4), mTargetPoint, glm::vec3(upReal));

        break;
      }
    } 
 
    mGraphicsView->UpdateView(view); 
  } 

  void Camera::UpdateCameraRotation(float aTilt, float aTwist, float aSpin)
  {
    //! 0.1f is a generic number, we dont want it to be exactly half of pi, so we take some
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
