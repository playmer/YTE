#pragma once 
 
#ifndef YTE_Graphics_Camera_hpp 
#define YTE_Graphics_Camera_hpp 
 
#include "YTE/Core/Component.hpp" 

#include "YTE/Graphics/ForwardDeclarations.hpp" 
 
#include "YTE/Platform/ForwardDeclarations.hpp" 
 
#include "YTE/Physics/ForwardDeclarations.hpp" 
 
namespace YTE 
{ 
  class Camera : public Component 
  { 
  public: 
    enum class CameraType 
    { 
      TargetObject, 
      TargetPoint, 
      CameraOrientation, 
      Flyby,
      Unknown 
    }; 
 
    YTEDeclareType(Camera); 
    Camera(Composition *aOwner, Space *aSpace, RSValue *aProperties); 
 
    void Initialize() override; 
 
    void UpdateView(); 
 
    Window* GetWindow() 
    { 
      return mWindow; 
    } 


    void MousePress(MouseButtonEvent *aEvent);
    void MouseScroll(MouseWheelEvent *aEvent);
    void MouseMove(MouseMoveEvent *aEvent);
    void MousePersist(MouseButtonEvent *aEvent);
 
    std::string& GetCameraType() 
    { 
      return mCameraType; 
    } 
 
    void RendererResize(WindowResize *aEvent); 
 
    void SetCameraType(std::string &aCameraType);
 
 
    void OrientationEvent(OrientationChanged *aEvent); 
    void PositionEvent(TransformChanged *aEvent); 
 
    void SetNearPlane(float aNearPlane) 
    { 
      mNearPlane = aNearPlane; 
 
      if (false == mConstructing) 
      { 
        UpdateView(); 
      } 
    } 
 
    float GetNearPlane() 
    { 
      return mNearPlane; 
    } 
 
    void SetFarPlane(float aFarPlane) 
    { 
      mFarPlane = aFarPlane; 
 
      if (false == mConstructing) 
      { 
        UpdateView(); 
      } 
    } 
 
    float GetFarPlane() 
    { 
      return mFarPlane; 
    } 
 
    void SetFieldOfViewY(float aFieldOfViewY) 
    { 
      mFieldOfViewY = glm::radians(aFieldOfViewY); 
 
      if (false == mConstructing) 
      { 
        UpdateView(); 
      } 
    } 
 
    float GetFieldOfViewY() 
    { 
      return glm::degrees(mFieldOfViewY); 
    } 
 
    glm::vec3 GetTargetPoint() 
    { 
      return mTargetPoint; 
    } 
 
    void SetTargetPoint(glm::vec3 &aTargetPoint) 
    { 
      mTargetPoint = aTargetPoint; 
 
      if (false == mConstructing) 
      { 
        UpdateView(); 
      } 
    } 
 
    Transform* GetTargetObject() 
    { 
      return mTargetObject; 
    } 
 
    void SetTargetObject(Transform *aTargetObject) 
    { 
      mTargetObject = aTargetObject; 
 
      if (false == mConstructing) 
      { 
        UpdateView(); 
      } 
    } 

    void SetZoomingMaxAndMin(glm::vec2 aDist)
    {
      // prevents weirdness
      if (aDist.x > aDist.y)
      {
        mZoomMax = aDist.x;
        mZoomMin = aDist.y;
      }

      mZoomMax = aDist.y;
      mZoomMin = aDist.x;
    }

    glm::vec2 GetZoomingMaxAndMin() const
    {
      return glm::vec2(mZoomMin, mZoomMax);
    }

    void Update(LogicUpdate* aEvent);

  private:
    void UpdateCameraRotation(float aPitch, float aYaw, float aRoll);
    void UpdateZoom(float aZoom);
 
  private: 
    std::string mCameraType; 
    GraphicsView *mGraphicsView; 
 
    Transform *mCameraTransform; 
    Orientation *mCameraOrientation; 

    Mouse *mMouse;
    Keyboard *mKeyboard;
 
    Window *mWindow; 
    Transform *mTargetObject; 
    glm::vec3 mTargetPoint; 
    glm::i32vec2 mMouseInitialPosition;
 
    float mFieldOfViewY; 
    float mNearPlane; 
    float mFarPlane; 

    float mZoom;
    float mZoomMin;
    float mZoomMax;
    float mMoveUp;
    float mMoveRight;

    float mPitch;
    float mYaw;
    float mRoll;

    float mDt;
 
 
    bool mConstructing; 
    CameraType mType; 
  }; 
} 
 
#endif
