#pragma once

#include <qscrollarea.h>
#include <qlayout.h>
#include <qpushButton>

#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/WWise/WWiseSystem.hpp"

#include "YTEditor/MainWindow/MainWindow.hpp"
#include "YTEditor/MainWindow/Widgets/Widget.hpp"

namespace YTEditor
{
  class WWiseWidget : public Widget
  {
  public:
    WWiseWidget(MainWindow *aParent, YTE::Engine *aEngine);
    ~WWiseWidget();

    AkGameObjectID OwnerId() { return reinterpret_cast<AkGameObjectID>(this); };

    void LoadEvents();

    static std::string GetName();

    Widget::DockArea GetDefaultDockPosition() const override;

  private:

    YTE::Engine *mEngine;
    YTE::WWiseSystem *mSystem;

    QVBoxLayout *mLayout;
    QWidget *mSubWidget;
    QVBoxLayout *mSubWidgetLayout;
    QScrollArea *mScrollArea;

    void ConstructSubWidgets();
    

  };
}
