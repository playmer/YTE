#include <fstream>

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include "YTE/Core/AssetLoader.hpp"
#include "YTE/Core/Engine.hpp"

#include "YTE/Graphics/Animation.hpp"
#include "YTE/Graphics/FacialAnimator.hpp"
#include "YTE/Graphics/Generics/InstantiatedModel.hpp"
#include "YTE/Graphics/Model.hpp"

#include "YTE/Utilities/Utilities.hpp"

namespace fs = std::filesystem;

namespace YTE
{
  YTEDefineType(FacialAnimator)
  {
    RegisterType<FacialAnimator>();
    TypeBuilder<FacialAnimator> builder;

    std::vector<std::vector<Type*>> deps = { { TypeId<Animator>() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);
  }

  FacialAnimator::FacialAnimator(Composition *aOwner, Space *aSpace)
    : Component(aOwner, aSpace)
    , mAnimator(nullptr)
    , mModel(nullptr)
  {
  }

  FacialAnimator::~FacialAnimator()
  {
  }

  void FacialAnimator::Initialize()
  {
    mModel = mOwner->GetComponent<Model>();
    mAnimator = mOwner->GetComponent<Animator>();

    mModel->GetMesh()->ResetTextureCoordinates();
    RefreshInitialBufffers();

    auto animations = mAnimator->GetAnimations();

    for (auto [key, animation] : animations)
    {
      mFaceAnimations[key] = std::make_unique<FaceAnim>(key, animation->GetTicksPerSecond());
    }
    
    mOwner->RegisterEvent<&FacialAnimator::OnModelChanged>(Events::ModelChanged, this);
    mOwner->RegisterEvent<&FacialAnimator::OnKeyFrameChanged>(Events::KeyFrameChanged, this);
    mOwner->RegisterEvent<&FacialAnimator::OnAnimationAdded>(Events::AnimationAdded, this);
    mOwner->RegisterEvent<&FacialAnimator::OnAnimationRemoved>(Events::AnimationRemoved, this);
  }

  void FacialAnimator::OnModelChanged(ModelChanged *aEvent)
  {
    UnusedArguments(aEvent);
    RefreshInitialBufffers();
  }

  void FacialAnimator::OnKeyFrameChanged(KeyFrameChanged *aEvent)
  {
    // get initial buffers
    InstantiatedModel *instModel = mModel->GetInstantiatedModel()[0];
    
    FaceAnim *anim = mFaceAnimations[aEvent->animation].get();

    Mesh *mesh = instModel->GetMesh();

    for (auto&& [submesh, i] : enumerate(mesh->mParts))
    {
      FaceFrame* frame{ nullptr };
      std::vector<Vertex> vertexBuffer;
      
      if (submesh->mData.mMaterialName == "OnlyDiff_Eye")
      {
        frame = FindFrame(anim->eyeFrames, aEvent->time * anim->ticksPerSecond);
        vertexBuffer = mInitialEyeVertexBuffer;
      }
      else if (submesh->mData.mMaterialName == "OnlyDiff_Mouth")
      {
        frame = FindFrame(anim->mouthFrames, aEvent->time * anim->ticksPerSecond);
        vertexBuffer = mInitialMouthVertexBuffer;
      }
      
      if (frame)
      {
        for (auto& vertex : vertexBuffer)
        {
          vertex.mTextureCoordinates.x += frame->uv.x;
          vertex.mTextureCoordinates.y += frame->uv.y;
        }
      
        mesh->UpdateVertices(i, vertexBuffer);
      }
    }
  }

  void FacialAnimator::OnAnimationAdded(AnimationAdded *event)
  {
    std::string anim = event->animation;
    mFaceAnimations[anim] = std::make_unique<FaceAnim>(anim, event->ticksPerSecond);
  }

  void FacialAnimator::OnAnimationRemoved(AnimationRemoved *event)
  {
    mFaceAnimations.erase(event->animation);
  }

  void FacialAnimator::RefreshInitialBufffers()
  {
    // get initial buffers
    auto models = mModel->GetInstantiatedModel();

    if (models.empty())
    {
      return;
    }

    auto instModel = models[0];

    Mesh *mesh = instModel->GetMesh();

    for (int i = 0; i < mesh->mParts.size(); i++)
    {
      Submesh &submesh = mesh->mParts[i];

      if (submesh.mData.mMaterialName == "OnlyDiff_Eye")
      {
        mEyeBufferIndex = i;
        mInitialEyeVertexBuffer = submesh.mData.mVertexData;
      }
      else if (submesh.mData.mMaterialName == "OnlyDiff_Mouth")
      {
        mMouthBufferIndex = i;
        mInitialMouthVertexBuffer = submesh.mData.mVertexData;
      }
    }

    instModel->mType = ShaderType::AlphaBlendShader;
  }

  FaceFrame* FacialAnimator::FindFrame(std::vector<FaceFrame>& aFrames, double time)
  {
    for (auto& frame : aFrames)
    {
      if (frame.time > time)
      {
        return &frame;
      }
    }

    return nullptr;
  }

  FaceAnim::FaceAnim(std::string animationFilename, double ticksPerSecond)
    : ticksPerSecond(ticksPerSecond)
  {
    fs::path workingDir{ YTE::Path::GetGamePath().String() };
    fs::path assetsDir{ workingDir.parent_path() };
    std::string animDir = assetsDir.string() + "/Animations/" + animationFilename;

    // check to see if there is a facial animation for this animation
    std::filesystem::path animPath(animDir);
    animPath.replace_extension("");

    std::string eyePath(animPath.string() + "_EyeAnim.txt");
    std::string mouthPath(animPath.string() + "_MouthAnim.txt");
    std::ifstream eyeFile(eyePath);
    std::ifstream mouthFile(mouthPath);

    if (eyeFile.is_open() && mouthFile.is_open())
    {
      // read eye frames
      std::string eyeLine;

      while (getline(eyeFile, eyeLine))
      {
        // parse line
        auto values = split(eyeLine, '\t', true);

        if (values.size() == 4)
        {
          FaceFrame eyeFrame;
          eyeFrame.id = std::stoi(values[0]);
          eyeFrame.time = std::stoi(values[1]);

          float u = std::stof(values[2]);

          if (u < 0.0f)
          {
            u = 1.0f - u;
          }
          eyeFrame.uv.x = u;

          float v = std::stof(values[3]);

          if (v < 0.0f)
          {
            v = 1.0f - v;
          }
          eyeFrame.uv.y = v;

          eyeFrames.push_back(eyeFrame);
        }
      }

      // read mouth frames
      std::string mouthLine;

      while (getline(mouthFile, mouthLine))
      {
        // parse line
        auto values = split(mouthLine, '\t', true);

        if (values.size() == 4)
        {
          FaceFrame mouthFrame;
          mouthFrame.id = std::stoi(values[0]);
          mouthFrame.time = std::stoi(values[1]);
          
          float u = std::stof(values[2]);
          
          if (u < 0.0f)
          {
            u = 1.0f - u;
          }
          mouthFrame.uv.x = u;
          
          float v = std::stof(values[3]);

          if (v < 0.0f)
          {
            v = 1.0f - v;
          }
          mouthFrame.uv.y = v;

          mouthFrames.push_back(mouthFrame);
        }
      }
    }

    // close our files
    eyeFile.close();
    mouthFile.close();
  }
}
