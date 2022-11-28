#include <algorithm>

#include "glm/gtc/type_ptr.hpp"

#include "YTE/Core/AssetLoader.hpp"
#include "YTE/Core/Engine.hpp"

#include "YTE/Graphics/Animation.hpp"
#include "YTE/Graphics/Generics/InstantiatedModel.hpp"
#include "YTE/Graphics/Model.hpp"

#include "YTE/StandardLibrary/File.hpp"

#include "YTE/Utilities/Utilities.hpp"

namespace YTE
{
//  static inline
//  glm::vec3 ToGlm(aiVector3D const* aVector)
//  {
//    return { aVector->x, aVector->y ,aVector->z };
//  }
//
//  static inline
//  glm::vec3 ToGlm(aiColor3D const* aVector)
//  {
//    return { aVector->r, aVector->g ,aVector->b };
//  }
//
//  static inline
//  glm::quat ToGlm(aiQuaternion const* aQuat)
//  {
//    glm::quat quaternion;
//
//    quaternion.x = aQuat->x;
//    quaternion.y = aQuat->y;
//    quaternion.z = aQuat->z;
//    quaternion.w = aQuat->w;
//
//    return quaternion;
//  }
//
//  static inline
//  glm::mat4 ToGlm(const aiMatrix4x4 &aMatrix)
//  {
//    return glm::transpose(glm::make_mat4(&aMatrix.a1));
//  }
//
//  static inline
//  aiMatrix4x4 ToAssimp(const glm::mat4 &aMatrix)
//  {
//    auto transposed = glm::transpose(aMatrix);
//    return *(reinterpret_cast<aiMatrix4x4*>(glm::value_ptr(transposed)));
//  }

  std::pair<AnimationData, std::vector<char>> ReadAnimationDataFromFile(std::string const& aName)
  {
    OPTICK_EVENT();
    AnimationData data;
    FileReader file{ aName };

    if (file.mOpened)
    {
      auto header = file.Read<AnimationFileHeader>();

      data.mDuration = header.mDuration;
      data.mTicksPerSecond = header.mTicksPerSecond;

      data.mNodes.resize(header.mNodeSize);
      file.Read<AnimationData::Node>(data.mNodes.data(), data.mNodes.size());

      data.mTransformations.resize(header.mTransformationsSize);
      file.Read<glm::mat4>(data.mTransformations.data(), data.mTransformations.size());

      data.mChildren.resize(header.mChildrenSize);
      file.Read<size_t>(data.mChildren.data(), data.mChildren.size());

      data.mTranslationKeys.resize(header.mTranslationKeysSize);
      file.Read<AnimationData::TranslationKey>(data.mTranslationKeys.data(), data.mTranslationKeys.size());

      data.mScaleKeys.resize(header.mScaleKeysSize);
      file.Read<AnimationData::ScaleKey>(data.mScaleKeys.data(), data.mScaleKeys.size());

      data.mRotationKeys.resize(header.mRotationKeysSize);
      file.Read<AnimationData::RotationKey>(data.mRotationKeys.data(), data.mRotationKeys.size());

      data.mNames.resize(header.mNamesSize);
      file.Read<char>(data.mNames.data(), data.mNames.size());
    }

    return { std::move(data), std::move(file.mData) };
  }


  static inline 
  glm::vec3 ScaleInterpolation(AnimationData const& aData, 
                               double aAnimationTime, 
                               AnimationData::Node const& aNode)
  {
    OPTICK_EVENT();

    glm::vec3 scale;
    if (aNode.mScaleKeySize == 1)
    {
      scale = aData.mScaleKeys[0].mScale;
    }
    else
    {
      auto startFrame = aData.mScaleKeys[0];

      uint32_t index = 0;
      for (uint32_t i = 0; i < aNode.mScaleKeySize - 1; ++i)
      {
        if (aAnimationTime < (float)aData.mScaleKeys[aNode.mScaleKeyOffset + i + 1].mTime - startFrame.mTime)
        {
          index = i;
          break;
        }
      }
      
      auto frame = aData.mScaleKeys[aNode.mScaleKeyOffset + index];
      auto nextFrame = aData.mScaleKeys[aNode.mScaleKeyOffset + ((index + 1) % aNode.mScaleKeySize)];

      float delta = static_cast<float>((aAnimationTime + startFrame.mTime - frame.mTime) / (nextFrame.mTime - frame.mTime));

      auto const& start = frame.mScale;
      auto const& end = nextFrame.mScale;

      scale = (start + delta * (end - start));
    }

    return scale;
  }

