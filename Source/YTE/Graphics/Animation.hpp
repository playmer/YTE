///////////////////
// Author: Andrew Griffin
// YTE - Graphics
///////////////////

#pragma once

#ifndef YTE_Graphics_Animation_hpp
#define YTE_Graphics_Animation_hpp

#include <queue>

#include "assimp/types.h"
#include "assimp/vector3.h"

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

  class Animation : public EventHandler
  {
  public:
    YTEDeclareType(Animation);

    Animation(std::string &aFile, uint32_t aAnimationIndex = 0);
    void Initialize(Model *aModel, Engine *aEngine);
    virtual ~Animation();

    void SetCurrentTime(double aCurrentTime);
    double GetMaxTime() const;

    float GetSpeed() const;
    void SetSpeed(float aSpeed);

    bool GetPlayOverTime() const;
    void SetPlayOverTime(bool aPlayOverTime);

    std::string mName;
    double mCurrentAnimationTime;
    uint32_t mAnimationIndex;
    double mSpeed;

    bool mPlayOverTime;
    double mElapsedTime;

    void ReadAnimation(aiNode *aNode, glm::mat4 const& aParentTransform);
    void Animate();

    UBOAnimation* GetUBOAnim();
    Skeleton* GetSkeleton();

    double GetTicksPerSecond()
    {
      return mTicksPerSecond;
    }

    double GetDuration()
    {
      return mDuration;
    }

  private:
    aiScene *mScene;
    aiAnimation *mAnimation;
    UBOAnimation mUBOAnimationData;
    Model *mModel;
    Engine *mEngine;

    // from mesh, has the bone offsets
    Skeleton* mMeshSkeleton;

    // Duration of the animation in ticks.
    double mDuration;

    // Ticks per second. 0 if not specified in the imported file.
    double mTicksPerSecond;
  };


  class Animator : public Component
  {
  public:
    YTEDeclareType(Animator);

    Animator(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    ~Animator();

    void Initialize() override;

    void Update(LogicUpdate* aEvent);

    void PlayAnimationSet(std::string aAnimation);

    void AddNextAnimation(std::string aAnimation);

    void SetDefaultAnimation(std::string aAnimation);
    void SetCurrentAnimation(std::string aAnimation);
    void SetCurrentPlayOverTime(bool aPlayOverTime);

    void SetCurrentAnimTime(double aTime);
    double GetMaxTime() const;

    std::map<std::string, Animation*>& GetAnimations();

    static std::vector<std::pair<YTE::Object*, std::string>> Lister(YTE::Object *aSelf);
    static RSValue Serializer(RSAllocator &aAllocator, Object *aOwner);
    static void Deserializer(RSValue &aValue, Object *aOwner);

    Animation* AddAnimation(std::string aName);
    Animation* InternalAddAnimation(std::string aName);
    void RemoveAnimation(Animation *aAnimation);

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
