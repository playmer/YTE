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
  
    Animation(std::string &aFile, Model *aModel, Engine *aEngine, uint32_t aAnimationIndex = 0);
    virtual ~Animation();
    void Update(LogicUpdate* aEvent);
  
    float GetSpeed() const
    {
      return mSpeed;
    }

    void SetSpeed(float aSpeed)
    {
      mSpeed = aSpeed;
    }

    std::string mName;
    float mCurrentAnimationTime;
    uint32_t mAnimationIndex;
    float mSpeed;
    
  private:
    aiScene *mScene;
    aiAnimation *mAnimation;
    UBOAnimation mUBOAnimationData;
    Model *mModel;
    Engine *mEngine;
    float mElapsedTime;

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

    Animation* AddAnimation(std::string aName);
    void RemoveAnimation(Animation *aAnimation);

  private:
    Model *mModel;
    Engine *mEngine;
    std::vector<std::pair<Animation*, std::string>> mAnimations;
  };


}

#endif
