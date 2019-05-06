#pragma once

#ifndef YTE_Graphics_Animation_hpp
#define YTE_Graphics_Animation_hpp

#include <queue>

#include "YTE/Core/EventHandler.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/Component.hpp"

#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/Generics/Mesh.hpp"
#include "YTE/Graphics/UBOs.hpp"

struct aiScene;
struct aiNodeAnim;
struct aiNode;
struct aiAnimation;

namespace YTE
{
  YTEDeclareEvent(KeyFrameChanged);

  class KeyFrameChanged :public Event
  {
  public:
    YTEDeclareType(KeyFrameChanged);

    std::string animation;
    double time;
  };

  YTEDeclareEvent(AnimationAdded);

  class AnimationAdded :public Event
  {
  public:
    YTEDeclareType(AnimationAdded);

    std::string animation;
    double ticksPerSecond;
  };

  YTEDeclareEvent(AnimationRemoved);

  class AnimationRemoved :public Event
  {
  public:
    YTEDeclareType(AnimationRemoved);

    std::string animation;
  };

  struct AnimationData
  {
    struct TranslationKey
    {
      TranslationKey() {}

      TranslationKey(double aTime, glm::vec3 aTranslation)
        : mTime{ aTime }
        , mTranslation{ aTranslation }
      {

      }

      bool operator==(TranslationKey const& aRight) const
      {
        return mTime == aRight.mTime &&
               mTranslation == aRight.mTranslation;
      }

      double mTime;
      glm::vec3 mTranslation;
    };

    struct ScaleKey
    {
      ScaleKey() {}

      ScaleKey(double aTime, glm::vec3 aScale)
        : mTime{ aTime }
        , mScale{ aScale }
      {

      }

      bool operator==(ScaleKey const& aRight) const
      {
        return mTime == aRight.mTime &&
               mScale == aRight.mScale;
      }

      double mTime;
      glm::vec3 mScale;
    };

    struct RotationKey
    {
      RotationKey() {}

      RotationKey(double aTime, glm::quat aRotation)
        : mTime{ aTime }
        , mRotation{ aRotation }
      {

      }

      bool operator==(RotationKey const& aRight) const
      {
        return mTime == aRight.mTime &&
                mRotation == aRight.mRotation;
      }

      double mTime;
      glm::quat mRotation;
    };

    struct Node
    {
      bool operator==(Node const& aRight) const
      {
        return
          mTransformationOffset == aRight.mTransformationOffset &&
          mTranslationKeyOffset == aRight.mTranslationKeyOffset &&
          mTranslationKeySize == aRight.mTranslationKeySize &&
          mScaleKeyOffset == aRight.mScaleKeyOffset &&
          mScaleKeySize == aRight.mScaleKeySize &&
          mRotationKeyOffset == aRight.mRotationKeyOffset &&
          mRotationKeySize == aRight.mRotationKeySize &&
          mNameOffset == aRight.mNameOffset &&
          mNameSize == aRight.mNameSize &&
          mChildrenOffset == aRight.mChildrenOffset &&
          mChildrenSize == aRight.mChildrenSize;
      }

      // Ids of nodes in parent AnimationData struct.
      size_t mTransformationOffset = 0;
      size_t mTranslationKeyOffset = 0;
      size_t mTranslationKeySize   = 0;
      size_t mScaleKeyOffset       = 0;
      size_t mScaleKeySize         = 0;
      size_t mRotationKeyOffset    = 0;
      size_t mRotationKeySize      = 0;
      size_t mNameOffset           = 0;
      size_t mNameSize             = 0;
      size_t mChildrenOffset       = 0;
      size_t mChildrenSize         = 0;
    };

    std::vector<Node> mNodes;
    std::vector<glm::mat4> mTransformations;
    std::vector<size_t> mChildren;
    std::vector<TranslationKey> mTranslationKeys;
    std::vector<ScaleKey> mScaleKeys;
    std::vector<RotationKey> mRotationKeys;
    std::vector<char> mNames;

    // Duration of the animation in ticks.
    double mDuration;

    // Ticks per second. 0 if not specified in the imported file.
    double mTicksPerSecond;
  };

  class Animation : public EventHandler
  {
  public:
    YTEDeclareType(Animation);

    YTE_Shared Animation(std::string &aFile, uint32_t aAnimationIndex = 0);
    YTE_Shared void Initialize(Model *aModel, Engine *aEngine);
    YTE_Shared virtual ~Animation();

    YTE_Shared void SetCurrentTime(double aCurrentTime);
    YTE_Shared double GetMaxTime() const;

    YTE_Shared float GetSpeed() const;
    YTE_Shared void SetSpeed(float aSpeed);

    YTE_Shared bool GetPlayOverTime() const;
    YTE_Shared void SetPlayOverTime(bool aPlayOverTime);

    YTE_Shared void ReadAnimation(AnimationData::Node const& aNode, glm::mat4 const& aParentTransform);
    YTE_Shared void Animate();

    YTE_Shared UBOs::Animation* GetUBOAnim();
    YTE_Shared Skeleton* GetSkeleton();

    std::string mName;
    double mCurrentAnimationTime;
    uint32_t mAnimationIndex;
    double mSpeed;

    bool mPlayOverTime;
    double mElapsedTime;

    double GetTicksPerSecond()
    {
      return mData.mTicksPerSecond;
    }

    double GetDuration()
    {
      return mData.mDuration;
    }

  private:
    UBOs::Animation mUBOAnimationData;
    Model *mModel;
    Engine *mEngine;

    // from mesh, has the bone offsets
    Skeleton* mMeshSkeleton;
    AnimationData mData;
  };


  class Animator : public Component
  {
  public:
    YTEDeclareType(Animator);

    YTE_Shared Animator(Composition *aOwner, Space *aSpace);

    YTE_Shared ~Animator();

    YTE_Shared void Initialize() override;

    YTE_Shared void Update(LogicUpdate* aEvent);

    YTE_Shared void PlayAnimationSet(std::string aAnimation);

    YTE_Shared void AddNextAnimation(std::string aAnimation);

    YTE_Shared void SetDefaultAnimation(std::string aAnimation);
    YTE_Shared void SetCurrentAnimation(std::string aAnimation);
    YTE_Shared void SetCurrentPlayOverTime(bool aPlayOverTime);

    YTE_Shared void SetCurrentAnimTime(double aTime);
    YTE_Shared double GetMaxTime() const;

    YTE_Shared std::map<std::string, Animation*>& GetAnimations();

    YTE_Shared static std::vector<std::pair<YTE::Object*, std::string>> Lister(YTE::Object *aSelf);
    YTE_Shared static RSValue Serializer(RSAllocator &aAllocator, Object *aOwner);
    YTE_Shared static void Deserializer(RSValue &aValue, Object *aOwner);

    YTE_Shared Animation* AddAnimation(std::string aName);
    YTE_Shared Animation* InternalAddAnimation(std::string aName);
    YTE_Shared void RemoveAnimation(Animation *aAnimation);

  private:
    Model * mModel;
    Engine *mEngine;

    Animation *mDefaultAnimation;
    Animation *mCurrentAnimation;
    
    std::queue<Animation*> mNextAnimations;

    std::map<std::string, Animation*> mAnimations;
  };
}

#endif
