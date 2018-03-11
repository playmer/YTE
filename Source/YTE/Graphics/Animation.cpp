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
#include "YTE/Graphics/Generics/InstantiatedModel.hpp"
#include "YTE/Graphics/Model.hpp"

#include "YTE/Utilities/Utilities.hpp"

namespace YTE
{
  YTEDefineEvent(KeyFrameChanged);

  YTEDefineType(KeyFrameChanged)
  {
    YTERegisterType(KeyFrameChanged);
    YTEBindField(&KeyFrameChanged::animation, "animation", PropertyBinding::Get);
    YTEBindField(&KeyFrameChanged::time, "time", PropertyBinding::Get);
  }

  YTEDefineEvent(AnimationAdded);

  YTEDefineType(AnimationAdded)
  {
    YTERegisterType(AnimationAdded);
    YTEBindField(&AnimationAdded::animation, "animation", PropertyBinding::Get);
  }

  YTEDefineEvent(AnimationRemoved);

  YTEDefineType(AnimationRemoved)
  {
    YTERegisterType(AnimationRemoved);
    YTEBindField(&AnimationRemoved::animation, "animation", PropertyBinding::Get);
  }


  YTEDefineType(Animation)
  {
    YTERegisterType(Animation);

    std::vector<std::vector<Type*>> deps = { { Animator::GetStaticType() } };

    Animation::GetStaticType()->AddAttribute<ComponentDependencies>(deps);

    YTEBindProperty(&Animation::GetSpeed, &Animation::SetSpeed, "Speed")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .SetDocumentation("The speed at which the animation will be played at.");

    YTEBindProperty(&Animation::GetPlayOverTime, &Animation::SetPlayOverTime, "PlayOverTime")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .SetDocumentation("True if the animation should play with respect to time.");
  }

  Animation::Animation(std::string &aFile, uint32_t aAnimationIndex)
    : mPlayOverTime(true)
  {
    Assimp::Importer importer;
    auto aniFile = Path::GetAnimationPath(Path::GetGamePath(), aFile);
    auto scene = importer.ReadFile(aniFile.c_str(),
      aiProcess_Triangulate |
      aiProcess_CalcTangentSpace |
      aiProcess_GenSmoothNormals);

    YTEUnusedArgument(scene);

    DebugObjection(scene == nullptr,
      "Failed to load animation file %s from assimp",
      aniFile.c_str());

    //TODO (Andrew): Dont keep the scene loaded
    mScene = importer.GetOrphanedScene();

    DebugObjection(mScene == nullptr,
      "Failed to load orphaned scene");

    DebugObjection(mScene->HasAnimations() == false,
      "Failed to find animations in scene loaded from %s",
      aniFile.c_str());

    mAnimationIndex = aAnimationIndex;

    mAnimation = mScene->mAnimations[0];

    mName = mAnimation->mName.data;
    mCurrentAnimationTime = 0.0f;

    mUBOAnimationData.mHasAnimation = true;
    mElapsedTime = 0.0;

    mSpeed = 1.0f;
  }

  void Animation::Initialize(Model *aModel, Engine *aEngine)
  {
    mEngine = aEngine;
    mModel = aModel;
    mMeshSkeleton = &mModel->GetMesh()->mSkeleton;
  }

  Animation::~Animation()
  {
    if (mScene)
    {
      delete mScene;
    }
  }

  void Animation::SetCurrentTime(double aCurrentTime)
  {
    aCurrentTime *= mAnimation->mTicksPerSecond;

    if (0.0 < aCurrentTime && aCurrentTime < mAnimation->mDuration)
    {
      mCurrentAnimationTime = aCurrentTime;
    }
  }

  double Animation::GetMaxTime() const
  {
    return mAnimation->mDuration / mAnimation->mTicksPerSecond;
  }

  float Animation::GetSpeed() const
  {
    return static_cast<float>(mSpeed);
  }

  void Animation::SetSpeed(float aSpeed)
  {
    mSpeed = static_cast<double>(aSpeed);
  }

  bool Animation::GetPlayOverTime() const
  {
    return mPlayOverTime;
  }

  void Animation::SetPlayOverTime(bool aPlayOverTime)
  {
    mPlayOverTime = aPlayOverTime;
  }

  aiAnimation* Animation::GetAnimation()
  {
    return mAnimation;
  }

