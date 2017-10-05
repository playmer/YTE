#include <QComboBox>
#include <QPushButton>
#include <QGroupBox>
#include <QLayout>
#include <QHBoxLayout>
#include <QLabel>

#include "YTEditor/WWiseWidget.hpp"

#include "YTE/Core/Engine.hpp"

namespace YTE
{
  class SetWWiseSwitch : public QComboBox
  {
  public:
    SetWWiseSwitch(QWidget *aOwner, u64 aGroupId, WWiseSystem *aSystem, WWiseWidget *aWidget, bool aSwitch)
      : QComboBox(aOwner)
      , mGroupId(aGroupId)
      , mSystem(aSystem)
      , mWidget(aWidget)
      , mSwitch(aSwitch)
    {

    }

    void SetCurrentSwitchOrState()
    {
      auto id = this->currentData().toULongLong();

      if (mSwitch)
      {
        mSystem->SetSwitch(mGroupId, id, OwnerId());
      }
      else
      {
        mSystem->SetState(mGroupId, id);
      }
    }

    ~SetWWiseSwitch()
    {

    }

    AkGameObjectID OwnerId() { return reinterpret_cast<AkGameObjectID>(mWidget); };

    void indexChanged(int aIndex)
    {
      YTEUnusedArgument(aIndex);
      SetCurrentSwitchOrState();
    }

  private:
    WWiseSystem *mSystem;
    u64 mGroupId;
    WWiseWidget *mWidget;
    bool mSwitch;
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
    : QScrollArea(aParent)
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
      auto bankGroupBox = new QGroupBox(bank.second.mName.c_str(), this);
      QVBoxLayout *bankVbox = new QVBoxLayout(bankGroupBox);

      /////////////////////////////////////////////////////////////////////////////
      // Switches
      /////////////////////////////////////////////////////////////////////////////
      QGroupBox *switchGroupGroupBox = nullptr;

      if (bank.second.mSwitchGroups.size())
      {
        switchGroupGroupBox = new QGroupBox("Switch Groups", bankGroupBox);
        QVBoxLayout *switchGroupVbox = new QVBoxLayout(switchGroupGroupBox);
        for (auto &switchGroup : bank.second.mSwitchGroups)
        {
          auto dummy = new QWidget(switchGroupGroupBox);
          auto hbox = new QHBoxLayout(dummy);

          hbox->addWidget(new QLabel(switchGroup.first.c_str(), dummy));

          auto comboBox = new SetWWiseSwitch(dummy,
            switchGroup.second.first.mId,
            mSystem,
            this,
            true);



          for (auto &aSwitch : switchGroup.second.second)
          {
            comboBox->addItem(aSwitch.mName.c_str(), aSwitch.mId);
          }

          comboBox->SetCurrentSwitchOrState();

          this->connect(static_cast<QComboBox*>(comboBox),
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            comboBox,
            &SetWWiseSwitch::indexChanged);

          hbox->addWidget(comboBox);

          dummy->setLayout(hbox);

          switchGroupVbox->addWidget(dummy);
        }
        switchGroupVbox->addStretch(1);
        bankVbox->addWidget(switchGroupGroupBox);
      }

      /////////////////////////////////////////////////////////////////////////////
      // States
      /////////////////////////////////////////////////////////////////////////////
      QGroupBox *stateGroupGroupBox = nullptr;

      if (bank.second.mStateGroups.size())
      {
        stateGroupGroupBox = new QGroupBox("State Groups", bankGroupBox);
        QVBoxLayout *stateGroupVbox = new QVBoxLayout(stateGroupGroupBox);
        for (auto &stateGroup : bank.second.mStateGroups)
        {
          auto dummy = new QWidget(stateGroupGroupBox);
          auto hbox = new QHBoxLayout(dummy);

          hbox->addWidget(new QLabel(stateGroup.first.c_str(), dummy));

          auto comboBox = new SetWWiseSwitch(dummy,
            stateGroup.second.first.mId,
            mSystem,
            this,
            false);

          for (auto &state : stateGroup.second.second)
          {
            comboBox->addItem(state.mName.c_str(), state.mId);
          }

          comboBox->SetCurrentSwitchOrState();

          this->connect(static_cast<QComboBox*>(comboBox),
                        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                        comboBox,
                        &SetWWiseSwitch::indexChanged);

          hbox->addWidget(comboBox);

          dummy->setLayout(hbox);

          stateGroupVbox->addWidget(dummy);
        }
        stateGroupVbox->addStretch(1);
        bankVbox->addWidget(stateGroupGroupBox);
      }

      /////////////////////////////////////////////////////////////////////////////
      // Events
      /////////////////////////////////////////////////////////////////////////////
      QGroupBox *eventGroupBox = nullptr;

      if (bank.second.mEvents.size())
      {
        eventGroupBox = new QGroupBox("Events", bankGroupBox);
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
        bankVbox->addWidget(eventGroupBox);
      }

      //bankVbox->addWidget(rtpcGroupBox);
      bankVbox->addStretch(1);

      bankGroupBox->setLayout(bankVbox);

      layout->addWidget(bankGroupBox);
    }
  }
}