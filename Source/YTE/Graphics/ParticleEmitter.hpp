#pragma once


#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Physics/ForwardDeclarations.hpp"
#include "YTE/Platform/ForwardDeclarations.hpp"

#include "YTE/Core/Component.hpp"

#include "YTE/Graphics/Generics/Texture.hpp"

#include "YTE/Graphics/UBOs.hpp"

#include "YTE/Graphics/Generics/Renderer.hpp"

namespace YTE
{
  struct Particle
  {
    glm::vec3 mPosition;
    glm::vec3 mScale;

    glm::vec4 mColor;

    glm::vec3 mVelocity;
    double mLife;

    glm::quat mRotation;

    UBOModel mUBO;
  };

  class ParticleEmitter : public Component
  {
  public:
    YTEDeclareType(ParticleEmitter);
    ParticleEmitter(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    ParticleEmitter(ParticleEmitter&&) = default;

    void Initialize() override;

    void Update(LogicUpdate* aEvent);


    std::string GetTextureName();
    void SetTextureName(std::string aName);

    glm::vec3 GetPositionOffset();
    void SetPositionOffset(glm::vec3 aOffset);

    glm::vec3 GetInitVelocity();
    void SetInitVelocity(glm::vec3 aVelocity);

    glm::vec3 GetVelocityVariance();
    void SetVelocityVariance(glm::vec3 aVelocity);

    double GetLifetime();
    void SetLifetime(double aLifetime);

    double GetLifetimeVariance();
    void SetLifetimeVariance(double aLifetime);

    glm::vec4 GetColor();
    void SetColor(glm::vec4 aColor);

    glm::vec3 GetParticleScale();
    void SetParticleScale(glm::vec3 aScale);

    glm::vec3 GetParticleScaleVariance();
    void SetParticleScaleVariance(glm::vec3 aVariance);

    glm::vec3 GetEmitterScale();
    void SetEmitterScale(glm::vec3 aScale);

    float GetEmitRate();
    void SetEmitRate(float aEmitRate);

    float GetEmitCount();
    void SetEmitCount(float aEmitCount);

    bool GetUseGravity();
    void SetUseGravity(bool aUseGravity);

  private:
    
    std::vector<std::pair<Particle, std::unique_ptr<InstantiatedModel>>> mParticles;

    YTE::Renderer *mRenderer;

    std::string mTextureName; //

    glm::vec3 mPosition;
    glm::vec3 mPositionOffset; //

    glm::vec3 mInitVelocity;
    glm::vec3 mVelocityVariance;
    
    double mLifetime;
    double mLifetimeVariance;

    glm::vec4 mColor;
    glm::vec3 mParticleScale;
    glm::vec3 mParticleScaleVariance;

    glm::vec3 mEmitterScale;

    bool mUseGravity;

    float mEmitRate;
    float mEmitCount;

    void CreateParticle();

    void OnTransformChanged(TransformChanged *aEvent);

    int RandomInt(int aMin, int aMax);
    float Variance();
    
  };
}