  void Animation::ReadAnimation(aiNode *aNode, aiMatrix4x4 &aParentTransform)
  {
    aiMatrix4x4 NodeTransformation(aNode->mTransformation);

    const aiNodeAnim* pNodeAnim = FindNodeAnimation(aNode->mName.data);

    if (pNodeAnim)
    {
      int numKeys = pNodeAnim->mNumPositionKeys;

      auto startKey = pNodeAnim->mPositionKeys[0];
      auto endKey = pNodeAnim->mPositionKeys[numKeys - 1];

      double duration = (endKey.mTime - startKey.mTime);

      if (duration != 0.0f)
      {
        mAnimation->mDuration = duration;
      }

      // Get interpolated matrices between current and next frame
      aiMatrix4x4 matScale = ScaleInterpolation(pNodeAnim);
      aiMatrix4x4 matRotation = RotationInterpolation(pNodeAnim);
      aiMatrix4x4 matTranslation = TranslationInterpolation(pNodeAnim);

      NodeTransformation = matTranslation * matRotation * matScale;
    }

    aiMatrix4x4 GlobalTransformation = aParentTransform * NodeTransformation;

    auto* bones = mMeshSkeleton->GetBones();
    auto bone = bones->find(aNode->mName.data);
    if (bone != bones->end())
    {
      uint32_t BoneIndex = bone->second;
      mMeshSkeleton->GetBoneData()[BoneIndex].mFinalTransformation =
        mMeshSkeleton->GetGlobalInverseTransform() *
        GlobalTransformation *
        mMeshSkeleton->GetBoneData()[BoneIndex].mOffset;
    }

    // visit the rest of the bone children
    for (uint32_t i = 0; i < aNode->mNumChildren; ++i)
    {
      ReadAnimation(aNode->mChildren[i], GlobalTransformation);
    }
  }

  Skeleton* Animation::GetSkeleton()
  {
    return mMeshSkeleton;
  }

  UBOAnimation* Animation::GetUBOAnim()
  {
    return &mUBOAnimationData;
  }

  aiScene* Animation::GetScene()
  {
    return mScene;
  }


  aiMatrix4x4 Animation::ScaleInterpolation(const aiNodeAnim* aNode)
  {
    aiVector3D scale;
    if (aNode->mNumScalingKeys == 1)
    {
      scale = aNode->mScalingKeys[0].mValue;
    }
    else
    {
      auto startFrame = aNode->mScalingKeys[0];

      uint32_t index = 0;
      for (uint32_t i = 0; i < aNode->mNumScalingKeys - 1; ++i)
      {
        if (mCurrentAnimationTime < (float)aNode->mScalingKeys[i + 1].mTime - startFrame.mTime)
        {
          index = i;
          break;
        }
      }

      aiVectorKey frame = aNode->mScalingKeys[index];
      aiVectorKey nextFrame = aNode->mScalingKeys[(index + 1) % aNode->mNumScalingKeys];

      float delta = static_cast<float>((mCurrentAnimationTime + startFrame.mTime - frame.mTime) / (nextFrame.mTime - frame.mTime));

      const aiVector3D& start = frame.mValue;
      const aiVector3D& end = nextFrame.mValue;

      scale = (start + delta * (end - start));
    }

    aiMatrix4x4 mat;
    aiMatrix4x4::Scaling(scale, mat);
    return mat;
  }



  aiMatrix4x4 Animation::RotationInterpolation(const aiNodeAnim* aNode)
  {
    aiQuaternion rot;

    if (aNode->mNumRotationKeys == 1)
    {
      rot = aNode->mRotationKeys[0].mValue;
    }
    else
    {
      auto startFrame = aNode->mRotationKeys[0];

      // TODO (Andrew): Can we resuse the keys between scale translate and rotation? is the index the same?
      uint32_t index = 0;
      for (uint32_t i = 0; i < aNode->mNumRotationKeys - 1; ++i)
      {
        if (mCurrentAnimationTime < (float)aNode->mRotationKeys[i + 1].mTime - startFrame.mTime)
        {
          index = i;
          break;
        }
      }

      aiQuatKey frame = aNode->mRotationKeys[index];
      aiQuatKey nextFrame = aNode->mRotationKeys[(index + 1) % aNode->mNumRotationKeys];

      float delta = static_cast<float>((mCurrentAnimationTime + startFrame.mTime - frame.mTime) / (nextFrame.mTime - frame.mTime));

      const aiQuaternion& start = frame.mValue;
      const aiQuaternion& end = nextFrame.mValue;

      aiQuaternion::Interpolate(rot, start, end, delta);
      rot.Normalize();
    }

    return aiMatrix4x4(rot.GetMatrix());
  }



