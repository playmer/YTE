#include <algorithm>

#include "YTE/Core/Engine.hpp" 
#include "YTE/Core/Space.hpp"
#include "YTE/Physics/Transform.hpp"

#include "YTE/Graphics/ParticleEmitter.hpp"
#include "YTE/Graphics/Generics/InstantiatedModel.hpp"
#include "YTE/Graphics/Generics/Mesh.hpp"
#include "YTE/Graphics/Generics/Shader.hpp"

#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/GraphicsView.hpp"


namespace YTE
{
  inline constexpr bool cNewParticles = true;

  static std::vector<std::string> PopulateDropDownList(Component *aComponent)
  {
    UnusedArguments(aComponent);

    std::wstring wStrPath = YTE::cWorkingDirectory;

    filesystem::path fsPath = Path::GetGamePath().String();

    filesystem::path finalPath = fsPath.parent_path() / "Textures";

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
    RegisterType<ParticleEmitter>();
    TypeBuilder<ParticleEmitter> builder;
    GetStaticType()->AddAttribute<RunInEditor>();

    std::vector<std::vector<Type*>> deps = { { Transform::GetStaticType() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);

    builder.Property<&ParticleEmitter::GetTextureName, &ParticleEmitter::SetTextureName>("Texture Name")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .AddAttribute<DropDownStrings>(PopulateDropDownList);

    builder.Property<&ParticleEmitter::GetPositionOffset, &ParticleEmitter::SetPositionOffset>("Position Offset")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    builder.Property<&ParticleEmitter::GetInitVelocity, &ParticleEmitter::SetInitVelocity>("Init Velocity")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    builder.Property<&ParticleEmitter::GetVelocityVariance, &ParticleEmitter::SetVelocityVariance>("Velocity Variance")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    builder.Property<&ParticleEmitter::GetLifetime, &ParticleEmitter::SetLifetime>("Lifetime")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    builder.Property<&ParticleEmitter::GetLifetimeVariance, &ParticleEmitter::SetLifetimeVariance>("Lifetime Variance")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    builder.Property<&ParticleEmitter::GetColor, &ParticleEmitter::SetColor>("Color")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    builder.Property<&ParticleEmitter::GetParticleScale, &ParticleEmitter::SetParticleScale>("Particle Scale")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    builder.Property<&ParticleEmitter::GetParticleScaleVariance, &ParticleEmitter::SetParticleScaleVariance>("Scale Variance")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    builder.Property<&ParticleEmitter::GetEmitterScale, &ParticleEmitter::SetEmitterScale>("Emitter Scale")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    builder.Property<&ParticleEmitter::GetEmitRate, &ParticleEmitter::SetEmitRate>("Emit Rate")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    builder.Property<&ParticleEmitter::GetEmitCount, &ParticleEmitter::SetEmitCount>("Emit Count")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    builder.Property<&ParticleEmitter::GetUseGravity, &ParticleEmitter::SetUseGravity>("Use Gravity")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    builder.Property<&ParticleEmitter::GetGravityValue, &ParticleEmitter::SetGravityValue>("Gravity Scalar")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();
  }


  ParticleEmitter::ParticleEmitter(Composition *aOwner, Space *aSpace)
    : Component(aOwner, aSpace)
    , mVarianceIndex{0}
    , mUsedParticles{ 0 }
    , mCapacityParticles{ 0 }
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
  }

