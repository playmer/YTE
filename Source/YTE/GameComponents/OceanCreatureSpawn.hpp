/******************************************************************************/
/*!
\file   OceanCreatureSpawn.hpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\date   2018-04-11
\brief
    Spawns and rotates ocean creatures.

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#ifndef YTE_Gameplay_OceanCreatureSpawn_hpp
#define YTE_Gameplay_OceanCreatureSpawn_hpp

#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Physics/ForwardDeclarations.hpp"
#include "YTE/Platform/ForwardDeclarations.hpp"

#include "YTE/Core/Component.hpp"

#include "YTE/Graphics/Generics/Texture.hpp"

#include "YTE/Graphics/UBOs.hpp"

#include "YTE/Graphics/Generics/Renderer.hpp"

namespace YTE
{
  struct Creature
  {
    glm::vec3 mPosition;
    glm::vec3 mScale;
    double mLife;
    glm::quat mRotation;
    UBOModel mUBO;
  };

  class OceanCreatureSpawn : public Component
  {
  public:
    YTEDeclareType(OceanCreatureSpawn);
    OceanCreatureSpawn(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    OceanCreatureSpawn(OceanCreatureSpawn&&) = default;

    void CreateMesh();

    void Initialize() override;

    void Update(LogicUpdate* aEvent);

    double GetLifetime();
    void SetLifetime(double aLifetime);

    double GetLifetimeVariance();
    void SetLifetimeVariance(double aLifetime);

    glm::vec3 GetParticleScale();
    void SetParticleScale(glm::vec3 aScale);

    float GetParticleScaleVariance();
    void SetParticleScaleVariance(float aVariance);

    glm::vec3 GetEmitterScale();
    void SetEmitterScale(glm::vec3 aScale);

    float GetEmitRate();
    void SetEmitRate(float aEmitRate);

    float GetEmitCount();
    void SetEmitCount(float aEmitCount);

    std::string GetTexture()
    {
      return mTextureName;
    }

    void SetTexture(std::string &aTexture)
    {
      if (aTexture != mTextureName &&
        0 != aTexture.size())
      {
        mTextureName = aTexture;
      }
    }

  private:
    Transform *mTransform;
    Transform *mBoatTransform;

    std::string mTextureName;

    std::vector<std::pair<Creature, std::unique_ptr<InstantiatedModel>>> mCreatures;
    std::vector<std::unique_ptr<InstantiatedModel>> mFreeCreatures;
    std::vector<float> mVarianceBuffer;
    size_t mVarianceIndex;

    Renderer *mRenderer;

    Transform *mCameraTransform;

    Mesh *mMesh;

    glm::vec3 mPosition;

    double mLifetime;
    double mLifetimeVariance;
    double mTimer;

    glm::vec3 mParticleScale;
    float mParticleScaleVariance;

    glm::vec3 mEmitterScale;

    float mEmitRate;
    float mEmitCount;

    void CreateCreature();

    void OnTransformChanged(TransformChanged *aEvent);

    float Variance();

    GraphicsView *mGraphicsView;
  };
} 

#endif
