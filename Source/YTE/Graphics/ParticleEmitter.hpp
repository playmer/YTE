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
    YTE_Shared ParticleEmitter(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    YTE_Shared ParticleEmitter(ParticleEmitter&&) = default;

    YTE_Shared void CreateMesh();

    YTE_Shared void Initialize() override;

    YTE_Shared void Update(LogicUpdate* aEvent);


    YTE_Shared std::string GetTextureName();
    YTE_Shared void SetTextureName(std::string aName);

    YTE_Shared glm::vec3 GetPositionOffset();
    YTE_Shared void SetPositionOffset(glm::vec3 aOffset);

    YTE_Shared glm::vec3 GetInitVelocity();
    YTE_Shared void SetInitVelocity(glm::vec3 aVelocity);

    YTE_Shared glm::vec3 GetVelocityVariance();
    YTE_Shared void SetVelocityVariance(glm::vec3 aVelocity);

    YTE_Shared double GetLifetime();
    YTE_Shared void SetLifetime(double aLifetime);

    YTE_Shared double GetLifetimeVariance();
    YTE_Shared void SetLifetimeVariance(double aLifetime);

    YTE_Shared glm::vec4 GetColor();
    YTE_Shared void SetColor(glm::vec4 aColor);

    YTE_Shared glm::vec3 GetParticleScale();
    YTE_Shared void SetParticleScale(glm::vec3 aScale);

    YTE_Shared glm::vec3 GetParticleScaleVariance();
    YTE_Shared void SetParticleScaleVariance(glm::vec3 aVariance);

    YTE_Shared glm::vec3 GetEmitterScale();
    YTE_Shared void SetEmitterScale(glm::vec3 aScale);

    YTE_Shared float GetEmitRate();
    YTE_Shared void SetEmitRate(float aEmitRate);

    YTE_Shared float GetEmitCount();
    YTE_Shared void SetEmitCount(float aEmitCount);

    YTE_Shared bool GetUseGravity();
    YTE_Shared void SetUseGravity(bool aUseGravity);

    YTE_Shared float GetGravityValue();
    YTE_Shared void SetGravityValue(float aGravityVal);

  private:
    std::vector<std::pair<Particle, std::unique_ptr<InstantiatedModel>>> mParticles;
    std::vector<std::unique_ptr<InstantiatedModel>> mFreeParticles;
    std::vector<float> mVarianceBuffer;
    size_t mVarianceIndex;

    Renderer *mRenderer;

    Transform *mCameraTransform;

    Mesh *mMesh;

    std::string mTextureName; //

    glm::vec3 mPosition;
    glm::vec3 mPositionOffset; //

    glm::vec3 mInitVelocity;
    glm::vec3 mVelocityVariance;

    double mLifetime;
    double mLifetimeVariance;
    double mTimer;

    glm::vec4 mColor;
    glm::vec3 mParticleScale;
    glm::vec3 mParticleScaleVariance;

    glm::vec3 mEmitterScale;

    bool mUseGravity;
    float mGravityValue;

    float mEmitRate;
    float mEmitCount;

    void CreateParticle();

    void OnTransformChanged(TransformChanged *aEvent);

    int RandomInt(int aMin, int aMax);
    float Variance();

    GraphicsView *mGraphicsView;

  };
}