#include <qcomboBox>
#include <qpushButton>
#include <qgroupBox>
#include <qhboxLayout>
#include <qlineEdit>
#include <qlayout>
#include <qlabel>

#include "AK/SoundEngine/Common/AkSoundEngine.h"

#include "YTE/Core/Engine.hpp"

#include "YTEditor/MainWindow/Widgets/WWiseViewer/WWiseWidget.hpp"

namespace YTEditor
{
  class SetWWiseSwitch : public QComboBox
  {
  public:
    SetWWiseSwitch(QWidget *aOwner, YTE::u64 aGroupId, YTE::WWiseSystem *aSystem, WWiseWidget *aWidget, bool aSwitch)
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
      YTE::UnusedArguments(aIndex);
      SetCurrentSwitchOrState();
    }

  private:
    YTE::WWiseSystem *mSystem;
    YTE::u64 mGroupId;
    WWiseWidget *mWidget;
    bool mSwitch;
  };

  class SetWWiseRTPC : public QLineEdit
  {
  public:
    SetWWiseRTPC(QWidget *aOwner, YTE::u64 aRTPCId, YTE::WWiseSystem *aSystem)
      : QLineEdit(aOwner)
      , mRTPCId(aRTPCId)
      , mSystem(aSystem)
    {
      this->setValidator(new QDoubleValidator(this));
    }

    void SetRTPC(float aValue)
    {
      mSystem->SetRTPC(mRTPCId, aValue);
    }

    ~SetWWiseRTPC()
    {

    }

    void changedText(const QString &aText)
    {
      SetRTPC(aText.toFloat());
    }

  private:
    YTE::WWiseSystem *mSystem;
    YTE::u64 mRTPCId;
  };

  class SendWWiseEvent : public QPushButton
  {
  public:
    SendWWiseEvent(YTE::WWiseSystem *aSystem, 
                   const std::string &aEvent, 
                   YTE::u64 aEventId, 
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
    YTE::WWiseSystem *mSystem;
    YTE::u64 mEventId;
    WWiseWidget *mWidget;
  };

  WWiseWidget::WWiseWidget(MainWindow *aMainWindow, YTE::Engine *aEngine)
    : Widget(aMainWindow)
    , mEngine(aEngine)
  {
    std::string name{ "WWiseWidget" };
    mSystem = mEngine->GetComponent<YTE::WWiseSystem>();
    mSystem->RegisterObject(OwnerId(), name);
    auto listener = OwnerId();
    AK::SoundEngine::SetListeners(OwnerId(), &listener, 1);

    ConstructSubWidgets();

    LoadEvents();
  }

  WWiseWidget::~WWiseWidget()
  {
    mSystem->DeregisterObject(reinterpret_cast<AkGameObjectID>(this));
  }

  void WWiseWidget::LoadEvents()
  {
    auto &banks = mSystem->GetBanks();

    for (auto &bank : banks)
    {
      auto bankGroupBox = new QGroupBox(bank.second.mName.c_str(), this);
      QVBoxLayout *bankVbox = new QVBoxLayout(bankGroupBox);

      /////////////////////////////////////////////////////////////////////////////
      // RTPCs
      /////////////////////////////////////////////////////////////////////////////
      QGroupBox *rtpcGroupBox = nullptr;

      if (bank.second.mRTPCs.size())
      {
        rtpcGroupBox = new QGroupBox("Game Parameters", bankGroupBox);

        QVBoxLayout *rtpcGroupVbox = new QVBoxLayout(rtpcGroupBox);

        rtpcGroupBox->setLayout(rtpcGroupVbox);

        for (auto &rtpc : bank.second.mRTPCs)
        {
          auto dummy = new QWidget(rtpcGroupBox);
          auto hbox = new QHBoxLayout(dummy);

          hbox->addWidget(new QLabel(rtpc.mName.c_str(), dummy));

          auto lineEdit = new SetWWiseRTPC(dummy,
                                           rtpc.mId,
                                           mSystem);

          lineEdit->SetRTPC(0.0f);

          this->connect(static_cast<QLineEdit*>(lineEdit),
                        static_cast<void (QLineEdit::*)(const QString &)>(&QLineEdit::textChanged),
                        lineEdit,
                        &SetWWiseRTPC::changedText);

          hbox->addWidget(lineEdit);

          dummy->setLayout(hbox);

          rtpcGroupVbox->addWidget(dummy);
        }
        rtpcGroupVbox->addStretch(1);
        bankVbox->addWidget(rtpcGroupBox);
      }

      /////////////////////////////////////////////////////////////////////////////
      // Switches
      /////////////////////////////////////////////////////////////////////////////
      QGroupBox *switchGroupGroupBox = nullptr;

      if (bank.second.mSwitchGroups.size())
      {
        switchGroupGroupBox = new QGroupBox("Switch Groups", bankGroupBox);

        QVBoxLayout *switchGroupVbox = new QVBoxLayout(switchGroupGroupBox);

        switchGroupGroupBox->setLayout(switchGroupVbox);

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

      mSubWidgetLayout->addWidget(bankGroupBox);
    }
  }

  std::string WWiseWidget::GetName()
  {
    return "WWiseWidget";
  }

  Widget::DockArea WWiseWidget::GetDefaultDockPosition() const
  {
    return Widget::DockArea::Right;
  }

  void WWiseWidget::ConstructSubWidgets()
  {
    mLayout = new QVBoxLayout(this);

    mScrollArea = new QScrollArea();
    mScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    mSubWidget = new QWidget(mScrollArea);

    mSubWidgetLayout = new QVBoxLayout(mSubWidget);
    mSubWidget->setLayout(mSubWidgetLayout);

    mScrollArea->setWidget(mSubWidget);

    mLayout->addWidget(mScrollArea);

    mLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    mScrollArea->setWidgetResizable(true);

  }
}