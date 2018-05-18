///////////////////
// Author: Andrew Griffin
// YTE - Graphics
///////////////////

#include <algorithm>
#include <fstream>

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/types.h"
#include "assimp/vector3.h"

#include "glm/gtc/type_ptr.hpp"

#include "YTE/Core/AssetLoader.hpp"
#include "YTE/Core/Engine.hpp"

#include "YTE/Graphics/Animation.hpp"
#include "YTE/Graphics/Generics/InstantiatedModel.hpp"
#include "YTE/Graphics/Model.hpp"

#include "YTE/Utilities/Utilities.hpp"

namespace YTE
{
  static inline
  glm::vec3 ToGlm(aiVector3D const* aVector)
  {
    return { aVector->x, aVector->y ,aVector->z };
  }

  static inline
  glm::vec3 ToGlm(aiColor3D const* aVector)
  {
    return { aVector->r, aVector->g ,aVector->b };
  }

  static inline
  glm::quat ToGlm(aiQuaternion const* aQuat)
  {
    glm::quat quaternion;

    quaternion.x = aQuat->x;
    quaternion.y = aQuat->y;
    quaternion.z = aQuat->z;
    quaternion.w = aQuat->w;

    return quaternion;
  }

  static inline
  glm::mat4 ToGlm(const aiMatrix4x4 &aMatrix)
  {
    return glm::transpose(glm::make_mat4(&aMatrix.a1));
  }

  static inline
  aiMatrix4x4 ToAssimp(const glm::mat4 &aMatrix)
  {
    auto transposed = glm::transpose(aMatrix);
    return *(reinterpret_cast<aiMatrix4x4*>(glm::value_ptr(transposed)));
  }


  //static inline 
  //glm::mat4 ScaleInterpolation(double aAnimationTime, const aiNodeAnim* aNode)
  //{
  //  aiVector3D scale;
  //  if (aNode->mNumScalingKeys == 1)
  //  {
  //    scale = aNode->mScalingKeys[0].mValue;
  //  }
  //  else
  //  {
  //    auto startFrame = aNode->mScalingKeys[0];
  //
  //    uint32_t index = 0;
  //    for (uint32_t i = 0; i < aNode->mNumScalingKeys - 1; ++i)
  //    {
  //      if (aAnimationTime < (float)aNode->mScalingKeys[i + 1].mTime - startFrame.mTime)
  //      {
  //        index = i;
  //        break;
  //      }
  //    }
  //
  //    aiVectorKey frame = aNode->mScalingKeys[index];
  //    aiVectorKey nextFrame = aNode->mScalingKeys[(index + 1) % aNode->mNumScalingKeys];
  //
  //    float delta = static_cast<float>((aAnimationTime + startFrame.mTime - frame.mTime) / (nextFrame.mTime - frame.mTime));
  //
  //    const aiVector3D& start = frame.mValue;
  //    const aiVector3D& end = nextFrame.mValue;
  //
  //    scale = (start + delta * (end - start));
  //  }
  //
  //  aiMatrix4x4 mat;
  //  aiMatrix4x4::Scaling(scale, mat);
  //  return ToGlm(mat);
  //}
  //
  //static inline
  //glm::mat4 RotationInterpolation(double aAnimationTime, const aiNodeAnim* aNode)
  //{
  //  aiQuaternion rot;
  //
  //  if (aNode->mNumRotationKeys == 1)
  //  {
  //    rot = aNode->mRotationKeys[0].mValue;
  //  }
  //  else
  //  {
  //    auto startFrame = aNode->mRotationKeys[0];
  //
  //    // TODO (Andrew): Can we resuse the keys between scale translate and rotation? is the index the same?
  //    uint32_t index = 0;
  //    for (uint32_t i = 0; i < aNode->mNumRotationKeys - 1; ++i)
  //    {
  //      if (aAnimationTime < (float)aNode->mRotationKeys[i + 1].mTime - startFrame.mTime)
  //      {
  //        index = i;
  //        break;
  //      }
  //    }
  //
  //    aiQuatKey frame = aNode->mRotationKeys[index];
  //    aiQuatKey nextFrame = aNode->mRotationKeys[(index + 1) % aNode->mNumRotationKeys];
  //
  //    float delta = static_cast<float>((aAnimationTime + startFrame.mTime - frame.mTime) / (nextFrame.mTime - frame.mTime));
  //
  //    const aiQuaternion& start = frame.mValue;
  //    const aiQuaternion& end = nextFrame.mValue;
  //
  //    aiQuaternion::Interpolate(rot, start, end, delta);
  //    rot.Normalize();
  //  }
  //
  //  return ToGlm(aiMatrix4x4(rot.GetMatrix()));
  //}
  //
  //static inline
  //glm::mat4 TranslationInterpolation(double aAnimationTime, const aiNodeAnim *aNode)
  //{
  //  aiVector3D trans;
  //  if (aNode->mNumPositionKeys == 1)
  //  {
  //    trans = aNode->mPositionKeys[0].mValue;
  //  }
  //  else
  //  {
  //    auto startFrame = aNode->mPositionKeys[0];
  //
  //    uint32_t index = 0;
  //    for (uint32_t i = 0; i < aNode->mNumPositionKeys - 1; ++i)
  //    {
  //      if (aAnimationTime < (float)aNode->mPositionKeys[i + 1].mTime - startFrame.mTime)
  //      {
  //        index = i;
  //        break;
  //      }
  //    }
  //
  //    aiVectorKey frame = aNode->mPositionKeys[index];
  //    aiVectorKey nextFrame = aNode->mPositionKeys[(index + 1) % aNode->mNumPositionKeys];
  //
  //    float delta = static_cast<float>((aAnimationTime + startFrame.mTime - frame.mTime) / (nextFrame.mTime - frame.mTime));
  //
  //    const aiVector3D& start = frame.mValue;
  //    const aiVector3D& end = nextFrame.mValue;
  //
  //    trans = (start + delta * (end - start));
  //  }
  //
  //  aiMatrix4x4 mat;
  //  aiMatrix4x4::Translation(trans, mat);
  //  return ToGlm(mat);
  //}

