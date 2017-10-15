#pragma once

#include <qscrollarea.h>
#include <qlayout.h>
#include <QPushButton>
#include <QWidget>

#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/WWise/WWiseSystem.hpp"

namespace YTE
{
  class WWiseWidget : public QWidget
  {
  public:
    WWiseWidget(QWidget *aParent, Engine *aEngine);
    ~WWiseWidget();

    AkGameObjectID OwnerId() { return reinterpret_cast<AkGameObjectID>(this); };

    void LoadEvents();

  private:
    Engine *mEngine;
    WWiseSystem *mSystem;

    QVBoxLayout *mLayout;
    QWidget *mSubWidget;
    QVBoxLayout *mSubWidgetLayout;
    QScrollArea *mScrollArea;

    void ConstructSubWidgets();
    

  };
}