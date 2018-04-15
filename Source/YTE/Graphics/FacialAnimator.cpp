///////////////////
// Author: Andrew Griffin
// YTE - Graphics
///////////////////

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

namespace fs = std::experimental::filesystem;

namespace YTE
{
  YTEDefineType(FacialAnimator)
  {
    YTERegisterType(FacialAnimator);

    std::vector<std::vector<Type*>> deps = { { TypeId<Animator>() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);
  }

  FacialAnimator::FacialAnimator(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mAnimator(nullptr)
    , mModel(nullptr)
  {
    DeserializeByType(aProperties, this, GetStaticType());
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

    for (auto &anim : animations)
    {
      mFaceAnimations.insert_or_assign(anim.first, new FaceAnim(anim.first, anim.second->GetAnimation()->mTicksPerSecond));
    }
    
    mOwner->YTERegister(Events::ModelChanged, this, &FacialAnimator::OnModelChanged);
    mOwner->YTERegister(Events::KeyFrameChanged, this, &FacialAnimator::OnKeyFrameChanged);
    mOwner->YTERegister(Events::AnimationAdded, this, &FacialAnimator::OnAnimationAdded);
    mOwner->YTERegister(Events::AnimationRemoved, this, &FacialAnimator::OnAnimationRemoved);
  }

  void FacialAnimator::OnModelChanged(ModelChanged *aEvent)
  {
    YTEUnusedArgument(aEvent);
    RefreshInitialBufffers();
  }

  void FacialAnimator::OnKeyFrameChanged(KeyFrameChanged *aEvent)
  {
    // get initial buffers
    InstantiatedModel *instModel = mModel->GetInstantiatedModel()[0];
    
    FaceAnim *anim = mFaceAnimations[aEvent->animation];

    Mesh *mesh = instModel->GetMesh();

    for (int i = 0; i < mesh->mParts.size(); i++)
    {
      Submesh &submesh = mesh->mParts[i];

      if (submesh.mMaterialName == "OnlyDiff_Eye")
      {
        FaceFrame *eyeFrame = FindEyeFrame(anim, aEvent->time * anim->ticksPerSecond);

        if (eyeFrame)
        {
          std::vector<Vertex> eyeBuffer = mInitialEyeVertexBuffer;

          for (int j = 0; j < mInitialEyeVertexBuffer.size(); j++)
          {
            eyeBuffer[j].mTextureCoordinates.x += eyeFrame->uv.x;
            eyeBuffer[j].mTextureCoordinates.y += eyeFrame->uv.y;
          }

          mesh->UpdateVertices(i, eyeBuffer);
        }
      }
      else if (submesh.mMaterialName == "OnlyDiff_Mouth")
      {
        FaceFrame *mouthFrame = FindMouthFrame(anim, aEvent->time * anim->ticksPerSecond);

        if (mouthFrame)
        {
          std::vector<Vertex> mouthBuffer = mInitialMouthVertexBuffer;

          for (int j = 0; j < mInitialMouthVertexBuffer.size(); j++)
          {
            mouthBuffer[j].mTextureCoordinates.x += mouthFrame->uv.x;
            mouthBuffer[j].mTextureCoordinates.y += mouthFrame->uv.y;
          }

          mesh->UpdateVertices(i, mouthBuffer);
        }
      }
    }
  }

  void FacialAnimator::OnAnimationAdded(AnimationAdded *event)
  {
    std::string anim = event->animation;
    mFaceAnimations.insert_or_assign(anim, new FaceAnim(anim, event->ticksPerSecond));
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

      if (submesh.mMaterialName == "OnlyDiff_Eye")
      {
        mEyeBufferIndex = i;
        mInitialEyeVertexBuffer = submesh.mVertexBuffer;
      }
      else if (submesh.mMaterialName == "OnlyDiff_Mouth")
      {
        mMouthBufferIndex = i;
        mInitialMouthVertexBuffer = submesh.mVertexBuffer;
      }
    }

    instModel->mType = ShaderType::AlphaBlendShader;
  }

  FaceFrame* FacialAnimator::FindEyeFrame(FaceAnim *anim, double time)
  {
    // find correct frame
    for (int k = 0; k < anim->eyeFrames.size(); k++)
    {
      if (anim->eyeFrames[k].time > time)
      {
        return &anim->eyeFrames[k];
      }
    }

    return nullptr;
  }

  FaceFrame* FacialAnimator::FindMouthFrame(FaceAnim *anim, double time)
  {
    // find correct frame
    for (int k = 0; k < anim->mouthFrames.size(); k++)
    {
      if (anim->mouthFrames[k].time > time)
      {
        return &anim->mouthFrames[k];
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
    std::experimental::filesystem::path animPath(animDir);
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
