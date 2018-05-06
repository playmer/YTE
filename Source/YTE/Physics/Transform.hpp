/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2015-10-16
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#ifndef YTE_Physics_Transform_h
#define YTE_Physics_Transform_h


#include "LinearMath/btMotionState.h"

#include "YTE/Core/Component.hpp"
#include "YTE/Core/Composition.hpp"

#include "YTE/Physics/ForwardDeclarations.hpp"

namespace YTE
{
  inline glm::quat AroundAxis(glm::vec3 aAxis, float aAngle)
  {
    float x = aAxis.x * glm::sin(aAngle / 2.0f);
    float y = aAxis.y * glm::sin(aAngle / 2.0f);
    float z = aAxis.z * glm::sin(aAngle / 2.0f);
    float w = glm::cos(aAngle / 2.0f);

    return glm::quat(w, x, y, z);
  }

  YTEDeclareEvent(PositionChanged);
  YTEDeclareEvent(RotationChanged);
  YTEDeclareEvent(ScaleChanged);

  class TransformChanged :public Event
  {
  public:
    YTEDeclareType(TransformChanged);

    glm::vec3 Position;
    glm::vec3 Scale;
    glm::quat Rotation;

    glm::vec3 WorldPosition;
    glm::vec3 WorldScale;
    glm::quat WorldRotation;

    glm::quat LocalRotationDifference;
    glm::quat WorldRotationDifference;
  };

  class MotionState : public btMotionState
  {
  public:
    MotionState(Transform *aTransform, bool kinematic = false)
      : mTransform(aTransform)
      , mKinematic(kinematic)
    {

    };

    ///synchronizes world transform from user to physics
    void getWorldTransform(btTransform& centerOfMassWorldTrans) const override;

    ///synchronizes world transform from physics to user
    ///Bullet only calls the update of world transform for active objects
    void setWorldTransform(const btTransform& centerOfMassWorldTrans) override;

    void SetKinematic(bool flag)
    {
      mKinematic = flag;
    }

    bool IsKinematic() const
    {
      return mKinematic;
    }

  private:
    Transform *mTransform;
    bool mKinematic;
  };

  class Transform : public Component
  {
  public:
    YTEDeclareType(Transform);

    YTE_Shared Transform(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    YTE_Shared void Initialize();

    //void NativeInitialize() override;

    // LOCAL TRANSFORM INFORMATION
    YTE_Shared const glm::vec3& GetTranslation() const;
    YTE_Shared void SetTranslation(const glm::vec3& aTrans);
    YTE_Shared void SetTranslation(float aX, float aY, float aZ);

    YTE_Shared const glm::vec3& GetScale() const;
    YTE_Shared void SetScale(const glm::vec3& aScale);
    YTE_Shared void SetScale(float aX, float aY, float aZ);

    YTE_Shared const glm::quat& GetRotation() const;
    YTE_Shared glm::vec3 GetRotationAsEuler() const;
    YTE_Shared glm::vec3 GetRotationAsEulerRadians() const;
    YTE_Shared void SetRotation(const glm::quat& aRotation);
    YTE_Shared void SetRotation(const glm::vec3& aEulerRot);
    YTE_Shared void SetRotation(float aThetaX, float aThetaY, float ThetaZ);

    YTE_Shared void RotateAboutLocalAxis(glm::vec3 aAxis, float aAngle);
    YTE_Shared void RotateAboutWorldAxis(const glm::vec3& aAxis, float aAngle);
    YTE_Shared void RotateToward(const glm::vec3& aTargetVector, const glm::vec3& aUpVector);
    YTE_Shared void RotateTowardPoint(const glm::vec3& aTargetPoint, const glm::vec3& aUpVector);
    YTE_Shared void RotateAround(glm::vec3 aAxis, float aAngle, glm::vec3 aPoint);

    // WORLD SPACE TRANSFORM INFORMATION
    YTE_Shared const glm::vec3& GetWorldTranslation() const;
    YTE_Shared void SetWorldTranslation(const glm::vec3& aTrans);
    YTE_Shared void SetWorldTranslation(float aX, float aY, float aZ);

    YTE_Shared const glm::vec3& GetWorldScale() const;
    YTE_Shared void SetWorldScale(const glm::vec3& aScale);
    YTE_Shared void SetWorldScale(float aX, float aY, float aZ);

    YTE_Shared const glm::quat& GetWorldRotation() const;
    YTE_Shared glm::vec3 GetWorldRotationAsEuler() const;
    YTE_Shared void SetWorldRotation(const glm::quat& aRotation);
    YTE_Shared void SetWorldRotation(const glm::vec3& aEulerRot);
    YTE_Shared void SetWorldRotation(float aThetaX, float aThetaY, float ThetaZ);

    void SetTranslationProperty(const glm::vec3& aTrans) { SetTranslation(aTrans); };
    void SetScaleProperty(const glm::vec3& aScale) { SetScale(aScale); };
    void SetRotationPropertyQuat(const glm::quat& aRot) { SetRotation(aRot); };
    void SetRotationProperty(const glm::vec3& aEulerRot) { SetRotation(aEulerRot); };

    void SetWorldTranslationProperty(const glm::vec3& aTrans) { SetWorldTranslation(aTrans); };
    void SetWorldScaleProperty(const glm::vec3& aScale) { SetWorldScale(aScale); };
    void SetWorldRotationProperty(const glm::vec3& aEulerRot) { SetWorldRotation(aEulerRot); };

    YTE_Shared void ParentPositionChanged(TransformChanged *aEvent);
    YTE_Shared void ParentScaleChanged(TransformChanged *aEvent);
    YTE_Shared void ParentRotationChanged(TransformChanged *aEvent);
    YTE_Shared void ParentObjectChanged(ParentChanged *aEvent);

    void SetInformPhysics(bool aInformPhysics)
    {
      mInformPhysics = aInformPhysics;
    }

    bool GetInformPhysics()
    {
      return mInformPhysics;
    }

    YTE_Shared glm::mat4 GetTransformMatrix();

  private:
    glm::vec3 GetAccumulatedParentTranslation();
    glm::vec3 GetAccumulatedParentScale();
    glm::quat GetAccumulatedParentRotation();

    void SetInternalTranslation(const glm::vec3 &aParentTranslation, const glm::vec3 &aLocalTranslation);
    void SetInternalScale(const glm::vec3 &aParentScale, const glm::vec3 &aLocalScale);
    void SetInternalRotation(const glm::quat &aParentRotation, const glm::quat &aLocalRotation);

    void SendTransformEvents(const std::string &aEvent,
                             glm::quat aLocalRotationDifference = glm::quat{},
                             glm::quat aWorldRotationDifference = glm::quat{});

    glm::vec3 mTranslation;
    glm::vec3 mScale;
    glm::quat mRotation;

    glm::vec3 mWorldTranslation;
    glm::vec3 mWorldScale;
    glm::quat mWorldRotation;

    bool mInformPhysics;
  };
}

#endif