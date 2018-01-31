#include <algorithm>

#include "YTE/Core/Engine.hpp" 
#include "YTE/Core/Space.hpp"

#include "YTE/Graphics/ParticleEmitter.hpp"
#include "YTE/Graphics/Generics/InstantiatedModel.hpp"
#include "YTE/Graphics/Generics/Mesh.hpp"
#include "YTE/Graphics/UBOs.hpp"

#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/Vulkan/VkRenderer.hpp"


namespace YTE
{

  YTEDefineType(ParticleEmitter)
  {
    YTERegisterType(ParticleEmitter);

    YTEBindProperty(&ParticleEmitter::GetTextureName, &ParticleEmitter::SetTextureName, "Texture Name")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&ParticleEmitter::GetPositionOffset, &ParticleEmitter::SetPositionOffset, "Position Offset")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&ParticleEmitter::GetInitVelocity, &ParticleEmitter::SetInitVelocity, "Init Velocity")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&ParticleEmitter::GetVelocityVariance, &ParticleEmitter::GetVelocityVariance, "Velocity Variance")
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

    YTEBindProperty(&ParticleEmitter::GetEmitterScale, &ParticleEmitter::SetEmitterScale, "Emitter Scale")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&ParticleEmitter::GetEmitRate, &ParticleEmitter::SetEmitRate, "Emit Rate")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&ParticleEmitter::GetEmitCount, &ParticleEmitter::SetEmitCount, "Emit Count")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();
  }


  ParticleEmitter::ParticleEmitter(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
  }

  ParticleEmitter::~ParticleEmitter()
  {
    mParticles.clear();
  }

  void ParticleEmitter::Initialize()
  {
    GetSpace()->YTERegister(Events::FrameUpdate, this, &ParticleEmitter::Update);
  }

  bool RemovePred(const std::pair<Particle, std::unique_ptr<InstantiatedModel>> &boi)
  {
    if (boi.first.mLife <= 0.0f) return true;

    return false;
  }

  void ParticleEmitter::Update(LogicUpdate *aEvent)
  {
    float dt = GetSpace()->GetEngine()->GetDt();;

    // erase the dead bois
    mParticles.erase(
      std::remove_if(mParticles.begin(),
                     mParticles.end(),
                     RemovePred
      ),
      mParticles.end()
    );

    // update all the bois ;)
    for (auto it = mParticles.begin(); it != mParticles.end(); ++it)
    {
      Particle &particle = it->first;

      particle.mLife -= dt;
      particle.mPosition += particle.mVelocity;
    }
    
    static float timer = mEmitRate;

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

  float ParticleEmitter::GetLifetime()
  {
    return mLifetime;
  }

  void ParticleEmitter::SetLifetime(float aLifetime)
  {
    mLifetime = aLifetime;
  }

  float ParticleEmitter::GetLifetimeVariance()
  {
    return mLifetimeVariance;
  }

  void ParticleEmitter::SetLifetimeVariance(float aLifetime)
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

  void ParticleEmitter::CreateParticle()
  {
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

    auto graphics = mSpace->GetComponent<GraphicsSystem>();
    VkRenderer *renderer = static_cast<VkRenderer*>(graphics->GetRenderer());

    auto mesh = renderer->CreateSimpleMesh(view, meshName, submeshes);

    auto model = renderer->CreateModel(view, mesh);

    Particle particle;
    particle.mPosition = mPosition + mPositionOffset;
    particle.mScale = mParticleScale;
    particle.mColor = mColor;
    particle.mVelocity = mInitVelocity;
    particle.mLife = mLifetime;

    particle.mUBO = new UBOModel();
    particle.mUBO->mModelMatrix = glm::translate(glm::mat4(), particle.mPosition);
    particle.mUBO->mModelMatrix = particle.mUBO->mModelMatrix * glm::toMat4(particle.mRotation);
    particle.mUBO->mModelMatrix = glm::scale(particle.mUBO->mModelMatrix, mParticleScale);
    
    mParticles.emplace_back(particle, std::move(model));
  }

}

