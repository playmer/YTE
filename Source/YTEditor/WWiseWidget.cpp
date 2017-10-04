#include <QPushButton>
#include <QGroupBox>
#include <QLayout>

#include "YTEditor/WWiseWidget.hpp"

#include "YTE/Core/Engine.hpp"

namespace YTE
{
  SendWWiseEvent::SendWWiseEvent(WWiseSystem *aSystem, 
                                 std::string &aEvent,
                                 WWiseWidget *aWidget)
    : QPushButton(aEvent.c_str())
    , mEvent(aEvent)
    , mSystem(aSystem)
    , mWidget(aWidget)
  {
  }

  void SendWWiseEvent::clicked()
  {
    mSystem->SendEvent(mEvent, OwnerId());
  }

  SendWWiseEvent::~SendWWiseEvent()
  {
  }

  WWiseWidget::WWiseWidget(QWidget *aParent, Engine *aEngine)
    : QWidget(aParent)
    , mEngine(aEngine)
  {
    std::string name{ "WWiseWidget" };
    mSystem = mEngine->GetComponent<WWiseSystem>();
    mSystem->RegisterObject(OwnerId(), name);

    LoadEvents();
  }

  WWiseWidget::~WWiseWidget()
  {
    mSystem->DeregisterObject(reinterpret_cast<AkGameObjectID>(this));
  }

  void WWiseWidget::LoadEvents()
  {
    auto &banks = mSystem->GetBanks();

    auto layout = new QVBoxLayout(this);

    this->setLayout(layout);

    for (auto &bank : banks)
    {
      if (0 == bank.second.mEvents.size())
      {
        continue;
      }

      auto groupBox = new QGroupBox(bank.second.mName.c_str(), this);

      QVBoxLayout *vbox = new QVBoxLayout(groupBox);

      for (auto &event : bank.second.mEvents)
      {
        SendWWiseEvent *toggleButton = new SendWWiseEvent(mSystem,
                                                          event.mName,
                                                          this);

        this->connect(toggleButton,
                      &SendWWiseEvent::released,
                      toggleButton,
                      &SendWWiseEvent::clicked);

        vbox->addWidget(toggleButton);
      }

      vbox->addStretch(1);


      groupBox->setLayout(vbox);

      layout->addWidget(groupBox);
    }
  }
}