  aiMatrix4x4 Animation::TranslationInterpolation(const aiNodeAnim *aNode)
  {
    aiVector3D trans;
    if (aNode->mNumPositionKeys == 1)
    {
      trans = aNode->mPositionKeys[0].mValue;
    }
    else
    {
      auto startFrame = aNode->mPositionKeys[0];

      uint32_t index = 0;
      for (uint32_t i = 0; i < aNode->mNumPositionKeys - 1; ++i)
      {
        if (mCurrentAnimationTime < (float)aNode->mPositionKeys[i + 1].mTime - startFrame.mTime)
        {
          index = i;
          break;
        }
      }

      aiVectorKey frame = aNode->mPositionKeys[index];
      aiVectorKey nextFrame = aNode->mPositionKeys[(index + 1) % aNode->mNumPositionKeys];

      float delta = static_cast<float>((mCurrentAnimationTime + startFrame.mTime - frame.mTime) / (nextFrame.mTime - frame.mTime));

      const aiVector3D& start = frame.mValue;
      const aiVector3D& end = nextFrame.mValue;

      trans = (start + delta * (end - start));
    }

    aiMatrix4x4 mat;
    aiMatrix4x4::Translation(trans, mat);
    return mat;
  }

  const aiNodeAnim* Animation::FindNodeAnimation(const char *aName)
  {
    for (uint32_t i = 0; i < mAnimation->mNumChannels; ++i)
    {
      const aiNodeAnim *anim = mAnimation->mChannels[i];

      if (!strcmp(anim->mNodeName.data, aName))
      {
        return anim;
      }
    }
    return nullptr;
  }



  YTEDefineType(Animator)
  {
    YTERegisterType(Animator);

    Animator::GetStaticType()->AddAttribute<EditorHeaderList>(&Deserializer,
      &Serializer,
      &Lister,
      "Animations");

    std::vector<std::vector<Type*>> deps = { { Model::GetStaticType() } };

    Animator::GetStaticType()->AddAttribute<ComponentDependencies>(deps);
  }