  static inline
  glm::quat RotationInterpolation(AnimationData const& aData,
                                  double aAnimationTime,
                                  AnimationData::Node const& aNode)
  {
    OPTICK_EVENT();

    glm::quat rot;

    if (aNode.mRotationKeySize == 1)
    {
      rot = aData.mRotationKeys[aNode.mRotationKeyOffset + 0].mRotation;
    }
    else
    {
      auto startFrame = aData.mRotationKeys[aNode.mRotationKeyOffset + 0];

      // TODO (Andrew): Can we resuse the keys between scale translate and rotation? is the index the same?
      uint32_t index = 0;
      for (uint32_t i = 0; i < aNode.mRotationKeySize - 1; ++i)
      {
        if (aAnimationTime < (float)aData.mRotationKeys[aNode.mRotationKeyOffset + i + 1].mTime - startFrame.mTime)
        {
          index = i;
          break;
        }
      }

      auto frame = aData.mRotationKeys[aNode.mRotationKeyOffset + index];
      auto nextFrame = aData.mRotationKeys[aNode.mRotationKeyOffset + ((index + 1) % aNode.mRotationKeySize)];

      float delta = static_cast<float>((aAnimationTime + startFrame.mTime - frame.mTime) / (nextFrame.mTime - frame.mTime));

      auto const& start = frame.mRotation;
      auto const& end = nextFrame.mRotation;
      
      rot = animation_interpolate(start, end, delta);
      glm::normalize(rot);
    }

    return rot;
  }

  static inline
  glm::vec3 TranslationInterpolation(AnimationData const& aData,
                                     double aAnimationTime,
                                     AnimationData::Node const& aNode)
  {
    OPTICK_EVENT();

    glm::vec3 trans;
    if (1 == aNode.mTranslationKeySize)
    {
      trans = aData.mTranslationKeys[aNode.mTranslationKeyOffset + 0].mTranslation;
    }
    else
    {
      auto startFrame = aData.mTranslationKeys[aNode.mTranslationKeyOffset + 0];

      uint32_t index = 0;
      for (uint32_t i = 0; i < aNode.mTranslationKeySize - 1; ++i)
      {
        if (aAnimationTime < (float)aData.mTranslationKeys[aNode.mTranslationKeyOffset + i + 1].mTime - startFrame.mTime)
        {
          index = i;
          break;
        }
      }

      auto frame = aData.mTranslationKeys[aNode.mTranslationKeyOffset + index];
      auto nextFrame = aData.mTranslationKeys[aNode.mTranslationKeyOffset + ((index + 1) % aNode.mTranslationKeySize)];

      float delta = static_cast<float>((aAnimationTime + startFrame.mTime - frame.mTime) / (nextFrame.mTime - frame.mTime));

      auto const& start = frame.mTranslation;
      auto const& end = nextFrame.mTranslation;

      trans = (start + delta * (end - start));
    }

    return trans;
  }








  YTEDefineEvent(KeyFrameChanged);

  YTEDefineType(KeyFrameChanged)
  {
    RegisterType<KeyFrameChanged>();
    TypeBuilder<KeyFrameChanged> builder;
    builder.Field<&KeyFrameChanged::animation>("animation", PropertyBinding::Get);
    builder.Field<&KeyFrameChanged::time>("time", PropertyBinding::Get);
  }

  YTEDefineEvent(AnimationAdded);

