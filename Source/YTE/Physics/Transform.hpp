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

    Transform(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize();

    //void NativeInitialize() override;

    // LOCAL TRANSFORM INFORMATION
    const glm::vec3& GetTranslation() const;
    void SetTranslation(const glm::vec3& aTrans);
    void SetTranslation(float aX, float aY, float aZ);

    const glm::vec3& GetScale() const;
    void SetScale(const glm::vec3& aScale);
    void SetScale(float aX, float aY, float aZ);

    const glm::quat& GetRotation() const;
    glm::vec3 GetRotationAsEuler() const;
    glm::vec3 GetRotationAsEulerRadians() const;
    void SetRotation(const glm::quat& aRotation);
    void SetRotation(const glm::vec3& aEulerRot);
    void SetRotation(float aThetaX, float aThetaY, float ThetaZ);

    // WORLD SPACE TRANSFORM INFORMATION
    const glm::vec3& GetWorldTranslation() const;
    void SetWorldTranslation(const glm::vec3& aTrans);
    void SetWorldTranslation(float aX, float aY, float aZ);

    const glm::vec3& GetWorldScale() const;
    void SetWorldScale(const glm::vec3& aScale);
    void SetWorldScale(float aX, float aY, float aZ);

    const glm::quat& GetWorldRotation() const;
    glm::vec3 GetWorldRotationAsEuler() const;
    void SetWorldRotation(const glm::quat& aRotation);
    void SetWorldRotation(const glm::vec3& aEulerRot);
    void SetWorldRotation(float aThetaX, float aThetaY, float ThetaZ);

    inline void SetTranslationProperty(const glm::vec3& aTrans) { SetTranslation(aTrans); };
    inline void SetScaleProperty(const glm::vec3& aScale) { SetScale(aScale); };
    inline void SetRotationPropertyQuat(const glm::quat& aRot) { SetRotation(aRot); };
    inline void SetRotationProperty(const glm::vec3& aEulerRot) { SetRotation(aEulerRot); };

    inline void SetWorldTranslationProperty(const glm::vec3& aTrans) { SetWorldTranslation(aTrans); };
    inline void SetWorldScaleProperty(const glm::vec3& aScale) { SetWorldScale(aScale); };
    inline void SetWorldRotationProperty(const glm::vec3& aEulerRot) { SetWorldRotation(aEulerRot); };

    void ParentPositionChanged(TransformChanged *aEvent);
    void ParentScaleChanged(TransformChanged *aEvent);
    void ParentRotationChanged(TransformChanged *aEvent);
    void ParentObjectChanged(ParentChanged *aEvent);

  private:
    glm::vec3 GetAccumulatedParentTranslation();
    glm::vec3 GetAccumulatedParentScale();
    glm::quat GetAccumulatedParentRotation();

    void SetInternalTranslation(const glm::vec3 &aParentTranslation, const glm::vec3 &aLocalTranslation);
    void SetInternalScale(const glm::vec3 &aParentScale, const glm::vec3 &aLocalScale);
    void SetInternalRotation(const glm::quat &aParentRotation, const glm::quat &aLocalRotation);

    void InformPhysicsOfChange(const std::string &aEvent,
                               glm::quat aLocalRotationDifference = glm::quat{},
                               glm::quat aWorldRotationDifference = glm::quat{});

    glm::vec3 mTranslation;
    glm::vec3 mScale;
    glm::quat mRotation;

    glm::vec3 mWorldTranslation;
    glm::vec3 mWorldScale;
    glm::quat mWorldRotation;
  };
}

#endif