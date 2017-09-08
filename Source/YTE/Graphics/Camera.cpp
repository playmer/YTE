#include <glm/gtx/euler_angles.hpp>

#include "YTE/Core/Engine.hpp" 
#include "YTE/Core/Space.hpp" 
#include "YTE/Core/Utilities.hpp" 
 
 
 
#include "YTE/Graphics/Camera.hpp" 
#include "YTE/Graphics/GraphicsView.hpp" 
#include "YTE/Graphics/GraphicsSystem.hpp" 
 
#include "YTE/Physics/Orientation.h" 
#include "YTE/Physics/Transform.h" 
 
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
      mConstructing(true), 
      mType(CameraType::CameraOrientation) 
  { 
    auto engine = aSpace->GetEngine(); 
    DeserializeByType<Camera*>(aProperties, this, Camera::GetStaticType()); 
 
    mGraphicsView = mSpace->GetComponent<GraphicsView>(); 
 
    mWindow = mGraphicsView->GetWindow(); 
    mWindow->CONNECT(Events::RendererResize, *this, &Camera::RendererResize); 
    mConstructing = false; 
  } 
 
  void Camera::Initialize() 
  { 
    mOwner->CONNECT(Events::PositionChanged, *this, &Camera::PositionEvent); 
    mOwner->CONNECT(Events::OrientationChanged, *this, &Camera::OrientationEvent); 

    mMouse = &mWindow->mMouse;

    mMouse->CONNECT(Events::MouseMove, *this, &Camera::MouseMove);
    mMouse->CONNECT(Events::MouseScroll, *this, &Camera::MouseScroll);
    mMouse->CONNECT(Events::MousePress, *this, &Camera::MousePress);
 
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
    const float zoomSpeed = 1.06f;
    mMouseInitialPosition = aEvent->WorldCoordinates;
  }

  // Handle the moment a mouse button is pressed
  void Camera::MouseScroll(MouseWheelEvent *aEvent)
  {
    const float zoomSpeed = 1.06f;

    //mMouseInitialPosition = aEvent->WorldCoordinates;

    //auto current = mCameraTransform->GetTranslation();
    //current.z += aEvent->ScrollMovement.y * zoomSpeed;
    //mCameraTransform->SetTranslation(current);

    mZoom += aEvent->ScrollMovement.y * zoomSpeed;

    UpdateView();
  }

  // Handle movement of the mouse. Should be fairly clear by code alone.
  void Camera::MouseMove(MouseMoveEvent *aEvent)
  {
    const float rotationSpeed = 0.01f;
    
    auto dx = aEvent->WorldCoordinates.x - mMouseInitialPosition.x;
    auto dy = aEvent->WorldCoordinates.y - mMouseInitialPosition.y;

    if (mMouse->IsButtonDown(Mouse_Buttons::Left))
    {
      mTilt -= dy * rotationSpeed;
      mSpin += dx * rotationSpeed;

      // For assignment 2 we must limit tilt.
      //if (CameraType::CameraOrientation == mType)
      //{
      //  if (mTilt < -glm::pi<float>())
      //  {
      //    mTilt = -glm::pi<float>();
      //  }
      //  else if (mTilt > 0.0f)
      //  {
      //    mTilt = 0.0f;
      //  }
      //}
    }

    if (mMouse->IsButtonDown(Mouse_Buttons::Right))
    {
      mTilt += dy * rotationSpeed;
      mTwist -= dx * rotationSpeed;
    }

    //auto position = glm::vec4(mCameraTransform->GetTranslation(), 1.0f);
    //auto rotation = mCameraTransform->GetRotation();
    //
    //glm::vec4 target;
    //
    //switch (mType)
    //{
    //  case CameraType::TargetPoint:
    //  {
    //    target = glm::vec4(mTargetPoint, 1.0f);;
    //    break;
    //  }
    //  case CameraType::TargetObject:
    //  {
    //    target = glm::vec4(mTargetObject->GetTranslation(), 1.0f);;;
    //    break;
    //  }
    //}

    if (mMouse->IsButtonDown(Mouse_Buttons::Middle))
    {
      mTilt += dy * rotationSpeed;
      mSpin -= dx * rotationSpeed;
    }


    ////pitch
    //position = (glm::rotate(glm::mat4(), pitch, mCameraOrientation->GetRightVector()) * (position - target)) + target;
    //
    ////yaw, Y-up system
    //position = (glm::rotate(glm::mat4(), yaw, { 0, 1, 0 }) * (position - target)) + target;


    //if (mMouse->IsButtonDown(Mouse_Buttons::Left))
    //{
    //  rotation += glm::quat(pitch * mCameraOrientation->GetRightVector());
    //  rotation += glm::quat(yaw * glm::vec3{ 0, 1, 0 });
    //  mCameraTransform->SetRotation(rotation);
    //}

    //mCameraTransform->SetTranslation(glm::vec3(position));

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

    glm::vec4 rotation;
    rotation.x = mTilt;
    rotation.y = mTwist;
    rotation.z = mSpin;


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
      case CameraType::TargetPoint: 
      { 
        //view.mViewMatrix = glm::translate(glm::mat4(), mTargetPoint) * Rotate(rotation);
        //view.mViewMatrix = glm::lookAt(position, mTargetPoint, glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 rotationMatrix = glm::mat4(1.0);

        rotationMatrix = glm::rotate(rotationMatrix, mSpin, glm::vec3(0.0, 1.0, 0.0));
        rotationMatrix = glm::rotate(rotationMatrix, mTilt, glm::vec3(1.0, 0.0, 0.0));

        const glm::vec4 cameraPosition4 = rotationMatrix * glm::vec4(mTargetPoint, 1.0);
        const glm::vec3 camPos = mZoom * glm::vec3(cameraPosition4);

        //printf("Spin: %f, Tilt: %f\n", mSpin, mTilt);
        //printf("x: %f, y: %f, z: %f\n\n", camPos.x, camPos.y, camPos.z);

        view.mViewMatrix = glm::lookAt(camPos, mTargetPoint, glm::vec3(0.0, 1.0, 0.0));
        break; 
      } 
      case CameraType::TargetObject: 
      { 
        glm::vec3 targetPos; 
 
        if (mTargetObject) 
        { 
          targetPos = mTargetObject->GetTranslation(); 
        } 

        //auto arcball = glm::yawPitchRoll(mSpin, mTilt, 0.0f);

        //view.mViewMatrix = glm::translate(glm::mat4(), targetPos) * Rotate(rotation);
        //view.mViewMatrix = glm::lookAt(position, targetPos, glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 rotationMatrix = glm::mat4(1.0);

        rotationMatrix = glm::rotate(rotationMatrix, mSpin, glm::vec3(0.0, 1.0, 0.0));
        rotationMatrix = glm::rotate(rotationMatrix, mTilt, glm::vec3(1.0, 0.0, 0.0));

        const glm::vec4 cameraPosition4 = rotationMatrix * glm::vec4(0.0, 0.0, 1.0, 1.0);
        const glm::vec3 cameraPositionFinal = 1.0f * glm::vec3(cameraPosition4);

        view.mViewMatrix = glm::lookAt(cameraPositionFinal, targetPos, glm::vec3(0.0, 1.0, 0.0));
        break; 
      } 
    } 
 
    mGraphicsView->UpdateView(view); 
  } 
}
