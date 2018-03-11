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

    std::vector<std::vector<Type*>> deps = { { Animator::GetStaticType() } };

    FacialAnimator::GetStaticType()->AddAttribute<ComponentDependencies>(deps);
  }

  FacialAnimator::FacialAnimator(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mAnimator(nullptr)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  FacialAnimator::~FacialAnimator()
  {
  }

  void FacialAnimator::Initialize()
  {
    mAnimator = mOwner->GetComponent<Animator>();

    auto animations = mAnimator->GetAnimations();

    for (auto &anim : animations)
    {
      mFaceAnimations.insert_or_assign(anim.first, FaceAnim(anim.first));
    }
    
    // register for key frame change
    mOwner->YTERegister(Events::KeyFrameChanged, this, &OnKeyFrameChange);
    mOwner->YTERegister(Events::KeyFrameChanged, this, &OnKeyFrameChange);
    mOwner->YTERegister(Events::KeyFrameChanged, this, &OnKeyFrameChange);
  }

  void FacialAnimator::OnKeyFrameChange(KeyFrameChanged *event)
  {


  }

  void FacialAnimator::OnAnimationAdded(AnimationAdded *event)
  {
    std::string anim = event->animation;
    mFaceAnimations.insert_or_assign(anim, FaceAnim(anim));
  }

  void FacialAnimator::OnAnimationRemoved(AnimationRemoved *event)
  {
    mFaceAnimations.erase(event->animation);
  }

  FaceAnim::FaceAnim(std::string animationFilename)
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
          eyeFrame.uv.x = std::stof(values[2]);
          eyeFrame.uv.y = std::stof(values[3]);

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
          mouthFrame.uv.x = std::stof(values[2]);
          mouthFrame.uv.y = std::stof(values[3]);

          mouthFrames.push_back(mouthFrame);
        }
      }
    }

    // close our files
    eyeFile.close();
    mouthFile.close();
  }

  /*
      // cause update to graphics card
      auto instModel = mModel->GetInstantiatedModel()[0];

      instModel->UpdateUBOAnimation(mCurrentAnimation->GetUBOAnim());

      if (mCurrentAnimation->HasFaceAnim())
      {
        // get the submesh
        Mesh *mesh = instModel->GetMesh();
        Submesh* eyePlate;
        int eyeIndex;
        Submesh* mouthPlate;
        int mouthIndex;

        for (int i = 0; i < mesh->mParts.size(); i++)
        {
          Submesh &submesh = mesh->mParts[i];

          if (submesh.mMaterialName == "OnlyDiff_Eye")
          {
            eyePlate = &submesh;
            eyeIndex = i;
          }
          else if (submesh.mMaterialName == "OnlyDiff_Mouth")
          {
            mouthPlate = &submesh;
            mouthIndex = i;
          }
        }

        // make a copy of the vertex buffer
        auto vBuff = mouthPlate->mVertexBuffer;

        // offset its texture coordinates


        // call mesh->updates vertices, pass submesh index and vertex buffer
      }

    */
}
