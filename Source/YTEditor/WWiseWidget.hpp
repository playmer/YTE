#pragma once

#include <qscrollarea.h>
#include <qlayout.h>
#include <QPushButton>

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
  };

  class SendWWiseEvent : public QPushButton
  {
  public:
    SendWWiseEvent(WWiseSystem *aSystem, std::string &aEvent, WWiseWidget *aWidget);
    ~SendWWiseEvent();
    AkGameObjectID OwnerId() { return reinterpret_cast<AkGameObjectID>(mWidget); };

    void clicked();
  private:
    WWiseSystem *mSystem;
    std::string mEvent;
    WWiseWidget *mWidget;
  };
}
