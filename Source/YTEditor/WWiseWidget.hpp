#pragma once

#include <qscrollarea.h>
#include <qlayout.h>
#include <QPushButton>
#include <QScrollArea>

#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/WWise/WWiseSystem.hpp"

namespace YTE
{
  class WWiseWidget : public QScrollArea
  {
  public:
    WWiseWidget(QWidget *aParent, Engine *aEngine);
    ~WWiseWidget();

    AkGameObjectID OwnerId() { return reinterpret_cast<AkGameObjectID>(this); };

    void LoadEvents();

  private:
    Engine *mEngine;
    WWiseSystem *mSystem;
  };
}
