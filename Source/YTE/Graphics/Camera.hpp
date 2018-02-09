#pragma once 
 
#ifndef YTE_Graphics_Camera_hpp 
#define YTE_Graphics_Camera_hpp 
 
#include "YTE/Core/Component.hpp" 
#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/UBOs.hpp"
 
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
      Gameplay,
      Unknown 
    };

    // Component, Ctor and YTE stuff
    YTEDeclareType(Camera); 
    Camera(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override; 
    UBOView ConstructUBOView();
    void UpdateView();
 
    Window* GetWindow()
    { 
      return mWindow; 
    }

    void SetUBOView(UBOView &aView);

    ///////////////////////////////////////
    // Events
    ///////////////////////////////////////
    void MousePress(MouseButtonEvent *aEvent);
    void MouseScroll(MouseWheelEvent *aEvent);
    void MouseMove(MouseMoveEvent *aEvent);
    void MousePersist(MouseButtonEvent *aEvent);
    void MouseRelease(MouseButtonEvent *aEvent);
    void OrientationEvent(OrientationChanged *aEvent);
    void Update(LogicUpdate* aEvent);
    void RendererResize(WindowResize *aEvent);


    ///////////////////////////////////////
    // Getters
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

    glm::vec3 GetGlobalIlluminaton() const
    {
      return glm::vec3(mIllumination.mGlobalIllumination);
    }

    glm::vec3 GetFogCoeffs() const
    {
      return glm::vec3(mIllumination.mFogCoefficients);
    }

    glm::vec3 GetFogColor() const
    {
      return glm::vec3(mIllumination.mFogColor);
    }

    glm::vec2 GetFogPlanes() const
    {
      return mIllumination.mFogPlanes;
    }

    ///////////////////////////////////////
    // Setters
    ///////////////////////////////////////
    void SetNearPlane(float aNearPlane) 
    { 
      mNearPlane = aNearPlane; 
 
      mIllumination.mFogPlanes = glm::vec2(mFarPlane / 2.0f, mFarPlane);

      if (false == mConstructing)
      { 
        UpdateView(); 
      } 
    } 
    
    void SetFarPlane(float aFarPlane)
    { 
      mFarPlane = aFarPlane; 
 
      mIllumination.mFogPlanes = glm::vec2(mFarPlane / 2.0f, mFarPlane);

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
    
    void SetTargetPoint(glm::vec3 aTargetPoint)
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

    void SetGlobalIlluminaton(glm::vec3 aGlobalIllum)
    {
      mIllumination.mGlobalIllumination = glm::vec4(aGlobalIllum, 1.0f);
    }

    void SetFogCoeffs(glm::vec3 aFogCoeffs)
    {
      mIllumination.mFogCoefficients = glm::vec4(aFogCoeffs, 0.0f);
    }

    void SetFogColor(glm::vec3 aColor)
    {
      mIllumination.mFogColor = glm::vec4(aColor, 1.0f);
    }

    void SetFogPlanes(glm::vec2 aFogPlanes)
    {
      mIllumination.mFogPlanes = aFogPlanes;
    }

    void SetCameraType(std::string &aCameraType);

    void SetCameraAsActive();


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
    float mSpeedLimiter;

    UBOIllumination mIllumination;
  }; 
} 
 
#endif
