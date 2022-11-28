#pragma once

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

    FacialAnimator(Composition *aOwner, Space *aSpace);
    FacialAnimator(FacialAnimator&) = delete;

    ~FacialAnimator();

    void Initialize() override;

    void OnModelChanged(ModelChanged *event);
    void OnKeyFrameChanged(KeyFrameChanged *event);
    void OnAnimationAdded(AnimationAdded *event);
    void OnAnimationRemoved(AnimationRemoved *event);

  private:

    Model *mModel;
    Animator *mAnimator;

    std::map<std::string, std::unique_ptr<FaceAnim>> mFaceAnimations;

    void RefreshInitialBufffers();

    int mEyeBufferIndex;
    VertexData mInitialEyeVertexBuffer;

    int mMouthBufferIndex;
    VertexData mInitialMouthVertexBuffer;

    FaceFrame* FindFrame(std::vector<FaceFrame>& aFrames, double time);
  };
}

