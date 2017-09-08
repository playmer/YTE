#pragma once 
 
#ifndef YTE_Graphics_Camera_hpp 
#define YTE_Graphics_Camera_hpp 
 
#include "YTE/Core/Component.hpp" 
 
 
 
#include "YTE/Graphics/ForwardDeclarations.hpp" 
 
#include "YTE/Platform/ForwardDeclarations.hpp" 
 
#include "YTE/Physics/ForwardDeclarations.h" 
 
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
      Unknown 
    }; 
 
    DeclareType(Camera); 
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
 
    std::string& GetCameraType() 
    { 
      return mCameraType; 
    } 
 
    void RendererResize(WindowResizeEvent *aEvent); 
 
    void SetCameraType(std::string &aCameraType);
 
 
    void OrientationEvent(OrientationChanged *aEvent); 
    void PositionEvent(PositionChanged *aEvent); 
 
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
 
  private: 
    std::string mCameraType; 
    GraphicsView *mGraphicsView; 
 
    Transform *mCameraTransform; 
    Orientation *mCameraOrientation; 

    Mouse *mMouse;
 
    Window *mWindow; 
    Transform *mTargetObject; 
    glm::vec3 mTargetPoint; 
    glm::i32vec2 mMouseInitialPosition;
 
    float mFieldOfViewY; 
    float mNearPlane; 
    float mFarPlane; 

    float mTilt;
    float mTwist;
    float mSpin;
    float mZoom;
 
 
    bool mConstructing; 
    CameraType mType; 
  }; 
} 
 
#endif
