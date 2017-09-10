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

#include "YTE/Core/Component.hpp"

namespace YTE
{
  DeclareEvent(PositionChanged);
  DeclareEvent(RotationChanged);
  DeclareEvent(ScaleChanged);
  DeclareEvent(OrientationChanged);
  class PositionChanged :public Event
  {
  public:
    DeclareType(PositionChanged);

    glm::vec3 Position;
  };

  class RotationChanged : public Event
  {
  public:
    DeclareType(RotationChanged);

    glm::quat Rotation;
  };

  class ScaleChanged : public Event
  {
  public:
    DeclareType(ScaleChanged);

    glm::vec3 Scale;
  };

  class OrientationChanged : public Event
  {
  public:
    DeclareType(OrientationChanged);

    glm::vec3 Forward;
    glm::vec3 Right;
    glm::vec3 Up;
  };

  class Transform : public Component
  {
  public:
    DeclareType(Transform);

    Transform(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    //void NativeInitialize() override;

    // LOCAL TRANSFORM INFORMATION
    const glm::vec3& GetTranslation() const;
    void SetPhysicsTranslation(const glm::vec3& aTrans);
    void SetTranslation(const glm::vec3& aTrans);
    void SetTranslation(float aX, float aY, float aZ);

    const glm::vec3& GetScale() const;
    void SetScale(const glm::vec3& aScale);
    void SetScale(float aX, float aY, float aZ);

    const glm::quat& GetRotation() const;
    glm::vec3 GetRotationAsEuler() const;
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
    inline void SetRotationProperty(const glm::vec3& aEulerRot) { SetRotation(aEulerRot); };

    inline void SetWorldTranslationProperty(const glm::vec3& aTrans) { SetWorldTranslation(aTrans); };
    inline void SetWorldScaleProperty(const glm::vec3& aScale) { SetWorldScale(aScale); };
    inline void SetWorldRotationProperty(const glm::vec3& aEulerRot) { SetWorldRotation(aEulerRot); };

  private:
    glm::vec3 mTranslation;
    glm::vec3 mScale;
    glm::quat mRotation;

    glm::vec3 mWorldTranslation;
    glm::vec3 mWorldScale;
    glm::quat mWorldRotation;
  };
}

#endif