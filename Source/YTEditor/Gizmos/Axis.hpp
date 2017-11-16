#pragma once

#include "YTE/Core/Composition.hpp"
#include "YTE/Graphics/Model.hpp"

namespace YTEditor
{

  class Axis : public YTE::Composition
  {
  public:

    Axis(int aDir, YTE::Engine *aEngine, YTE::Space *aSpace);

  private:

    YTE::Model *mTranslateModel;
    YTE::Model *mScaleModel;
    YTE::Model *mRotateModel;

    int mDir;

  public:
    enum Dir
    {
      X,
      Y,
      Z
    };


  };

}
