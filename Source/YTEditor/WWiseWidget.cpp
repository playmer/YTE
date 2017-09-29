#include <QPushButton>
#include <QGroupBox>
#include <QLayout>

#include "YTEditor/WWiseWidget.hpp"

#include "YTE/Core/Engine.hpp"

namespace YTE
{
  WWiseWidget::WWiseWidget(QWidget *aParent, Engine *aEngine)
    : QWidget(aParent)
    , mEngine(aEngine)
  {
    mSystem = mEngine->GetComponent<WWiseSystem>();
  }

  void WWiseWidget::LoadEvents()
  {
    auto &banks = mSystem->GetBanks();

    auto layout = new QVBoxLayout(this);

    this->setLayout(layout);

    for (auto &bank : banks)
    {
      auto groupBox = new QGroupBox(this);

      QVBoxLayout *vbox = new QVBoxLayout;


      for (auto &event : bank.second.mEvents)
      {
        QPushButton *toggleButton = new QPushButton(event.mName.c_str());

        vbox->addWidget(radio1);
      }

      vbox->addStretch(1);


      groupBox->setLayout(vbox);

      layout->addWidget(groupBox);
    }
  }
}