  // The following is a glmification of the assimp aiQuaterniont<TReal>::Interpolate
  // function.
  // TODO (Joshua): Investigate glm::mix function after we've updated glm.
  // We've preserved the following comments from assimps writing of the function:
  // ---------------------------------------------------------------------------
  // Performs a spherical interpolation between two quaternions
  // Implementation adopted from the gmtl project. All others I found on the net fail in some cases.
  // Congrats, gmtl!
  inline glm::quat interpolate(glm::quat const& aStart, glm::quat const& aEnd, float aFactor)
  {
    // calc cosine theta
    float cosom = glm::dot(aStart, aEnd);

    // adjust signs (if necessary)
    glm::quat end = aEnd;
    if (cosom < 0.0f)
    {
      cosom = -cosom;
      end.x = -end.x;   // Reverse all signs
      end.y = -end.y;
      end.z = -end.z;
      end.w = -end.w;
    }

    // Calculate coefficients
    float sclp;
    float sclq;
    if ((1.0f - cosom) > 0.0001f) // 0.0001 -> some epsillon
    {
      // Standard case (slerp)
      float omega;
      float sinom;
      omega = std::acos(cosom); // extract theta from dot product's cos theta
      sinom = std::sin(omega);
      sclp = std::sin((1.0f - aFactor) * omega) / sinom;
      sclq = std::sin(aFactor * omega) / sinom;
    }
    else
    {
      // Very close, do linear interp (because it's faster)
      sclp = 1.0f - aFactor;
      sclq = aFactor;
    }

    return { sclp * aStart.w + sclq * end.w,
             sclp * aStart.x + sclq * end.x,
             sclp * aStart.y + sclq * end.y,
             sclp * aStart.z + sclq * end.z };
  }







  

