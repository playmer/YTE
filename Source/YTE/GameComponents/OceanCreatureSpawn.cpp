/******************************************************************************/
/*!
\file   OceanCreatureSpawn.cpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\date   2018-04-11

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
 

#include "YTE/GameComponents/OceanCreatureSpawn.hpp"

#include "YTE/Core/Engine.hpp" 
#include "YTE/Core/Space.hpp"
#include "YTE/Physics/Transform.hpp"

#include "YTE/Graphics/Generics/InstantiatedModel.hpp"
#include "YTE/Graphics/Generics/Mesh.hpp"

#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/GraphicsView.hpp"

#include "glm/gtx/rotate_vector.hpp"


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


  YTEDefineType(OceanCreatureSpawn)
  {
    YTERegisterType(OceanCreatureSpawn);

    YTEBindProperty(&OceanCreatureSpawn::GetTexture, &OceanCreatureSpawn::SetTexture, "Texture")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<DropDownStrings>(PopulateDropDownList);


    YTEBindProperty(&OceanCreatureSpawn::GetLifetime, &OceanCreatureSpawn::SetLifetime, "Lifetime")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&OceanCreatureSpawn::GetLifetimeVariance, &OceanCreatureSpawn::SetLifetimeVariance, "Lifetime Variance")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&OceanCreatureSpawn::GetParticleScale, &OceanCreatureSpawn::SetParticleScale, "Particle Scale")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&OceanCreatureSpawn::GetParticleScaleVariance, &OceanCreatureSpawn::SetParticleScaleVariance, "Scale Variance")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&OceanCreatureSpawn::GetEmitterScale, &OceanCreatureSpawn::SetEmitterScale, "Emitter Scale")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&OceanCreatureSpawn::GetEmitRate, &OceanCreatureSpawn::SetEmitRate, "Emit Rate")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&OceanCreatureSpawn::GetEmitCount, &OceanCreatureSpawn::SetEmitCount, "Emit Count")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();
  }

  OceanCreatureSpawn::OceanCreatureSpawn(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mVarianceIndex{ 0 }
    , mRenderer{ nullptr }
    , mTextureName{ "" }
    , mLifetime{ 3.0f }
    , mLifetimeVariance{ 0.0f }
    , mParticleScale{ 0.1f, 0.1f, 0.1f }
    , mParticleScaleVariance{ 0.0f }
    , mEmitterScale{}
    , mEmitRate{ 0.0f }
    , mEmitCount{ 0.0f }
    , mCameraTransform{ nullptr }
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void OceanCreatureSpawn::Initialize()
  {
    mTransform = mOwner->GetComponent<Transform>();
    
    if (Composition *boat = mOwner->GetSpace()->FindFirstCompositionByName("Boat"))
    {
      mBoatTransform = boat->GetComponent<Transform>();
    }

    mGraphicsView = mSpace->GetComponent<GraphicsView>();

    if (Composition *camera = GetSpace()->FindFirstCompositionByName("Camera"))
    {
      mCameraTransform = camera->GetComponent<Transform>();
    }

    mPosition = mOwner->GetComponent<Transform>()->GetWorldTranslation();
    mOwner->YTERegister(Events::PositionChanged, this, &OceanCreatureSpawn::OnTransformChanged);

    mRenderer = mOwner->GetEngine()->GetComponent<GraphicsSystem>()->GetRenderer();
    mTimer = mEmitRate;
    CreateMesh();

    for (size_t i = 0; i < 4000; ++i)
    {
      mVarianceBuffer.emplace_back((static_cast<float>(rand() % 200) / 100.0f) - 1.0f);
    }

    mOwner->GetSpace()->YTERegister(Events::LogicUpdate, this, &OceanCreatureSpawn::Update);
  }

  void OceanCreatureSpawn::Update(LogicUpdate *aEvent)
  {
    YTEUnusedArgument(aEvent);
    double dt = GetSpace()->GetEngine()->GetDt();
    float floatDt = aEvent->Dt;

    mTimer -= dt;

    //glm::quat camRot = mCameraTransform->GetWorldRotation();

    glm::vec3 xAxis(1.0f, 0.0f, 0.0f);

    // update the particles
    for (auto it = mCreatures.begin(); it < mCreatures.end(); ++it)
    {
      while (it < mCreatures.end() && it->first.mLife <= 0.0f)
      {
        it->second->SetVisibility(false);
        mFreeCreatures.emplace_back(std::move(it->second));
        it = mCreatures.erase(it);
      }

      if (it == mCreatures.end())
      {
        break;
      }

      Creature &creature = it->first;

      //auto rotation = creature.mRotation * YTE::AroundAxis(xAxis, floatDt);

      creature.mLife -= dt;
      //creature.mRotation = rotation;

      creature.mPosition = glm::rotate(creature.mPosition, 0.01f * floatDt, xAxis);

      creature.mUBO.mModelMatrix = glm::scale(glm::mat4(), creature.mScale);
      creature.mUBO.mModelMatrix[3][0] = creature.mPosition.x;
      creature.mUBO.mModelMatrix[3][1] = creature.mPosition.y;
      creature.mUBO.mModelMatrix[3][2] = creature.mPosition.z;

      it->second->UpdateUBOModel(creature.mUBO);
    }

    // time to make some new particles
    if (mTimer <= 0.0f)
    {
      // create the required particles
      for (int i = 0; i < mEmitCount; ++i)
      {
        CreateCreature();
      }

      // reset the countdown
      mTimer = mEmitRate;
    }
  }

  double OceanCreatureSpawn::GetLifetime()
  {
    return mLifetime;
  }

  void OceanCreatureSpawn::SetLifetime(double aLifetime)
  {
    mLifetime = aLifetime;
  }

  double OceanCreatureSpawn::GetLifetimeVariance()
  {
    return mLifetimeVariance;
  }

  void OceanCreatureSpawn::SetLifetimeVariance(double aLifetime)
  {
    mLifetimeVariance = aLifetime;
  }

  glm::vec3 OceanCreatureSpawn::GetParticleScale()
  {
    return mParticleScale;
  }

  void OceanCreatureSpawn::SetParticleScale(glm::vec3 aScale)
  {
    mParticleScale = aScale;
  }

  float OceanCreatureSpawn::GetParticleScaleVariance()
  {
    return mParticleScaleVariance;
  }

  void OceanCreatureSpawn::SetParticleScaleVariance(float aVariance)
  {
    mParticleScaleVariance = aVariance;
  }

  glm::vec3 OceanCreatureSpawn::GetEmitterScale()
  {
    return mEmitterScale;
  }

  void OceanCreatureSpawn::SetEmitterScale(glm::vec3 aScale)
  {
    mEmitterScale = aScale;
  }

  float OceanCreatureSpawn::GetEmitRate()
  {
    return mEmitRate;
  }

  void OceanCreatureSpawn::SetEmitRate(float aEmitRate)
  {
    mEmitRate = aEmitRate;
    mTimer = mEmitRate;
  }

  float OceanCreatureSpawn::GetEmitCount()
  {
    return mEmitCount;
  }

  void OceanCreatureSpawn::SetEmitCount(float aEmitCount)
  {
    mEmitCount = aEmitCount;
  }

  void OceanCreatureSpawn::CreateMesh()
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

    mFreeCreatures.clear();
  }

  void OceanCreatureSpawn::CreateCreature()
  {
    std::unique_ptr<InstantiatedModel> model;

    if (mFreeCreatures.empty())
    {
      model = mRenderer->CreateModel(mGraphicsView, mMesh);
      model->SetInstanced(true);
      model->mType = ShaderType::ShaderNoCull;
    }
    else
    {
      model = std::move(mFreeCreatures.back());
      mFreeCreatures.pop_back();
      model->SetVisibility(true);
    }

    float randomX = Variance();
    float randomY = Variance();
    float randomZ = Variance();

    // position randomness to cover whole emitter area
    glm::vec3 posDist;
    posDist.x = randomX * mEmitterScale.x;
    posDist.y = randomY * mEmitterScale.y;
    posDist.z = randomZ * mEmitterScale.z;

    glm::quat camRot = mCameraTransform->GetWorldRotation();

    Creature creature;
    creature.mPosition = mPosition + posDist;
    creature.mRotation = camRot;
    creature.mScale = mParticleScale;
    creature.mLife = mLifetime + randomX * mLifetimeVariance;

    creature.mUBO.mModelMatrix = glm::scale(glm::toMat4(creature.mRotation), creature.mScale);
    creature.mUBO.mModelMatrix[3][0] = creature.mPosition.x;
    creature.mUBO.mModelMatrix[3][1] = creature.mPosition.y;
    creature.mUBO.mModelMatrix[3][2] = creature.mPosition.z;

    mCreatures.emplace_back(creature, std::move(model));
  }

  void OceanCreatureSpawn::OnTransformChanged(TransformChanged *aEvent)
  {
    mPosition = aEvent->WorldPosition;
  }

  float OceanCreatureSpawn::Variance()
  {
    mVarianceIndex = (mVarianceIndex == (mVarianceBuffer.size() - 1)) ? 0 : ++mVarianceIndex;
    return mVarianceBuffer[mVarianceIndex];
  }
}
