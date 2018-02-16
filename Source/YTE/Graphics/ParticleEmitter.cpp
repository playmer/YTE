#include <algorithm>

#include "YTE/Core/Engine.hpp" 
#include "YTE/Core/Space.hpp"
#include "YTE/Physics/Transform.hpp"

#include "YTE/Graphics/ParticleEmitter.hpp"
#include "YTE/Graphics/Generics/InstantiatedModel.hpp"
#include "YTE/Graphics/Generics/Mesh.hpp"

#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/GraphicsView.hpp"


namespace YTE
{

  YTEDefineType(ParticleEmitter)
  {
    YTERegisterType(ParticleEmitter);

    std::vector<std::vector<Type*>> deps = { { Transform::GetStaticType() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);

    YTEBindProperty(&ParticleEmitter::GetTextureName, &ParticleEmitter::SetTextureName, "Texture Name")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&ParticleEmitter::GetPositionOffset, &ParticleEmitter::SetPositionOffset, "Position Offset")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&ParticleEmitter::GetInitVelocity, &ParticleEmitter::SetInitVelocity, "Init Velocity")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&ParticleEmitter::GetVelocityVariance, &ParticleEmitter::SetVelocityVariance, "Velocity Variance")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&ParticleEmitter::GetLifetime, &ParticleEmitter::SetLifetime, "Lifetime")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&ParticleEmitter::GetLifetimeVariance, &ParticleEmitter::SetLifetimeVariance, "Lifetime Variance")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&ParticleEmitter::GetColor, &ParticleEmitter::SetColor, "Color")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&ParticleEmitter::GetParticleScale, &ParticleEmitter::SetParticleScale, "Particle Scale")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&ParticleEmitter::GetParticleScaleVariance, &ParticleEmitter::SetParticleScaleVariance, "Scale Variance")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&ParticleEmitter::GetEmitterScale, &ParticleEmitter::SetEmitterScale, "Emitter Scale")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&ParticleEmitter::GetEmitRate, &ParticleEmitter::SetEmitRate, "Emit Rate")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&ParticleEmitter::GetEmitCount, &ParticleEmitter::SetEmitCount, "Emit Count")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&ParticleEmitter::GetUseGravity, &ParticleEmitter::SetUseGravity, "Use Gravity")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();
  }


  ParticleEmitter::ParticleEmitter(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mTextureName("")
    , mPosition()
    , mPositionOffset()
    , mInitVelocity(0.0f, 1.0f, 0.0f)
    , mVelocityVariance()
    , mLifetime(3.0f)
    , mLifetimeVariance(0.0f)
    , mColor(1.0f, 1.0f, 1.0f, 1.0f)
    , mParticleScale(0.1f, 0.1f, 0.1f)
    , mParticleScaleVariance()
    , mEmitterScale()
    , mEmitRate(0.0f)
    , mEmitCount(0.0f)
    , mUseGravity(false)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void ParticleEmitter::Initialize()
  {
    GetSpace()->YTERegister(Events::FrameUpdate, this, &ParticleEmitter::Update);

    mPosition = mOwner->GetComponent<Transform>()->GetWorldTranslation();
    mOwner->YTERegister(Events::PositionChanged, this, &ParticleEmitter::OnTransformChanged);

    mRenderer = mOwner->GetEngine()->GetComponent<GraphicsSystem>()->GetRenderer();
  }

  bool RemovePred(const std::pair<Particle, std::unique_ptr<InstantiatedModel>> &boi)
  {
    if (boi.first.mLife <= 0.0f) return true;
  
    return false;
  }

  void ParticleEmitter::Update(LogicUpdate *aEvent)
  {
    YTEUnusedArgument(aEvent);
    double dt = GetSpace()->GetEngine()->GetDt();

    // erase the dead bois
    mParticles.erase(
      std::remove_if(mParticles.begin(),
                     mParticles.end(),
                     RemovePred
      ),
      mParticles.end()
    );

    static double timer = mEmitRate;

    // time to make some new particle bois
    if (timer <= 0.0f)
    {
      // create those bois
      for (int i = 0; i < mEmitCount; ++i)
      {
        CreateParticle();
      }

      // reset the countdown
      timer = mEmitRate;
    }
    else
    {
      // decrement the timer
      timer -= dt;
    }

    // update all the bois ;)

    for (auto &particleIt : mParticles)
    {
      Particle &particle = particleIt.first;

      //std::cout << fmt::format("{}\n", particle.mPosition.x);
    
      particle.mLife -= dt;
      particle.mPosition += particle.mVelocity * static_cast<float>(dt);

      if (mUseGravity)
      {
        particle.mPosition.y -= static_cast<float>((mLifetime - particle.mLife) * dt);
      }

      particle.mUBO.mModelMatrix = glm::translate(glm::mat4(1.0f), particle.mPosition);
      particle.mUBO.mModelMatrix = particle.mUBO.mModelMatrix * glm::toMat4(particle.mRotation);
      particle.mUBO.mModelMatrix = glm::scale(particle.mUBO.mModelMatrix, particle.mScale);

      particleIt.second->UpdateUBOModel(particle.mUBO);
    }
  }

  std::string ParticleEmitter::GetTextureName()
  {
    return mTextureName;
  }

  void ParticleEmitter::SetTextureName(std::string aName)
  {
    mTextureName = aName;
  }

  glm::vec3 ParticleEmitter::GetPositionOffset()
  {
    return mPositionOffset;
  }

  void ParticleEmitter::SetPositionOffset(glm::vec3 aOffset)
  {
    mPositionOffset = aOffset;
  }

  glm::vec3 ParticleEmitter::GetInitVelocity()
  {
    return mInitVelocity;
  }

  void ParticleEmitter::SetInitVelocity(glm::vec3 aVelocity)
  {
    mInitVelocity = aVelocity;
  }

  glm::vec3 ParticleEmitter::GetVelocityVariance()
  {
    return mVelocityVariance;
  }

  void ParticleEmitter::SetVelocityVariance(glm::vec3 aVelocity)
  {
    mVelocityVariance = aVelocity;
  }

  double ParticleEmitter::GetLifetime()
  {
    return mLifetime;
  }

  void ParticleEmitter::SetLifetime(double aLifetime)
  {
    mLifetime = aLifetime;
  }

  double ParticleEmitter::GetLifetimeVariance()
  {
    return mLifetimeVariance;
  }

  void ParticleEmitter::SetLifetimeVariance(double aLifetime)
  {
    mLifetimeVariance = aLifetime;
  }

  glm::vec4 ParticleEmitter::GetColor()
  {
    return mColor;
  }

  void ParticleEmitter::SetColor(glm::vec4 aColor)
  {
    mColor = aColor;
  }

  glm::vec3 ParticleEmitter::GetParticleScale()
  {
    return mParticleScale;
  }

  void ParticleEmitter::SetParticleScale(glm::vec3 aScale)
  {
    mParticleScale = aScale;
  }

  glm::vec3 ParticleEmitter::GetParticleScaleVariance()
  {
    return mParticleScaleVariance;
  }

  void ParticleEmitter::SetParticleScaleVariance(glm::vec3 aVariance)
  {
    mParticleScaleVariance = aVariance;
  }

  glm::vec3 ParticleEmitter::GetEmitterScale()
  {
    return mEmitterScale;
  }

  void ParticleEmitter::SetEmitterScale(glm::vec3 aScale)
  {
    mEmitterScale = aScale;
  }

  float ParticleEmitter::GetEmitRate()
  {
    return mEmitRate;
  }

  void ParticleEmitter::SetEmitRate(float aEmitRate)
  {
    mEmitRate = aEmitRate;
  }

  float ParticleEmitter::GetEmitCount()
  {
    return mEmitCount;
  }

  void ParticleEmitter::SetEmitCount(float aEmitCount)
  {
    mEmitCount = aEmitCount;
  }

  bool ParticleEmitter::GetUseGravity()
  {
    return mUseGravity;
  }

  void ParticleEmitter::SetUseGravity(bool aUseGravity)
  {
    mUseGravity = aUseGravity;
  }

  void ParticleEmitter::CreateParticle()
  {
    if (mTextureName.empty())
    {
      mTextureName = "Mats_Diffuse.png";
    }

    std::string meshName = "__Sprite" + mTextureName;

    Submesh submesh;

    Vertex vert0;
    Vertex vert1;
    Vertex vert2;
    Vertex vert3;

    vert0.mPosition = { -0.5, -0.5, 0.0 };
    vert0.mTextureCoordinates = { 0.0f, 0.0f, 0.0f };
    vert1.mPosition = { 0.5, -0.5, 0.0 };
    vert1.mTextureCoordinates = { 1.0f, 0.0f, 0.0f };
    vert2.mPosition = { 0.5, 0.5, 0.0 };
    vert2.mTextureCoordinates = { 1.0f, 1.0f, 0.0f };
    vert3.mPosition = { -0.5, 0.5, 0.0 };
    vert3.mTextureCoordinates = { 0.0f, 1.0f, 0.0f };

    std::vector<u32> mIndices{
      0, 1, 2,
      2, 3, 0
    };

    submesh.mDiffuseMap = mTextureName;
    submesh.mDiffuseType = TextureViewType::e2D;
    submesh.mShaderSetName = "Sprite";

    submesh.mCullBackFaces = false;

    submesh.mVertexBuffer.emplace_back(vert0);
    submesh.mVertexBuffer.emplace_back(vert1);
    submesh.mVertexBuffer.emplace_back(vert2);
    submesh.mVertexBuffer.emplace_back(vert3);

    submesh.mIndexBuffer = std::move(mIndices);

    submesh.mVertexBufferSize = submesh.mVertexBuffer.size() * sizeof(Vertex);
    submesh.mIndexBufferSize = submesh.mIndexBuffer.size() * sizeof(u32);

    std::vector<Submesh> submeshes{ submesh };

    auto view = mSpace->GetComponent<GraphicsView>();

    auto mesh = mRenderer->CreateSimpleMesh(view, meshName, submeshes);

    std::unique_ptr<InstantiatedModel> model = mRenderer->CreateModel(view, mesh);

    model->mUseAlphaBlending = true;
    model->mUseAdditiveBlending = true;

    // calculate the random bullshit
    glm::vec3 velVar;
    velVar.x = Variance() * mVelocityVariance.x;
    velVar.y = Variance() * mVelocityVariance.y;
    velVar.z = Variance() * mVelocityVariance.z;

    glm::vec3 scaleVar;
    scaleVar.x = Variance() * mParticleScaleVariance.x;
    scaleVar.y = Variance() * mParticleScaleVariance.y;
    scaleVar.z = Variance() * mParticleScaleVariance.z;

    // position randomness to cover whole emitter area
    glm::vec3 posDist;
    posDist.x = Variance() * mEmitterScale.x;
    posDist.y = Variance() * mEmitterScale.y;
    posDist.z = Variance() * mEmitterScale.z;

    Particle particle;
    particle.mPosition = mPosition + mPositionOffset + posDist;
    particle.mScale = mParticleScale + scaleVar;
    particle.mColor = mColor;
    particle.mVelocity = mInitVelocity + velVar;
    particle.mLife = mLifetime + Variance() * mLifetimeVariance;

    particle.mUBO.mModelMatrix = glm::translate(glm::mat4(), particle.mPosition);
    particle.mUBO.mModelMatrix = particle.mUBO.mModelMatrix * glm::toMat4(particle.mRotation);
    particle.mUBO.mModelMatrix = glm::scale(particle.mUBO.mModelMatrix, mParticleScale);
    
    mParticles.emplace_back(particle, std::move(model));
  }

  void ParticleEmitter::OnTransformChanged(TransformChanged *aEvent)
  {
    mPosition = aEvent->WorldPosition;
  }

  int ParticleEmitter::RandomInt(int aMin, int aMax)
  {
    int range = aMax - aMin;

    return rand() % range + aMin;
  }

  float ParticleEmitter::Variance()
  {
    return (static_cast<float>(rand() % 200) / 100.0f) - 1.0f;
  }

}