  static inline 
  glm::vec3 ScaleInterpolation(AnimationData const& aData, 
                               double aAnimationTime, 
                               AnimationData::Node const& aNode)
  {
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
      
      rot = interpolate(start, end, delta);
      glm::normalize(rot);
    }

    return rot;
  }

  static inline
    glm::vec3 TranslationInterpolation(AnimationData const& aData,
                                       double aAnimationTime,
                                       AnimationData::Node const& aNode)
  {
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











  const aiNodeAnim* FindNodeAnimation(aiAnimation *aAnimation, const char *aName)
  {
    for (uint32_t i = 0; i < aAnimation->mNumChannels; ++i)
    {
      const aiNodeAnim *anim = aAnimation->mChannels[i];

      if (!strcmp(anim->mNodeName.data, aName))
      {
        return anim;
      }
    }
    return nullptr;
  }





  YTEDefineEvent(KeyFrameChanged);

  YTEDefineType(KeyFrameChanged)
  {
    RegisterType<KeyFrameChanged>();
    TypeBuilder<KeyFrameChanged> builder;
    builder.Field<&KeyFrameChanged::animation>( "animation", PropertyBinding::Get);
    builder.Field<&KeyFrameChanged::time>( "time", PropertyBinding::Get);
  }

  YTEDefineEvent(AnimationAdded);

  YTEDefineType(AnimationAdded)
  {
    RegisterType<AnimationAdded>();
    TypeBuilder<AnimationAdded> builder;
    builder.Field<&AnimationAdded::animation>( "animation", PropertyBinding::Get);
  }

  YTEDefineEvent(AnimationRemoved);

  YTEDefineType(AnimationRemoved)
  {
    RegisterType<AnimationRemoved>();
    TypeBuilder<AnimationRemoved> builder;
    builder.Field<&AnimationRemoved::animation>( "animation", PropertyBinding::Get);
  }


  YTEDefineType(Animation)
  {
    RegisterType<Animation>();
    TypeBuilder<Animation> builder;

    std::vector<std::vector<Type*>> deps = { { TypeId<Animator>() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);

    builder.Property<&Animation::GetSpeed, &Animation::SetSpeed>( "Speed")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .SetDocumentation("The speed at which the animation will be played at.");

    builder.Property<&Animation::GetPlayOverTime, &Animation::SetPlayOverTime>( "PlayOverTime")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .SetDocumentation("True if the animation should play with respect to time.");
  }

  template <typename tContainer, typename tKey>
  void InsertKeys(tContainer& aContainer, tKey const* aBegin, tKey const* aEnd)
  {
    for (auto it = aBegin; it < aEnd; ++it)
    {
      aContainer.emplace_back(it->mTime, ToGlm(&it->mValue));
    }
  }
  
  static inline
  void MakeNodes(aiAnimation *aAssimpAnimation,
                 aiNode *aAssimpNode,
                 AnimationData& aData,
                 size_t aNodeIndex)
  {
    AnimationData::Node& node = aData.mNodes[aNodeIndex];

    node.mTransformationOffset = aData.mTransformations.size();
    aData.mTransformations.emplace_back(ToGlm(aAssimpNode->mTransformation));

    size_t nameLength = aAssimpNode->mName.length;

    node.mNameOffset = aData.mNames.size();
    node.mNameSize = nameLength;
    for (size_t i = 0; i < aAssimpNode->mName.length; ++i)
    {
      aData.mNames.emplace_back(*(aAssimpNode->mName.C_Str() + i));
    }
    
    // Null terminate the string.
    aData.mNames.emplace_back('\0');

    aiNodeAnim const* animKeys = FindNodeAnimation(aAssimpAnimation, aAssimpNode->mName.data);

    if (animKeys)
    {
      node.mTranslationKeyOffset = aData.mTranslationKeys.size();
      node.mTranslationKeySize = animKeys->mNumPositionKeys;
      InsertKeys(aData.mTranslationKeys, animKeys->mPositionKeys, animKeys->mPositionKeys + animKeys->mNumPositionKeys);

      node.mScaleKeyOffset = aData.mScaleKeys.size();
      node.mScaleKeySize = animKeys->mNumScalingKeys;
      InsertKeys(aData.mScaleKeys, animKeys->mScalingKeys, animKeys->mScalingKeys + animKeys->mNumScalingKeys);

      node.mRotationKeyOffset = aData.mRotationKeys.size();
      node.mRotationKeySize = animKeys->mNumRotationKeys;
      InsertKeys(aData.mRotationKeys, animKeys->mRotationKeys, animKeys->mRotationKeys + animKeys->mNumRotationKeys);
    }

    node.mChildrenOffset = aData.mChildren.size();
    node.mChildrenSize = aAssimpNode->mNumChildren;

    // Add the children nodes to the AnimationData.
    // NOTE: node can now be potentially invalidated, so we no longer use it.
    for (uint32_t i = 0; i < aAssimpNode->mNumChildren; ++i)
    {
      aData.mChildren.emplace_back(aData.mNodes.size());
      aData.mNodes.emplace_back(AnimationData::Node{});
    }

    // Process the children added
    for (auto i = 0; i < aAssimpNode->mNumChildren; ++i)
    {
      MakeNodes(aAssimpAnimation, 
                aAssimpNode->mChildren[i], 
                aData, 
                aData.mChildren[i + aData.mNodes[aNodeIndex].mChildrenOffset]);
    }
  }

  AnimationData GetAnimationData(std::string &aFile)
  {
    Assimp::Importer importer;
    auto aniFile = Path::GetAnimationPath(Path::GetGamePath(), aFile);
    auto scene = importer.ReadFile(aniFile.c_str(),
                                   aiProcess_Triangulate |
                                   aiProcess_CalcTangentSpace |
                                   aiProcess_GenSmoothNormals);
    
    DebugObjection(scene == nullptr,
                   "Failed to load animation file %s from assimp",
                   aniFile.c_str());

    DebugObjection(scene->HasAnimations() == false,
                   "Failed to find animations in scene loaded from %s",
                   aniFile.c_str());
                   
    UnusedArguments(scene);

    auto animation = scene->mAnimations[0];

    AnimationData data;
    AnimationData::Node node;

    data.mNodes.emplace_back(node);
    
    MakeNodes(animation, scene->mRootNode, data, 0);

    data.mTicksPerSecond = animation->mTicksPerSecond;
    data.mDuration = animation->mDuration;

    return std::move(data);
  }

  Animation::Animation(std::string &aFile, uint32_t aAnimationIndex)
    : mPlayOverTime{ true }
    , mData{ GetAnimationData(aFile) }
  {
    mAnimationIndex = aAnimationIndex;

    mName = aFile;
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
    glm::mat4 identity;
    ReadAnimation(mData.mNodes.front(), identity);
  }

  void Animation::ReadAnimation(AnimationData::Node const& aNode, 
                                glm::mat4 const& aParentTransform)
  {
    glm::mat4 nodeTransformation = mData.mTransformations[aNode.mTransformationOffset];

    if (aNode.mTranslationKeySize &&
        aNode.mScaleKeySize && 
        aNode.mRotationKeySize)
    {
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

  UBOAnimation* Animation::GetUBOAnim()
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

  Animator::Animator(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mDefaultAnimation(nullptr)
    , mCurrentAnimation(nullptr)
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

    mEngine->RegisterEvent<&Animator::Update>(Events::AnimationUpdate, this);
  }

  void Animator::Update(LogicUpdate *aEvent)
  {
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
    std::string initFile = aAnimation + "_Init.fbx";
    std::string loopFile = aAnimation + "_Loop.fbx";
    std::string exitFile = aAnimation + "_Exit.fbx";

    AddNextAnimation(initFile);
    AddNextAnimation(loopFile);
    AddNextAnimation(exitFile);
  }

  void Animator::AddNextAnimation(std::string aAnimation)
  {
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
