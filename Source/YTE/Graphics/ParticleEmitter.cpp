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
  static std::vector<std::string> PopulateDropDownList(Component *aComponent)
  {
    YTEUnusedArgument(aComponent);

    std::wstring wStrPath = YTE::cWorkingDirectory;

    filesystem::path fsPath = Path::GetGamePath().String();

    filesystem::path finalPath = fsPath.parent_path() / L"Textures/Originals";

    std::vector<std::string> result;

    for (auto & p : filesystem::directory_iterator(finalPath))
    {
      std::string str = p.path().filename().generic_string();

      result.push_back(str);
    }

    return result;
  }


  YTEDefineType(ParticleEmitter)
  {
    YTERegisterType(ParticleEmitter);
    GetStaticType()->AddAttribute<RunInEditor>();

    std::vector<std::vector<Type*>> deps = { { Transform::GetStaticType() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);

    YTEBindProperty(&ParticleEmitter::GetTextureName, &ParticleEmitter::SetTextureName, "Texture Name")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .AddAttribute<DropDownStrings>(PopulateDropDownList);

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

    YTEBindProperty(&ParticleEmitter::GetGravityValue, &ParticleEmitter::SetGravityValue, "Gravity Scalar")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();
  }


  ParticleEmitter::ParticleEmitter(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mVarianceIndex{0}
    , mRenderer{ nullptr }
    , mTextureName{ "" }
    , mPosition{}
    , mPositionOffset{}
    , mInitVelocity{0.0f, 1.0f, 0.0f}
    , mVelocityVariance{}
    , mLifetime{3.0f}
    , mLifetimeVariance{0.0f}
    , mColor{1.0f, 1.0f, 1.0f, 1.0f}
    , mParticleScale{0.1f, 0.1f, 0.1f}
    , mParticleScaleVariance{}
    , mEmitterScale{}
    , mEmitRate{0.0f}
    , mEmitCount{0.0f}
    , mUseGravity{false}
    , mGravityValue{1.0f}
    , mCameraTransform{nullptr}
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void ParticleEmitter::Initialize()
  {
    GetSpace()->YTERegister(Events::FrameUpdate, this, &ParticleEmitter::Update);

    mGraphicsView = mSpace->GetComponent<GraphicsView>();

    if (Composition *camera = GetSpace()->FindFirstCompositionByName("Camera"))
    {
      mCameraTransform = camera->GetComponent<Transform>();
    }

    mPosition = mOwner->GetComponent<Transform>()->GetWorldTranslation();
    mOwner->YTERegister(Events::PositionChanged, this, &ParticleEmitter::OnTransformChanged);

    mRenderer = mOwner->GetEngine()->GetComponent<GraphicsSystem>()->GetRenderer();
    mTimer = mEmitRate;
    CreateMesh();

    for (size_t i = 0; i < 4000; ++i)
    {
      mVarianceBuffer.emplace_back((static_cast<float>(rand() % 200) / 100.0f) - 1.0f);
    }
  }

  void ParticleEmitter::Update(LogicUpdate *aEvent)
  {
    YTEProfileFunction();
    YTEUnusedArgument(aEvent);
    double dt = GetSpace()->GetEngine()->GetDt();

    mTimer -= dt;

    glm::quat camRot = mCameraTransform->GetWorldRotation();

    // update the particles
    for (auto it = mParticles.begin(); it < mParticles.end(); ++it)
    {
      while (it < mParticles.end() && it->first.mLife <= 0.0f)
      {
        it->second->SetVisibility(false);
        mFreeParticles.emplace_back(std::move(it->second));
        it = mParticles.erase(it);
      }

      if (it == mParticles.end())
      {
        break;
      }

      Particle &particle = it->first;

      particle.mLife -= dt;
      particle.mPosition += particle.mVelocity * static_cast<float>(dt);

      if (mUseGravity)
      {
        particle.mPosition.y -= mGravityValue * static_cast<float>((mLifetime - particle.mLife) * dt);
      }

      particle.mRotation = camRot;

      particle.mUBO.mModelMatrix = glm::scale(glm::toMat4(particle.mRotation), particle.mScale);
      particle.mUBO.mModelMatrix[3][0] = particle.mPosition.x;
      particle.mUBO.mModelMatrix[3][1] = particle.mPosition.y;
      particle.mUBO.mModelMatrix[3][2] = particle.mPosition.z;

      particle.mUBO.mDiffuseColor.a = particle.mLife / mLifetime;

      it->second->UpdateUBOModel(particle.mUBO);
    }

    // time to make some new particles
    if (mTimer <= 0.0f)
    {
      // create the required particles
      for (int i = 0; i < mEmitCount; ++i)
      {
        CreateParticle();
      }

      // reset the countdown
      mTimer = mEmitRate;
    }
  }

  std::string ParticleEmitter::GetTextureName()
  {
    return mTextureName;
  }

  void ParticleEmitter::SetTextureName(std::string aName)
  {
    mTextureName = aName;

    CreateMesh();
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
    mTimer = mEmitRate;
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

  float ParticleEmitter::GetGravityValue()
  {
    return mGravityValue;
  }

  void ParticleEmitter::SetGravityValue(float aGravityVal)
  {
    mGravityValue = aGravityVal;
  }


  void ParticleEmitter::CreateMesh()
  {
    if (nullptr == mRenderer)
    {
      return;
    }

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

    std::vector<Submesh> submeshes{ submesh };

    mMesh = mRenderer->CreateSimpleMesh(meshName, submeshes);

    mFreeParticles.clear();
  }

  void ParticleEmitter::CreateParticle()
  {
    std::unique_ptr<InstantiatedModel> model;

    if (mFreeParticles.empty())
    {
      model = mRenderer->CreateModel(mGraphicsView, mMesh);
      model->SetInstanced(true);
      model->mType = ShaderType::AdditiveBlendShader;
    }
    else
    {
      model = std::move(mFreeParticles.back());
      mFreeParticles.pop_back();
      model->SetVisibility(true);
    }

    float randomX = Variance();
    float randomY = Variance();
    float randomZ = Variance();

    // calculate the random bullshit
    glm::vec3 velVar;
    velVar.x = randomX * mVelocityVariance.x;
    velVar.y = randomY * mVelocityVariance.y;
    velVar.z = randomZ * mVelocityVariance.z;

    // position randomness to cover whole emitter area
    glm::vec3 posDist;
    posDist.x = randomX * mEmitterScale.x;
    posDist.y = randomY * mEmitterScale.y;
    posDist.z = randomZ * mEmitterScale.z;

    Particle particle;
    particle.mPosition = mPosition + mPositionOffset + posDist;
    particle.mScale = mParticleScale;
    particle.mColor = mColor;
    particle.mVelocity = mInitVelocity + velVar;
    particle.mLife = mLifetime + randomX * mLifetimeVariance;

    particle.mUBO.mModelMatrix = glm::scale(glm::toMat4(particle.mRotation), particle.mScale);
    particle.mUBO.mModelMatrix[3][0] = particle.mPosition.x;
    particle.mUBO.mModelMatrix[3][1] = particle.mPosition.y;
    particle.mUBO.mModelMatrix[3][2] = particle.mPosition.z;

    model->UpdateUBOModel(particle.mUBO);

    mParticles.emplace_back(particle, std::move(model));
  }

  void ParticleEmitter::OnTransformChanged(TransformChanged *aEvent)
  {
    mPosition = aEvent->WorldPosition;
  }

  float ParticleEmitter::Variance()
  {
    mVarianceIndex = (mVarianceIndex == (mVarianceBuffer.size() - 1)) ? 0 : ++mVarianceIndex;
    return mVarianceBuffer[mVarianceIndex];

    //return (static_cast<float>(rand() % 200) / 100.0f) - 1.0f;
  }
}