  Animator::Animator(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mDefaultAnimation(nullptr)
    , mCurrentAnimation(nullptr)
    , mNextAnimation(nullptr)
  {
    DeserializeByType(aProperties, this, GetStaticType());
    mEngine = aSpace->GetEngine();
  }

  Animator::~Animator()
  {
    if (mOwner->GetIsBeingDeleted() == false)
    {
      if (mModel->GetInstantiatedModel().size())
      {
        mModel->GetInstantiatedModel()[0]->SetDefaultAnimationOffset();
      }
    }

    for (auto it : mAnimations)
    {
      delete it.second;
    }

    mAnimations.clear();
  }

  void Animator::Initialize()
  {
    mModel = mOwner->GetComponent<Model>();

    for (auto it : mAnimations)
    {
      it.second->Initialize(mModel, mEngine);
    }

    mEngine->YTERegister(Events::AnimationUpdate, this, &Animator::Update);
  }

  void Animator::Update(LogicUpdate *aEvent)
  {
    if (!mCurrentAnimation)
    {
      if (mNextAnimation)
      {
        mCurrentAnimation = mNextAnimation;
        mNextAnimation = nullptr;
      }
      else if (mDefaultAnimation)
      {
        mCurrentAnimation = mDefaultAnimation;
      }
      else
      {
        return;
      }
    }

    if (mCurrentAnimation->mPlayOverTime)
    {
      mCurrentAnimation->mElapsedTime += aEvent->Dt * mCurrentAnimation->mSpeed;

      mCurrentAnimation->mCurrentAnimationTime = fmodf(static_cast<float>(mCurrentAnimation->mElapsedTime * mCurrentAnimation->GetAnimation()->mTicksPerSecond),
        static_cast<float>(mCurrentAnimation->GetAnimation()->mDuration));

      if (mCurrentAnimation->mElapsedTime * mCurrentAnimation->GetAnimation()->mTicksPerSecond > mCurrentAnimation->GetAnimation()->mDuration)
      {
        mCurrentAnimation->mCurrentAnimationTime = 0.0;
        mCurrentAnimation->mElapsedTime = 0.0;

        // set to nullptr, will switch to appropriate animation at beginning of next update loop
        mCurrentAnimation = nullptr;
      }
    }

    if (mCurrentAnimation)
    {
      aiMatrix4x4 identity = aiMatrix4x4();
      mCurrentAnimation->ReadAnimation(mCurrentAnimation->GetScene()->mRootNode, identity);

      for (int i = 0; i < mCurrentAnimation->GetSkeleton()->GetBoneData().size(); ++i)
      {
        mCurrentAnimation->GetUBOAnim()->mBones[i] = AssimpToGLM(mCurrentAnimation->GetSkeleton()->GetBoneData()[i].mFinalTransformation);
      }

      // cause update to graphics card
      mModel->GetInstantiatedModel()[0]->UpdateUBOAnimation(mCurrentAnimation->GetUBOAnim());
    }
  }

  void Animator::PlayAnimation(std::string aAnimation)
  {
    auto it = mAnimations.find(aAnimation);

    // animation doesn't exist on this animator component
    if (it == mAnimations.end())
    {
      return;
    }

    mNextAnimation = it->second;
  }

  void Animator::SetDefaultAnimation(std::string aAnimation)
  {
    auto it = mAnimations.find(aAnimation);

    // animation doesn't exist on this animator component
    if (it == mAnimations.end())
    {
      return;
    }

    mDefaultAnimation = it->second;
  }

  void Animator::SetCurrentAnimation(std::string aAnimation)
  {
    auto it = mAnimations.find(aAnimation);

    // animation doesn't exist on this animator component
    if (it == mAnimations.end())
    {
      return;
    }

    mCurrentAnimation = it->second;
  }

  void Animator::SetCurrentPlayOverTime(bool aPlayOverTime)
  {
    mCurrentAnimation->SetPlayOverTime(aPlayOverTime);
  }

  void Animator::SetCurrentAnimTime(double aTime)
  {
    mCurrentAnimation->SetCurrentTime(aTime);
  }

  double Animator::GetMaxTime() const
  {
    return mCurrentAnimation->GetMaxTime();
  }

  std::map<std::string, Animation*>& Animator::GetAnimations()
  {
    return mAnimations;
  }

  std::vector<std::pair<YTE::Object*, std::string>> Animator::Lister(YTE::Object *aSelf)
  {
    auto self = static_cast<Animator*>(aSelf);

    std::vector<std::pair<YTE::Object*, std::string>> animations;

    for (auto &animation : self->mAnimations)
    {
      std::string name = animation.first;
      Animation *anim = animation.second;

      animations.emplace_back(std::make_pair(anim, name));
    }

    return animations;
  }

  Animation* Animator::AddAnimation(std::string aName)
  {
    Animation* anim = InternalAddAnimation(aName);

    anim->Initialize(mOwner->GetComponent<Model>(), mEngine);

    return anim;
  }

  Animation* Animator::InternalAddAnimation(std::string aName)
  {
    std::experimental::filesystem::path path(aName);
    std::string exten = path.extension().generic_string();

    if (exten != ".fbx")
    {
      return nullptr;
    }

    Animation *anim = new Animation(aName);

    mAnimations.insert_or_assign(aName, anim);

    return anim;
  }

  void Animator::RemoveAnimation(Animation *aAnimation)
  {
    for (auto it = mAnimations.begin(); it != mAnimations.end(); ++it)
    {
      if (it->second == aAnimation)
      {
        delete it->second;
        mAnimations.erase(it);
        return;
      }
    }
  }

  RSValue Animator::Serializer(RSAllocator &aAllocator, Object *aOwner)
  {
    auto owner = static_cast<Animator*>(aOwner);

    RSValue animations;
    animations.SetObject();
    for (auto &animationIt : owner->mAnimations)
    {
      auto materialSerialized = SerializeByType(aAllocator,
        animationIt.second,
        TypeId<Animation>());

      RSValue materialName;

      auto &name = animationIt.first;
      materialName.SetString(name.c_str(),
        static_cast<RSSizeType>(name.size()),
        aAllocator);

      animations.AddMember(materialName, materialSerialized, aAllocator);
    }

    return animations;
  }

  void Animator::Deserializer(RSValue &aValue, Object *aOwner)
  {
    auto owner = static_cast<Animator*>(aOwner);

    for (auto valueIt = aValue.MemberBegin(); valueIt < aValue.MemberEnd(); ++valueIt)
    {
      auto animation = owner->InternalAddAnimation(valueIt->name.GetString());

      DeserializeByType(&(valueIt->value), animation, animation->GetType());
    }
  }
}