  void ParticleEmitter::Initialize()
  {
    GetSpace()->RegisterEvent<&ParticleEmitter::Update>(Events::FrameUpdate, this);

    mGraphicsView = mSpace->GetComponent<GraphicsView>();

    if (Composition *camera = GetSpace()->FindFirstCompositionByName("Camera"))
    {
      mCameraTransform = camera->GetComponent<Transform>();
    }

    mPosition = mOwner->GetComponent<Transform>()->GetWorldTranslation();
    mOwner->RegisterEvent<&ParticleEmitter::OnTransformChanged>(Events::PositionChanged, this);

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
    UnusedArguments(aEvent);
    double dt = GetSpace()->GetEngine()->GetDt();

    mTimer -= dt;

    glm::quat camRot = mCameraTransform->GetWorldRotation();

    UBOs::Material modelMaterial{};
    modelMaterial.mDiffuse = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
    modelMaterial.mAmbient = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
    modelMaterial.mSpecular = glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f };
    modelMaterial.mEmissive = glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f };
    modelMaterial.mShininess = 1.0f;

    //////////////////////////////////
    // New Particles
    glm::vec4 position0;
    glm::vec4 position1;
    glm::vec4 position2;
    glm::vec4 position3;
    glm::vec4 normal0;
    glm::vec4 normal1;
    glm::vec4 normal2;
    glm::vec4 normal3;
    glm::vec4 uv0;
    glm::vec4 uv1;
    glm::vec4 uv2;
    glm::vec4 uv3;

    glm::vec3 emptyPosition{ 0.0f, 0.0f, 0.0f };

    position0 = { -0.5f, -0.5f, 0.0f, 1.0f };
    position1 = {  0.5f, -0.5f, 0.0f, 1.0f };
    position2 = {  0.5f,  0.5f, 0.0f, 1.0f };
    position3 = { -0.5f,  0.5f, 0.0f, 1.0f };

    normal0 = { 0.0f, 0.0f, 1.0f, 1.0f };
    normal2 = { 0.0f, 0.0f, 1.0f, 1.0f };
    normal2 = { 0.0f, 0.0f, 1.0f, 1.0f };
    normal3 = { 0.0f, 0.0f, 1.0f, 1.0f };

    uv0 = { 0.0f, 0.0f, 0.0f, 1.0 };
    uv1 = { 1.0f, 0.0f, 0.0f, 1.0 };
    uv2 = { 1.0f, 1.0f, 0.0f, 1.0 };
    uv3 = { 0.0f, 1.0f, 0.0f, 1.0 };

    // update the particles
    for (auto it = mParticles.begin(); it < mParticles.end(); ++it)
    {
      while (it < mParticles.end() && it->first.mLife <= 0.0f)
      {
        if constexpr (false == cNewParticles)
        {
          it->second->SetVisibility(false);
          mFreeParticles.emplace_back(std::move(it->second));
          it = mParticles.erase(it);
        }
        else
        {
          --mUsedParticles;
        }
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

      float opacity = static_cast<float>(particle.mLife + 0.5f * mLifetime / mLifetime);

      if constexpr (false == cNewParticles)
      {
        modelMaterial.mDiffuse.w = opacity;

        it->second->UpdateUBOModel(particle.mUBO);
        it->second->UpdateUBOMaterial(&modelMaterial);
      }
    }

    if constexpr (cNewParticles)
    { 
      if (mParticles.size())
      {
        auto& positions = mSubmesh.mVertexData.mPositionData;
        auto& normals = mSubmesh.mVertexData.mNormalData;
        auto& textureCoordinates = mSubmesh.mVertexData.mTextureCoordinatesData;
        auto& indices = mSubmesh.mIndexData;

        indices.clear();
        positions.clear();
        normals.clear();

        size_t uvIndex = 0;
        for (auto& [particle, model] : mParticles)
        {
          float opacity = static_cast<float>(particle.mLife + 0.5f * mLifetime / mLifetime);

          positions.emplace_back(particle.mUBO.mModelMatrix * position0);
          positions.emplace_back(particle.mUBO.mModelMatrix * position1);
          positions.emplace_back(particle.mUBO.mModelMatrix * position2);
          positions.emplace_back(particle.mUBO.mModelMatrix * position3);

          auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(particle.mUBO.mModelMatrix)));

          normals.emplace_back(glm::normalize(normalMatrix * normal0));
          normals.emplace_back(glm::normalize(normalMatrix * normal1));
          normals.emplace_back(glm::normalize(normalMatrix * normal2));
          normals.emplace_back(glm::normalize(normalMatrix * normal3));

          // Update the texture coordinates.
          textureCoordinates[uvIndex + 0].z = opacity;
          textureCoordinates[uvIndex + 1].z = opacity;
          textureCoordinates[uvIndex + 2].z = opacity;
          textureCoordinates[uvIndex + 3].z = opacity;

          uvIndex += 4;
        }

        auto emptyParticles = static_cast<i64>(mCapacityParticles) - static_cast<i64>(mUsedParticles);

        // This should always just be rewritten completely for now. This could be optimized to always be 
        // filled with the correct data, since it never actually changes.
        for (i64 i = 0; i < mCapacityParticles; ++i)
        {
          indices.insert(indices.end(), { 0, 1, 2, 2, 3, 0 });
        }

        if (emptyParticles > 0)
        {
          positions.insert(positions.end(), static_cast<size_t>(emptyParticles * 4), emptyPosition);
        }

        for (i64 i = 0; i < emptyParticles; ++i)
        {
          normals.insert(normals.end(), { normal0, normal1,normal2,normal3 });
          //normals.emplace_back(normal0);
          //normals.emplace_back(normal1);
          //normals.emplace_back(normal2);
          //normals.emplace_back(normal3);
        }

        for (i64 i = 0; i < emptyParticles; ++i)
        {
          textureCoordinates.insert(textureCoordinates.end(), { uv0, uv1, uv2, uv3 });
          //textureCoordinates.emplace_back(uv0);
          //textureCoordinates.emplace_back(uv1);
          //textureCoordinates.emplace_back(uv2);
          //textureCoordinates.emplace_back(uv3);
        }

        //if (mParticles.size() > mCapacityParticles)
        //{
        //  RecreateMesh();
        //}
        //else
        //{
        //  auto& submesh = mMesh->GetSubmeshes()[0];
        //  auto& bufferData = submesh.mVertexBufferData;
        //  bufferData.mPositionBuffer.Update(positions);
        //  bufferData.mNormalBuffer.Update(normals);
        //  bufferData.mTextureCoordinatesBuffer.Update(textureCoordinates);
        //  submesh.mIndexBuffer.Update(indices);
        //}
      }
    }

    // time to make some new particles
    if (mTimer <= 0.0f)
    {
      // create the required particles
      for (int i = 0; i < mEmitCount; ++i)
      {
        if (cNewParticles)
        {
          ++mUsedParticles;
        }

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

    mSubmesh.mName = "__Sprite" + mTextureName;

    Vertex vert0;
    Vertex vert1;
    Vertex vert2;
    Vertex vert3;

    vert0.mPosition = { -0.5, -0.5, 0.0 };
    vert0.mTextureCoordinates = { 0.0f, 0.0f, 0.0f };
    vert0.mNormal = { 0.0f, 0.0f, 1.0f };
    vert1.mPosition = { 0.5, -0.5, 0.0 };
    vert1.mTextureCoordinates = { 1.0f, 0.0f, 0.0f };
    vert1.mNormal = { 0.0f, 0.0f, 1.0f };
    vert2.mPosition = { 0.5, 0.5, 0.0 };
    vert2.mTextureCoordinates = { 1.0f, 1.0f, 0.0f };
    vert2.mNormal = { 0.0f, 0.0f, 1.0f };
    vert3.mPosition = { -0.5, 0.5, 0.0 };
    vert3.mTextureCoordinates = { 0.0f, 1.0f, 0.0f };
    vert3.mNormal = { 0.0f, 0.0f, 1.0f };

    UBOs::Material modelMaterial{};
    modelMaterial.mDiffuse = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
    modelMaterial.mAmbient = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
    modelMaterial.mSpecular = glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f };
    modelMaterial.mEmissive = glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f };
    modelMaterial.mShininess = 1.0f;

    mSubmesh.mUBOMaterial = modelMaterial;

    std::vector<u32> mIndices{
      0, 1, 2,
      2, 3, 0
    };

    mSubmesh.mTextureData.emplace_back(mTextureName, TextureViewType::e2D, SubmeshData::TextureType::Diffuse);
    //sphere.mTextureData.emplace_back("white.png", TextureViewType::e2D, SubmeshData::TextureType::Specular);
    //sphere.mTextureData.emplace_back("white.png", TextureViewType::e2D, SubmeshData::TextureType::Normal);

    mSubmesh.mShaderSetName = "CPUParticles";
    mSubmesh.mDescriptionOverride = true;

    mSubmesh.mCullBackFaces = false;

    mSubmesh.mVertexData.AddVertex(vert0);
    mSubmesh.mVertexData.AddVertex(vert1);
    mSubmesh.mVertexData.AddVertex(vert2);
    mSubmesh.mVertexData.AddVertex(vert3);

    mSubmesh.mIndexData = std::move(mIndices);

    auto& descriptions = mSubmesh.mDescriptions;

    auto addUBO = [&descriptions](char const* aName, DescriptorType aDescriptorType, ShaderStageFlags aStage, size_t aBufferSize, size_t aBufferOffset = 0)
    {
      descriptions.AddPreludeLine(fmt::format("#define UBO_{}_BINDING {}", aName, descriptions.GetBufferBinding()));
      descriptions.AddDescriptor(aDescriptorType, aStage, aBufferSize, aBufferOffset);
    };

    addUBO("VIEW", DescriptorType::UniformBuffer, ShaderStageFlags::Vertex, sizeof(UBOs::View));
    addUBO("ANIMATION_BONE", DescriptorType::UniformBuffer, ShaderStageFlags::Vertex, sizeof(UBOs::Animation));
    addUBO("MODEL_MATERIAL", DescriptorType::UniformBuffer, ShaderStageFlags::Fragment, sizeof(UBOs::Material));
    addUBO("SUBMESH_MATERIAL", DescriptorType::UniformBuffer, ShaderStageFlags::Fragment, sizeof(UBOs::Material));
    addUBO("LIGHTS", DescriptorType::UniformBuffer, ShaderStageFlags::Fragment, sizeof(UBOs::LightManager));
    addUBO("ILLUMINATION", DescriptorType::UniformBuffer, ShaderStageFlags::Fragment, sizeof(UBOs::Illumination));
    addUBO("WATER", DescriptorType::UniformBuffer, ShaderStageFlags::Vertex, sizeof(UBOs::WaterInformationManager));
    addUBO("MODEL", DescriptorType::UniformBuffer, ShaderStageFlags::Vertex, sizeof(UBOs::Model));

    // Descriptions for the textures we support based on which maps we found above:
    for (auto sampler : mSubmesh.mTextureData)
    {
      descriptions.AddPreludeLine(fmt::format("#define UBO_{}_BINDING {}", SubmeshData::ToShaderString(sampler.mSamplerType), descriptions.GetBufferBinding()));
      descriptions.AddDescriptor(DescriptorType::CombinedImageSampler, ShaderStageFlags::Fragment, ImageLayout::ShaderReadOnlyOptimal);
    }

    descriptions.AddBindingAndAttribute<glm::vec3>(VertexInputRate::Vertex, VertexFormat::R32G32B32Sfloat);    //glm::vec3 mPosition;
    descriptions.AddBindingAndAttribute<glm::vec3>(VertexInputRate::Vertex, VertexFormat::R32G32B32Sfloat);    //glm::vec3 mTextureCoordinates;
    descriptions.AddBindingAndAttribute<glm::vec3>(VertexInputRate::Vertex, VertexFormat::R32G32B32Sfloat);    //glm::vec3 mNormal;

    RecreateMesh();
  }

  void ParticleEmitter::RecreateMesh()
  {
    // We make a copy here because CreateSimpleMesh steals our data away.
    auto submesh = mSubmesh;

    mCapacityParticles = mSubmesh.mVertexData.mPositionData.size();

    mMesh = mRenderer->CreateSimpleMesh(mSubmesh.mName, submesh, true);

    auto& vbd = mMesh->GetSubmeshes()[0].mVertexBufferData;

    vbd.mColorBuffer.reset();
    vbd.mTangentBuffer.reset();
    vbd.mBinormalBuffer.reset();
    vbd.mBitangentBuffer.reset();
    vbd.mBoneWeightsBuffer.reset();
    vbd.mBoneWeights2Buffer.reset();
    vbd.mBoneIDsBuffer.reset();
    vbd.mBoneIDs2Buffer.reset();


    if constexpr (false == cNewParticles)
    {
      mFreeParticles.clear();
    }
  }

  void ParticleEmitter::CreateParticle()
  {
    std::unique_ptr<InstantiatedModel> model;
    
    if constexpr (false == cNewParticles)
    {
      if (mFreeParticles.empty())
      {
        model = mRenderer->CreateModel(mGraphicsView, mMesh);
        model->mType = ShaderType::AlphaBlendShader;
      }
      else
      {
        model = std::move(mFreeParticles.back());
        mFreeParticles.pop_back();
        model->SetVisibility(true);
      }
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
    particle.mUBO.mDiffuseColor = mColor;
    particle.mVelocity = mInitVelocity + velVar;
    particle.mLife = mLifetime + randomX * mLifetimeVariance;

    particle.mUBO.mModelMatrix = glm::scale(glm::toMat4(particle.mRotation), particle.mScale);
    particle.mUBO.mModelMatrix[3][0] = particle.mPosition.x;
    particle.mUBO.mModelMatrix[3][1] = particle.mPosition.y;
    particle.mUBO.mModelMatrix[3][2] = particle.mPosition.z;

    UBOs::Material modelMaterial{};
    modelMaterial.mDiffuse = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
    modelMaterial.mAmbient = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
    modelMaterial.mSpecular = glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f };
    modelMaterial.mEmissive = glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f };
    modelMaterial.mShininess = 1.0f;


    if constexpr (cNewParticles)
    {
      mParticles.emplace_back(particle, nullptr);
    }
    else
    {
      modelMaterial.mDiffuse.w = static_cast<float>(particle.mLife / mLifetime);

      model->UpdateUBOMaterial(&modelMaterial);
      model->UpdateUBOModel(particle.mUBO);

      mParticles.emplace_back(particle, std::move(model));
    }
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
