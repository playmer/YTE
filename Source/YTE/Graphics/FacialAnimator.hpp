///////////////////
// Author: Andrew Griffin
// YTE - Graphics
///////////////////

#pragma once

#include "assimp/types.h"
#include "assimp/vector3.h"

#include "YTE/Core/EventHandler.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/Component.hpp"

#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/Generics/Mesh.hpp"
#include "YTE/Graphics/UBOs.hpp"

namespace YTE
{
  struct FaceFrame
  {
    int id;
    double time;
    glm::vec2 uv;
  };

  class FaceAnim
  {
  public:
    FaceAnim(std::string animationFilename, double ticksPerSecond);

    std::vector<FaceFrame> eyeFrames;
    std::vector<FaceFrame> mouthFrames;

    double ticksPerSecond;
  };

  class FacialAnimator : public Component
  {
  public:
    YTEDeclareType(FacialAnimator);

    FacialAnimator(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    ~FacialAnimator();

    void Initialize() override;

    void OnModelChanged(ModelChanged *event);
    void OnKeyFrameChanged(KeyFrameChanged *event);
    void OnAnimationAdded(AnimationAdded *event);
    void OnAnimationRemoved(AnimationRemoved *event);

  private:

    Model *mModel;
    Animator *mAnimator;

    std::map<std::string, FaceAnim*> mFaceAnimations;

    void RefreshInitialBufffers();

    int mEyeBufferIndex;
    std::vector<Vertex> mInitialEyeVertexBuffer;

    int mMouthBufferIndex;
    std::vector<Vertex> mInitialMouthVertexBuffer;

    FaceFrame* FindEyeFrame(FaceAnim *anim, double time);
    FaceFrame* FindMouthFrame(FaceAnim *anim, double time);
  };
}

