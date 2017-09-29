#pragma once

#include <qscrollarea.h>
#include <qlayout.h>
#include <QPushButton>

#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/WWise/WWiseSystem.hpp"

namespace YTE
{
  class SendWWiseEvent : public QPushButton
  {
  public:
    SendWWiseEvent(WWiseSystem *aSystem, std::string &aEvent);
    ~SendWWiseEvent();
    AkGameObjectID OwnerId() { return reinterpret_cast<AkGameObjectID>(this); };

    void clicked();
  private:
    WWiseSystem *mSystem;
    std::string mEvent;
  };

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
