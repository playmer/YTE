#pragma once 
 
#ifndef YTE_Graphics_Camera_hpp 
#define YTE_Graphics_Camera_hpp 
 
#include "YTE/Core/Component.hpp" 
#include "YTE/Core/ForwardDeclarations.hpp"

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

    // Component, Ctor and YTE stuff
    YTEDeclareType(Camera); 
    Camera(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override; 
    UBOView&& ConstructUBOView();
    void UpdateView();
 
    Window* GetWindow()
    { 
      return mWindow; 
    }

    ///////////////////////////////////////
    // Events
    ///////////////////////////////////////
    void MousePress(MouseButtonEvent *aEvent);
    void MouseScroll(MouseWheelEvent *aEvent);
    void MouseMove(MouseMoveEvent *aEvent);
    void MousePersist(MouseButtonEvent *aEvent);
    void OrientationEvent(OrientationChanged *aEvent);
    void Update(GraphicsDataUpdate* aEvent);
    void RendererResize(WindowResize *aEvent);


    ///////////////////////////////////////
    // Gettors
    ///////////////////////////////////////
    std::string& GetCameraType()
    { 
      return mCameraType; 
    } 
    
    float GetNearPlane()
    {
      return mNearPlane;
    }

    float GetFarPlane()
    {
      return mFarPlane;
    }
    
    float GetFieldOfViewY()
    {
      return glm::degrees(mFieldOfViewY);
    }
    
    glm::vec3 GetTargetPoint()
    {
      return mTargetPoint;
    }
    
    Transform* GetTargetObject()
    {
      return mTargetObject;
    }
    
    glm::vec2 GetZoomingMaxAndMin() const
    {
      return glm::vec2(mZoomMin, mZoomMax);
    }

    float GetPanSpeed() const
    {
      return mPanSpeed;
    }

    float GetRotateSpeed() const
    {
      return mRotateSpeed;
    }

    float GetMoveSpeed() const
    {
      return mMoveSpeed;
    }

    float GetScrollSpeed() const
    {
      return mScrollSpeed;
    }

    ///////////////////////////////////////
    // Settors
    ///////////////////////////////////////
    void SetNearPlane(float aNearPlane) 
    { 
      mNearPlane = aNearPlane; 
 
      if (false == mConstructing) 
      { 
        UpdateView(); 
      } 
    } 
    
    void SetFarPlane(float aFarPlane)
    { 
      mFarPlane = aFarPlane; 
 
      if (false == mConstructing) 
      { 
        UpdateView(); 
      } 
    } 
    
    void SetFieldOfViewY(float aFieldOfViewY)
    { 
      mFieldOfViewY = glm::radians(aFieldOfViewY); 
 
      if (false == mConstructing) 
      { 
        UpdateView(); 
      } 
    } 
    
    void SetTargetPoint(glm::vec3 &aTargetPoint)
    { 
      mTargetPoint = aTargetPoint; 
 
      if (false == mConstructing) 
      { 
        UpdateView(); 
      } 
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

    void SetPanSpeed(float aPanSpeed)  
    {
      mPanSpeed = aPanSpeed;
    }

    void SetRotateSpeed(float aRotateSpeed)  
    {
      mRotateSpeed = aRotateSpeed;
    }

    void SetMoveSpeed(float aMoveSpeed)  
    {
      mMoveSpeed = aMoveSpeed;
    }

    void SetScrollSpeed(float aScrollSpeed)  
    {
      mScrollSpeed = aScrollSpeed;
    }

    void SetCameraType(std::string &aCameraType);


  private:
    void UpdateCameraRotation(float aPitch, float aYaw, float aRoll);
    void UpdateZoom(float aZoom);
    void ToTargetCamera(bool aUseTarget);
    void ToFlybyCamera();

  private: 
    // YTE
    Engine *mEngine;
    GraphicsView *mGraphicsView; 
    Transform *mCameraTransform; 
    Orientation *mCameraOrientation; 
    Mouse *mMouse;
    Keyboard *mKeyboard;
    Window *mWindow; 
    Transform *mTargetObject; 

    // Other object Related
    glm::vec3 mTargetPoint; 
    glm::i32vec2 mMouseInitialPosition;
   
    // Perspective
    float mFieldOfViewY; 
    float mNearPlane; 
    float mFarPlane; 

    // Movement
    float mZoom;
    float mZoomMin;
    float mZoomMax;
    float mMoveUp;
    float mMoveRight;

    // Rotation
    float mPitch;
    float mYaw;
    float mRoll;

    // Utilities
    float mDt;
    std::string mCameraType;
    bool mConstructing; 
    CameraType mType; 
    bool mChanged;

    // speeds
    float mPanSpeed;
    float mMoveSpeed;
    float mScrollSpeed;
    float mRotateSpeed;
  }; 
} 
 
#endif
