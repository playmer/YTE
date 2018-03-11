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
    FaceAnim(std::string animationFilename);

  private:
    std::vector<FaceFrame> eyeFrames;
    std::vector<FaceFrame> mouthFrames;
  };

  class FacialAnimator : public Component
  {
  public:
    YTEDeclareType(FacialAnimator);

    FacialAnimator(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    ~FacialAnimator();

    void Initialize() override;

    void OnKeyFrameChange(KeyFrameChanged *event);
    void OnAnimationAdded(AnimationAdded *event);
    void OnAnimationRemoved(AnimationRemoved *event);

  private:

    Animator *mAnimator;

    std::map<std::string, FaceAnim> mFaceAnimations;

  };
}

