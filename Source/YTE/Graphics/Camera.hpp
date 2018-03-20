#pragma once 
 
#ifndef YTE_Graphics_Camera_hpp 
#define YTE_Graphics_Camera_hpp 
 
#include "YTE/Core/Component.hpp" 
#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/UBOs.hpp"
 
#include "YTE/Platform/ForwardDeclarations.hpp" 
 
#include "YTE/Physics/ForwardDeclarations.hpp" 
 
namespace YTE 
{
  class Camera : public Component 
  { 
  public:
    YTEDeclareType(Camera); 
    Camera(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override; 
    UBOView ConstructUBOView();
 
    Window* GetWindow()
    { 
      return mWindow; 
    }

    ///////////////////////////////////////
    // Events
    ///////////////////////////////////////
    void OrientationEvent(OrientationChanged *aEvent);
    void TransformEvent(TransformChanged *aEvent);
    void Update(LogicUpdate* aEvent);
    void RendererResize(WindowResize *aEvent);
    void SurfaceGainedEvent(ViewChanged *aEvent);


    ///////////////////////////////////////
    // Getters
    ///////////////////////////////////////
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

    bool GetUseOrtho() const
    {
      return mUseOrthographicProj;
    }

    ///////////////////////////////////////
    // Setters
    ///////////////////////////////////////
    void SetNearPlane(float aNearPlane) 
    { 
      mNearPlane = aNearPlane; 
 
      mIllumination.mFogPlanes = glm::vec2(mFarPlane / 2.0f, mFarPlane);
    } 
    
    void SetFarPlane(float aFarPlane)
    { 
      mFarPlane = aFarPlane; 
 
      mIllumination.mFogPlanes = glm::vec2(mFarPlane / 2.0f, mFarPlane);
    } 
    
    void SetFieldOfViewY(float aFieldOfViewY)
    { 
      mFieldOfViewY = glm::radians(aFieldOfViewY); 
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

    void SetUseOrtho(bool aUseOrtho)
    {
      mUseOrthographicProj = aUseOrtho;
    }

    void SetCameraAsActive();

  private: 
    // YTE
    Engine *mEngine;
    GraphicsView *mGraphicsView; 
    Transform *mCameraTransform; 
    Orientation *mCameraOrientation; 
    Window *mWindow; 

    // Perspective
    float mFieldOfViewY; 
    float mNearPlane; 
    float mFarPlane; 
    bool mUseOrthographicProj;

    // Utilities
    double mDt;
    bool mConstructing; 
    bool mChanged;

    UBOIllumination mIllumination;
  }; 
} 
 
#endif
