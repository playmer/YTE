#pragma once

#include <qscrollarea.h>
#include <qlayout.h>
#include <qpushButton>

#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/WWise/WWiseSystem.hpp"

#include "YTEditor/Framework/MainWindow.hpp"
#include "YTEditor/Framework/Widget.hpp"

#include "YTEditor/YTELevelEditor/YTELevelEditor.hpp"

namespace YTEditor
{
  class WWiseWidget : public Framework::Widget
  {
  public:
    WWiseWidget(YTELevelEditor* aWorkspace, YTE::Engine* aEngine);
    ~WWiseWidget();

    AkGameObjectID OwnerId() { return reinterpret_cast<AkGameObjectID>(this); };

    void LoadEvents();

    static std::string GetName();

    Framework::Widget::DockArea GetDefaultDockArea() const override;

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
