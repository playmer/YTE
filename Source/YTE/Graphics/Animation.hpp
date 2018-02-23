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
  class Animation : public EventHandler
  {
  public:
    YTEDeclareType(Animation);
  
    Animation(std::string &aFile, uint32_t aAnimationIndex = 0);
    void Initialize(Model *aModel, Engine *aEngine);
    virtual ~Animation();
    void Update(LogicUpdate* aEvent);
  
    double GetSpeed() const

    double GetMaxTime() const;

    {
      return mSpeed;
    }

    void SetSpeed(double aSpeed)
    {
      mSpeed = aSpeed;
    }

    bool GetPlayOverTime() const;

    void SetPlayOverTime(bool aPlayOverTime);

    std::string mName;
    double mCurrentAnimationTime;
    uint32_t mAnimationIndex;
    double mSpeed;
    
  private:
    aiScene *mScene;
    aiAnimation *mAnimation;
    UBOAnimation mUBOAnimationData;
    Model *mModel;
    Engine *mEngine;
    double mElapsedTime;

    bool mPlayOverTime;

    void ReadAnimation(aiNode *aNode, aiMatrix4x4 &aParentTransform);
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

    static std::vector<std::pair<YTE::Object*, std::string>> Lister(YTE::Object *aSelf);
    static RSValue Serializer(RSAllocator &aAllocator, Object *aOwner);
    static void Deserializer(RSValue &aValue, Object *aOwner);

    Animation* AddAnimation(std::string aName);
    void RemoveAnimation(Animation *aAnimation);

  private:
    Model *mModel;
    Engine *mEngine;
    std::vector<std::pair<Animation*, std::string>> mAnimations;
  };


}

#endif
