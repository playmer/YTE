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


  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Properties
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

  std::string ParticleEmitter::GetTextureName()
  {
    return mTextureName;
  }

  void ParticleEmitter::SetTextureName(std::string aName)
  {
    if (mTextureName != aName)
    {
      mTextureName = aName;

      CreateMesh();
    }
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
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


  ParticleEmitter::ParticleEmitter(Composition *aOwner, Space *aSpace)
    : Component(aOwner, aSpace)
    , mVarianceIndex{0}
    , mCapacityParticles{ 0 }
    , mRenderer{ nullptr }
    , mCameraTransform{ nullptr }
    , mMesh{ nullptr }
    , mTextureName{ "" }
    , mPosition{}
    , mPositionOffset{}
    , mInitVelocity{0.0f, 1.0f, 0.0f}
    , mVelocityVariance{0.0f, 0.0f, 0.0f}
    , mLifetime{3.0f}
    , mLifetimeVariance{0.0f}
    , mColor{1.0f, 1.0f, 1.0f, 1.0f}
    , mParticleScale{0.1f, 0.1f, 0.1f}
    , mParticleScaleVariance{}
    , mEmitterScale{0.0f, 0.0f, 0.0f}
    , mEmitRate{0.0f}
    , mEmitCount{ 0.0f }
    , mUseGravity{false}
    , mGravityValue{1.0f}
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

  void ParticleEmitter::Update(LogicUpdate* aEvent)
  {
    YTEProfileFunction();

    double dt = GetSpace()->GetEngine()->GetDt();

    //////////////////////////////////
    // New Particles

    DeleteParticlesPass(dt);
    UpdateParticlesPass(dt);
    RecreateMeshIfNeededPass();
    UpdateParticleMesh();
    NewParticlesPass(dt);
  }


  // In this pass we decide which particles must be deleted and then remove them.
  void ParticleEmitter::DeleteParticlesPass(double aDt)
  {
    auto startToRemove = std::remove_if(
      mParticles.begin(), 
      mParticles.end(), 
      [aDt](Particle& particle)
    {
        particle.mLife -= aDt;

        return (particle.mLife < 0.0f);
    });

    mParticles.erase(startToRemove, mParticles.end());
  }

  // In this pass we create a model matrix for each particle.
  void ParticleEmitter::UpdateParticlesPass(double aDt)
  {
    glm::quat camRot = mCameraTransform->GetWorldRotation();
    mParticleMatrices.clear();

    for (auto& particle : mParticles)
    {
      particle.mPosition += particle.mVelocity * static_cast<float>(aDt);

      if (mUseGravity)
      {
        particle.mPosition.y -= mGravityValue * static_cast<float>((mLifetime - particle.mLife) * aDt);
      }

      particle.mRotation = camRot;

      particle.mUBO.mModelMatrix = glm::scale(glm::toMat4(particle.mRotation), particle.mScale);
      particle.mUBO.mModelMatrix[3][0] = particle.mPosition.x;
      particle.mUBO.mModelMatrix[3][1] = particle.mPosition.y;
      particle.mUBO.mModelMatrix[3][2] = particle.mPosition.z;

      mParticleMatrices.emplace_back(particle.mUBO.mModelMatrix);
    }
  }

  // In this pass we create a new Model and Mesh if we've exceeded our current capacity.
  void ParticleEmitter::RecreateMeshIfNeededPass()
  {
    if (mParticles.size() > mCapacityParticles)
    {
      FillBuffersToRequired();
      RecreateMesh();
    }
  }

  void ParticleEmitter::FillBuffersToRequired()
  {
    auto& positions = mSubmesh.mVertexData.mPositionData;
    auto& normals = mSubmesh.mVertexData.mNormalData;
    auto& textureCoordinates = mSubmesh.mVertexData.mTextureCoordinatesData;
    auto& indices = mSubmesh.mIndexData;

    positions.clear();
    normals.clear();
    textureCoordinates.clear();
    indices.clear();

    size_t instancesNeeded = mParticleMatrices.size();
    size_t verticesNeeded = mParticleMatrices.size() * 4;
    size_t indicesNeeded = mParticleMatrices.size() * 6;

    glm::vec3 emptyPosition{ 0.0f, 0.0f, 0.0f };

    std::array<glm::vec3, 4> normalsToInsert{
      glm::vec3{ 0.0f, 0.0f, 1.0f },
      glm::vec3{ 0.0f, 0.0f, 1.0f },
      glm::vec3{ 0.0f, 0.0f, 1.0f },
      glm::vec3{ 0.0f, 0.0f, 1.0f }
    };

    std::array<glm::vec3, 4> textureCoordinatesToInsert{
      glm::vec3{ 0.0f, 0.0f, 1.0f },
      glm::vec3{ 0.0f, 0.0f, 1.0f },
      glm::vec3{ 0.0f, 0.0f, 1.0f },
      glm::vec3{ 0.0f, 0.0f, 1.0f }
    };

    positions.insert(positions.begin(), verticesNeeded, emptyPosition);

    for (size_t i = 0; i < instancesNeeded; ++i)
    {
      normals.insert(normals.begin(), normalsToInsert.begin(), normalsToInsert.end());
    }

    for (size_t i = 0; i < instancesNeeded; ++i)
    {
      textureCoordinates.insert(textureCoordinates.begin(), textureCoordinatesToInsert.begin(), textureCoordinatesToInsert.end());
    }

    indices.insert(indices.begin(), indicesNeeded, 0);
  }


  // In this pass we update the buffers in the Mesh.
  void ParticleEmitter::UpdateParticleMesh()
  {
    if (0 == mParticleMatrices.size())
    {
      mModel->SetVisibility(false);
      return;
    }

    mModel->SetVisibility(true);

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
    position1 = { 0.5f, -0.5f, 0.0f, 1.0f };
    position2 = { 0.5f,  0.5f, 0.0f, 1.0f };
    position3 = { -0.5f,  0.5f, 0.0f, 1.0f };

    normal0 = { 0.0f, 0.0f, 1.0f, 1.0f };
    normal2 = { 0.0f, 0.0f, 1.0f, 1.0f };
    normal2 = { 0.0f, 0.0f, 1.0f, 1.0f };
    normal3 = { 0.0f, 0.0f, 1.0f, 1.0f };

    uv0 = { 0.0f, 0.0f, 0.0f, 1.0 };
    uv1 = { 1.0f, 0.0f, 0.0f, 1.0 };
    uv2 = { 1.0f, 1.0f, 0.0f, 1.0 };
    uv3 = { 0.0f, 1.0f, 0.0f, 1.0 };

    auto& positions = mSubmesh.mVertexData.mPositionData;
    auto& normals = mSubmesh.mVertexData.mNormalData;
    auto& textureCoordinates = mSubmesh.mVertexData.mTextureCoordinatesData;
    auto& indices = mSubmesh.mIndexData;

    indices.clear();
    positions.clear();
    normals.clear();
    textureCoordinates.clear();

    // This should always just be rewritten completely for now. This could be optimized to always be 
    // filled with the correct data, since it never actually changes.
    for (size_t i = 0; i < mParticles.size(); ++i)
    {
      textureCoordinates.insert(textureCoordinates.end(), { uv0, uv1, uv2, uv3 });
    }

    // Insert the indices needed.
    u32 particleIndex = 0;
    for (size_t i = 0; i < mParticles.size(); ++i)
    {
      indices.insert(indices.end(), {
        particleIndex + 0,
        particleIndex + 1,
        particleIndex + 2,
        particleIndex + 2,
        particleIndex + 3,
        particleIndex + 0
        });
      particleIndex += 4;
    }

    // Update the positions
    for (auto& matrix : mParticleMatrices)
    {
      positions.emplace_back(matrix * position0);
      positions.emplace_back(matrix * position1);
      positions.emplace_back(matrix * position2);
      positions.emplace_back(matrix * position3);
    }

    // Update the normals
    for (auto& matrix : mParticleMatrices)
    {
      auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(matrix)));

      normals.emplace_back(glm::normalize(normalMatrix * normal0));
      normals.emplace_back(glm::normalize(normalMatrix * normal1));
      normals.emplace_back(glm::normalize(normalMatrix * normal2));
      normals.emplace_back(glm::normalize(normalMatrix * normal3));
    }

    // Update the texture coordinates.
    size_t uvIndex = 0;

    for (auto& particle : mParticles)
    {
      float opacity = static_cast<float>(particle.mLife + 0.5f * mLifetime / mLifetime);

      textureCoordinates[uvIndex + 0].z = opacity;
      textureCoordinates[uvIndex + 1].z = opacity;
      textureCoordinates[uvIndex + 2].z = opacity;
      textureCoordinates[uvIndex + 3].z = opacity;

      uvIndex += 4;
    }

    auto& submesh = mMesh->GetSubmeshes()[0];
    auto& bufferData = submesh.mVertexBufferData;
    bufferData.mPositionBuffer.Update(positions);
    bufferData.mNormalBuffer.Update(normals);
    bufferData.mTextureCoordinatesBuffer.Update(textureCoordinates);
    submesh.mIndexBuffer.Update(indices);

    // Should fix this in the core engine, but we use this index data for counting the indices.
    // Ideally we use some other way to count them.
    submesh.mData.mIndexData = indices;

    mMesh->RecalculateDimensions();
  }

  // In this pass we create new particles.
  void ParticleEmitter::NewParticlesPass(double aDt)
  {
    mTimer -= aDt;

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

    mSubmesh.mName = fmt::format("{}__Sprite{}", mOwner->GetGUID().ToIdentifierString(), mTextureName);
    mSubmesh.mTextureData.clear();
    mSubmesh.mTextureData.emplace_back(mTextureName, TextureViewType::e2D, SubmeshData::TextureType::Diffuse);

    if (nullptr == mMesh)
    {
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
    }
   
    RecreateMesh();
  }

  void ParticleEmitter::RecreateMesh()
  {
    // We make a copy here because CreateSimpleMesh steals our data away.
    auto submesh = mSubmesh;

    DebugAssert(0 == (mSubmesh.mVertexData.mPositionData.size() % 4), "We must have a mod 4 number of particles.");

    DebugAssert((mSubmesh.mVertexData.mPositionData.size() / 4) == (mSubmesh.mIndexData.size() / 6),
      "When recreating the mesh, we need enough index instances to match position instances.");

    mCapacityParticles = mParticles.size();

    std::string name = submesh.mName;

    mMesh = mRenderer->CreateSimpleMesh(name, submesh, true);

    DebugAssert((1 == mMesh->GetSubmeshes().size()), "We need a submesh, what happened to it?");

    auto& vbd = mMesh->GetSubmeshes()[0].mVertexBufferData;

    vbd.mColorBuffer.reset();
    vbd.mTangentBuffer.reset();
    vbd.mBinormalBuffer.reset();
    vbd.mBitangentBuffer.reset();
    vbd.mBoneWeightsBuffer.reset();
    vbd.mBoneWeights2Buffer.reset();
    vbd.mBoneIDsBuffer.reset();
    vbd.mBoneIDs2Buffer.reset();

    mModel = mRenderer->CreateModel(mGraphicsView, mMesh);
    mModel->mType = ShaderType::AlphaBlendShader;
    mModel->GetModelUBOBuffer().Update(mModel->GetUBOModelData());
  }

  void ParticleEmitter::CreateParticle()
  {
    std::unique_ptr<InstantiatedModel> model;
    
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
    
    mParticles.emplace_back(particle);
  }

  void ParticleEmitter::OnTransformChanged(TransformChanged *aEvent)
  {
    mPosition = aEvent->WorldPosition;
  }

  float ParticleEmitter::Variance()
  {
    mVarianceIndex = (mVarianceIndex == (mVarianceBuffer.size() - 1)) ? 0 : ++mVarianceIndex;
    return mVarianceBuffer[mVarianceIndex];
  }
}
