#include <QComboBox>
#include <QPushButton>
#include <QGroupBox>
#include <QLayout>

#include "YTEditor/WWiseWidget.hpp"

#include "YTE/Core/Engine.hpp"

namespace YTE
{
  class SetWWiseSwitch : public QComboBox
  {
  public:
    SetWWiseSwitch(QWidget *aOwner, u64 aSwitchGroupId, WWiseSystem *aSystem, WWiseWidget *aWidget)
      : QComboBox(aOwner)
      , mSwitchGroupId(aSwitchGroupId)
      , mSystem(aSystem)
      , mWidget(aWidget)
    {

    }

    void SetCurrentSwitch()
    {
      auto id = this->currentData().toULongLong();

      mSystem->SetSwitch(mSwitchGroupId, id, OwnerId());
    }

    ~SetWWiseSwitch()
    {

    }

    AkGameObjectID OwnerId() { return reinterpret_cast<AkGameObjectID>(mWidget); };

    void indexChanged(int aIndex)
    {
      YTEUnusedArgument(aIndex);
      SetCurrentSwitch();
    }

  private:
    WWiseSystem *mSystem;
    u64 mSwitchGroupId;
    WWiseWidget *mWidget;
  };

  class SendWWiseEvent : public QPushButton
  {
  public:
    SendWWiseEvent(WWiseSystem *aSystem, 
                   const std::string &aEvent, 
                   u64 aEventId, 
                   WWiseWidget *aWidget)
      : QPushButton(aEvent.c_str())
      , mEventId(aEventId)
      , mSystem(aSystem)
      , mWidget(aWidget)
    {

    }

    ~SendWWiseEvent()
    {

    }

    AkGameObjectID OwnerId() { return reinterpret_cast<AkGameObjectID>(mWidget); };

    void clicked()
    {
      mSystem->SendEvent(mEventId, OwnerId());
    }

  private:
    WWiseSystem *mSystem;
    u64 mEventId;
    WWiseWidget *mWidget;
  };

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

      auto bankGroupBox = new QGroupBox(bank.second.mName.c_str(), this);
      QVBoxLayout *bankVbox = new QVBoxLayout(bankGroupBox);

      auto switchGroupGroupBox = new QGroupBox("Switch Groups", bankGroupBox);
      QVBoxLayout *switchGroupVbox = new QVBoxLayout(switchGroupGroupBox);
      for (auto &switchGroup : bank.second.mSwitchGroups)
      {
        auto comboBox = new SetWWiseSwitch(switchGroupGroupBox, 
                                           switchGroup.second.first.mId, 
                                           mSystem, 
                                           this);

        for (auto &aSwitch : switchGroup.second.second)
        {
          comboBox->addItem(aSwitch.mName.c_str(), aSwitch.mId);
        }

        comboBox->SetCurrentSwitch();

        this->connect(static_cast<QComboBox*>(comboBox),
                      static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                      comboBox,
                      &SetWWiseSwitch::indexChanged);

        switchGroupVbox->addWidget(comboBox);
      }
      switchGroupVbox->addStretch(1);

      auto eventGroupBox = new QGroupBox("Events", bankGroupBox);
      QVBoxLayout *eventVbox = new QVBoxLayout(eventGroupBox);
      for (auto &event : bank.second.mEvents)
      {
        SendWWiseEvent *toggleButton = new SendWWiseEvent(mSystem,
                                                          event.mName,
                                                          event.mId,
                                                          this);

        this->connect(toggleButton,
                      &SendWWiseEvent::released,
                      toggleButton,
                      &SendWWiseEvent::clicked);

        eventVbox->addWidget(toggleButton);
      }
      eventVbox->addStretch(1);

      //bankVbox->addWidget(rtpcGroupBox);
      bankVbox->addWidget(switchGroupGroupBox);
      bankVbox->addWidget(eventGroupBox);
      bankVbox->addStretch(1);


      bankGroupBox->setLayout(bankVbox);

      layout->addWidget(bankGroupBox);
    }
  }
}