  YTEDefineType(AnimationAdded)
  {
    RegisterType<AnimationAdded>();
    TypeBuilder<AnimationAdded> builder;
    builder.Field<&AnimationAdded::animation>("animation", PropertyBinding::Get);
  }

  YTEDefineEvent(AnimationRemoved);

  YTEDefineType(AnimationRemoved)
  {
    RegisterType<AnimationRemoved>();
    TypeBuilder<AnimationRemoved> builder;
    builder.Field<&AnimationRemoved::animation>("animation", PropertyBinding::Get);
  }


  YTEDefineType(Animation)
  {
    RegisterType<Animation>();
    TypeBuilder<Animation> builder;

    std::vector<std::vector<Type*>> deps = { { TypeId<Animator>() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);

    builder.Property<&Animation::GetSpeed, &Animation::SetSpeed>("Speed")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .SetDocumentation("The speed at which the animation will be played at.");

    builder.Property<&Animation::GetPlayOverTime, &Animation::SetPlayOverTime>("PlayOverTime")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .SetDocumentation("True if the animation should play with respect to time.");
  }

  Animation::Animation(std::string &aFile, uint32_t aAnimationIndex)
    : mPlayOverTime{ true }
    , mData{ ReadAnimationFromFile(aFile) }
  {
    OPTICK_EVENT();
    mAnimationIndex = aAnimationIndex;

    mName = aFile;
    mCurrentAnimationTime = 0.0f;

    mElapsedTime = 0.0;

    mSpeed = 1.0f;
  }

  void Animation::Initialize(Model *aModel, Engine *aEngine)
  {
    OPTICK_EVENT();
    mEngine = aEngine;
    mModel = aModel;
    mMeshSkeleton = &mModel->GetMesh()->mSkeleton;
  }

  Animation::~Animation()
  {

  }

  void Animation::SetCurrentTime(double aCurrentTime)
  {
    aCurrentTime *= mData.mTicksPerSecond;

    if (0.0 < aCurrentTime && aCurrentTime < mData.mDuration)
    {
      mCurrentAnimationTime = aCurrentTime;
    }
  }

  double Animation::GetMaxTime() const
  {
    return mData.mDuration / mData.mTicksPerSecond;
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

  void Animation::Animate()
  {
    OPTICK_EVENT();
    glm::mat4 identity;
    ReadAnimation(mData.mNodes.front(), identity);
  }

  void Animation::ReadAnimation(AnimationData::Node const& aNode, 
                                glm::mat4 const& aParentTransform)
  {
    OPTICK_EVENT();
    glm::mat4 nodeTransformation = mData.mTransformations[aNode.mTransformationOffset];

    if (aNode.mTranslationKeySize &&
        aNode.mScaleKeySize && 
        aNode.mRotationKeySize)
    {
      OPTICK_EVENT("Animation Interpolation");
      auto numKeys = aNode.mTranslationKeySize;
    
      auto startKey = mData.mTranslationKeys[aNode.mTranslationKeyOffset + 0];
      auto endKey = mData.mTranslationKeys[aNode.mTranslationKeyOffset + numKeys - 1];
    
      double duration = (endKey.mTime - startKey.mTime);
    
      if (duration != 0.0f)
      {
        mData.mDuration = duration;
      }
    
      // Get interpolated matrices between current and next frame
      auto scale = ScaleInterpolation(mData, mCurrentAnimationTime, aNode);
      auto rotation = RotationInterpolation(mData, mCurrentAnimationTime, aNode);
      auto translation = TranslationInterpolation(mData, mCurrentAnimationTime, aNode);

      nodeTransformation = glm::scale(glm::toMat4(rotation), scale);
      nodeTransformation[3][0] = translation.x;
      nodeTransformation[3][1] = translation.y;
      nodeTransformation[3][2] = translation.z;
    }
    
    glm::mat4 globalTransformation = aParentTransform * nodeTransformation;

    {
      OPTICK_EVENT("Looking For Bone");

      auto name = std::string_view{ mData.mNames.data() + aNode.mNameOffset,
                                    aNode.mNameSize };

      auto bones = mMeshSkeleton->GetBones();
      auto bone = bones->find(name);
      if (bone != bones->end())
      {
        uint32_t boneIndex = bone->second;
        mMeshSkeleton->GetBoneData()[boneIndex].mFinalTransformation =
          mMeshSkeleton->GetGlobalInverseTransform() *
          globalTransformation *
          mMeshSkeleton->GetBoneData()[boneIndex].mOffset;
      }
    }
    
    // visit the rest of the bone children
    for (size_t i = 0; i < aNode.mChildrenSize; ++i)
    {
      // i + aData.mChildren[aNode.mChildrenOffset]
      ReadAnimation(mData.mNodes[mData.mChildren[i + aNode.mChildrenOffset]], 
                    globalTransformation);
    }
  }

  Skeleton* Animation::GetSkeleton()
  {
    return mMeshSkeleton;
  }

  UBOs::Animation* Animation::GetUBOAnim()
  {
    return &mUBOAnimationData;
  }

  YTEDefineType(Animator)
  {
    RegisterType<Animator>();
    TypeBuilder<Animator> builder;

    GetStaticType()->AddAttribute<EditorHeaderList>(&Deserializer,
                                                    &Serializer,
                                                    &Lister,
                                                    "Animations");

    std::vector<std::vector<Type*>> deps = { { TypeId<Model>() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);
  }

  Animator::Animator(Composition *aOwner, Space *aSpace)
    : Component(aOwner, aSpace)
    , mDefaultAnimation(nullptr)
    , mCurrentAnimation(nullptr)
  {
    OPTICK_EVENT();
    mEngine = aSpace->GetEngine();
  }

  Animator::~Animator()
  {

    for (auto it : mAnimations)
    {
      delete it.second;
    }

    mAnimations.clear();
  }

  void Animator::Initialize()
  {
    OPTICK_EVENT();
    mModel = mOwner->GetComponent<Model>();

    for (auto it : mAnimations)
    {
      it.second->Initialize(mModel, mEngine);
    }

    mEngine->RegisterEvent<&Animator::Update>(Events::AnimationUpdate, this);
  }
  
  void Animator::Deinitialize()
  {
    for (auto& model : mModel->GetInstantiatedModel())
    {
      model->SetDefaultAnimationOffset();
    }
  }

  void Animator::Update(LogicUpdate *aEvent)
  {
    OPTICK_EVENT();
    if (!mCurrentAnimation)
    {
      if (!mNextAnimations.empty())
      {
        mCurrentAnimation = mNextAnimations.front();
        mNextAnimations.pop();
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

      mCurrentAnimation->mCurrentAnimationTime = fmodf(static_cast<float>(mCurrentAnimation->mElapsedTime * mCurrentAnimation->GetTicksPerSecond()),
                                                       static_cast<float>(mCurrentAnimation->GetDuration()));

      if (mCurrentAnimation->mElapsedTime * mCurrentAnimation->GetTicksPerSecond() > mCurrentAnimation->GetDuration())
      {
        mCurrentAnimation->mCurrentAnimationTime = 0.0;
        mCurrentAnimation->mElapsedTime = 0.0;

        // set to nullptr, will switch to appropriate animation at beginning of next update loop
        mCurrentAnimation = nullptr;
      }
      else
      {
        KeyFrameChanged keyChange;
        keyChange.animation = mCurrentAnimation->mName;
        keyChange.time = mCurrentAnimation->mElapsedTime;

        mOwner->SendEvent(Events::KeyFrameChanged, &keyChange);
      }
    }

    if (mCurrentAnimation)
    {
      mCurrentAnimation->Animate();

      for (int i = 0; i < mCurrentAnimation->GetSkeleton()->GetBoneData().size(); ++i)
      {
        mCurrentAnimation->GetUBOAnim()->mBones[i] = mCurrentAnimation->GetSkeleton()->GetBoneData()[i].mFinalTransformation;
      }

      // cause update to graphics card
      mModel->GetInstantiatedModel()[0]->UpdateUBOAnimation(mCurrentAnimation->GetUBOAnim());
    }
  }

  void Animator::PlayAnimationSet(std::string aAnimation)
  {
    OPTICK_EVENT();
    std::string initFile = aAnimation + "_Init.YTEAnimation";
    std::string loopFile = aAnimation + "_Loop.YTEAnimation";
    std::string exitFile = aAnimation + "_Exit.YTEAnimation";

    AddNextAnimation(initFile);
    AddNextAnimation(loopFile);
    AddNextAnimation(exitFile);
  }

  void Animator::AddNextAnimation(std::string aAnimation)
  {
    OPTICK_EVENT();
    auto it = mAnimations.find(aAnimation);

    // animation doesn't exist on this animator component
    if (it == mAnimations.end())
    {
      std::cout << aAnimation << " not found\n";
      return;
    }

    mNextAnimations.push(it->second);
  }

  void Animator::SetDefaultAnimation(std::string aAnimation)
  {
    OPTICK_EVENT();
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
    OPTICK_EVENT();
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
    OPTICK_EVENT();
    mCurrentAnimation->SetPlayOverTime(aPlayOverTime);
  }

  void Animator::SetCurrentAnimTime(double aTime)
  {
    OPTICK_EVENT();
    mCurrentAnimation->SetCurrentTime(aTime);

    KeyFrameChanged keyChange;
    keyChange.animation = mCurrentAnimation->mName;
    keyChange.time = aTime;

    mOwner->SendEvent(Events::KeyFrameChanged, &keyChange);
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
    OPTICK_EVENT();
    Animation* anim = InternalAddAnimation(aName);

    anim->Initialize(mOwner->GetComponent<Model>(), mEngine);

    AnimationAdded animAdd;
    animAdd.animation = anim->mName;
    animAdd.ticksPerSecond = anim->GetTicksPerSecond();
    mOwner->SendEvent(Events::AnimationAdded, &animAdd);

    return anim;
  }

  Animation* Animator::InternalAddAnimation(std::string aName)
  {
    OPTICK_EVENT();
    std::filesystem::path path(aName);
    std::string exten = path.extension().generic_string();

    if (exten != ".YTEAnimation")
    {
      return nullptr;
    }

    Animation *anim = new Animation(aName);
    mAnimations.insert_or_assign(aName, anim);
    return anim;
  }

  void Animator::RemoveAnimation(Animation *aAnimation)
  {
    OPTICK_EVENT();
    for (auto it = mAnimations.begin(); it != mAnimations.end(); ++it)
    {
      if (it->second == aAnimation)
      {
        AnimationRemoved animRemoved;
        animRemoved.animation = it->second->mName;
        mOwner->SendEvent(Events::AnimationRemoved, &animRemoved);

        delete it->second;
        mAnimations.erase(it);
        return;
      }
    }
  }

  RSValue Animator::Serializer(RSAllocator &aAllocator, Object *aOwner)
  {
    OPTICK_EVENT();
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
    OPTICK_EVENT();
    auto owner = static_cast<Animator*>(aOwner);

    for (auto valueIt = aValue.MemberBegin(); valueIt < aValue.MemberEnd(); ++valueIt)
    {
      auto animation = owner->InternalAddAnimation(valueIt->name.GetString());

      DeserializeByType(&(valueIt->value), animation, animation->GetType());
    }
  }

  AnimationData ReadAnimationFromFile(std::string& aFile)
  {
    OPTICK_EVENT();
    auto aniFile = Path::GetAnimationPath(Path::GetGamePath(), aFile);

    if (std::filesystem::exists(aniFile))
    {
      auto [readFile, readFileData] = ReadAnimationDataFromFile(aniFile);

      return std::move(readFile);
    }

    return AnimationData{};
  }
}
