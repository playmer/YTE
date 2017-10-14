#pragma once

#include "../../YTE/Core/Composition.hpp"

#include "../../YTE/Graphics/Model.hpp"


class Axis : public YTE::Composition
{
public:

  Axis(int aDir, YTE::Engine *aEngine, YTE::Space *aSpace);

  void AddComponents();

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