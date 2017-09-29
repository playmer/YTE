#pragma once

#include <qscrollarea.h>
#include <qlayout.h>

#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/WWise/WWiseSystem.hpp"

namespace YTE
{
  class WWiseWidget : public QWidget
  {
  public:
    WWiseWidget(QWidget *aParent, Engine *aEngine);

    void LoadEvents();

  private:
    Engine *mEngine;
    WWiseSystem *mSystem;
  };
}
