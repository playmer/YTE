///////////////////
// Author: Andrew Griffin
// YTE - Graphics
///////////////////

#pragma once

#ifndef YTE_Graphics_Animation_hpp
#define YTE_Graphics_Animation_hpp

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

    void ReadAnimation(aiNode *aNode, aiMatrix4x4 &aParentTransform);

    aiScene* GetScene();
    aiAnimation* GetAnimation();
    UBOAnimation* GetUBOAnim();
    Skeleton* GetSkeleton();

  private:
    aiScene *mScene;
    aiAnimation *mAnimation;
    UBOAnimation mUBOAnimationData;
    Model *mModel;
    Engine *mEngine;

    aiMatrix4x4 ScaleInterpolation(const aiNodeAnim *aNode);
    aiMatrix4x4 RotationInterpolation(const aiNodeAnim *aNode);
    aiMatrix4x4 TranslationInterpolation(const aiNodeAnim *aNode);
    const aiNodeAnim* FindNodeAnimation(const char *aName);

    // from mesh, has the bone offsets
    Skeleton* mMeshSkeleton;
  };


  class Animator : public Component
  {
  public:
    YTEDeclareType(Animator);

    Animator(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    ~Animator();

    void Initialize() override;

    void Update(LogicUpdate* aEvent);

    void PlayAnimation(std::string aAnimation);

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
    Animation *mNextAnimation;

    std::map<std::string, Animation*> mAnimations;
  };


}

#endif
