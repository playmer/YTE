/******************************************************************************/
/*!
\file   QuestProgressionTrigger.cpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-03-27

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/QuestProgressionTrigger.hpp"
#include "YTE/GameComponents/ProgressionParticles.hpp"

namespace YTE
{
  YTEDefineType(QuestProgressionTrigger) { RegisterType<QuestProgressionTrigger>();
    TypeBuilder<QuestProgressionTrigger> builder; }

  QuestProgressionTrigger::QuestProgressionTrigger(Composition *aOwner, 
                                                   Space *aSpace, 
                                                   RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mDeleteFlag(false)
    , mDeleteCounter(0)
  {
    UnusedArguments(aProperties);
  }

  void QuestProgressionTrigger::Initialize()
  {
    mSpace->RegisterEvent<&QuestProgressionTrigger::OnLogicUpdate>(Events::LogicUpdate, this);
    mOwner->RegisterEvent<&QuestProgressionTrigger::OnCollisionStarted>(Events::CollisionStarted, this);
  }

  void QuestProgressionTrigger::OnLogicUpdate(LogicUpdate *)
  {
    if (mDeleteFlag)
    {
      if (mDeleteCounter > 20)
      {
        mOwner->GetParent()->RemoveComposition(mOwner);
      }
      else
      {
        mDeleteCounter++;
      }
    }
  }

  void QuestProgressionTrigger::OnCollisionStarted(CollisionStarted *aEvent)
  {
    if (aEvent->OtherObject->GetComponent<QuestLogic>() != nullptr)
    {
      ProgressionItemEvent item;
      mSpace->SendEvent(Events::ProgressionItemEvent, &item);
      //mOwner->GetParent()->RemoveComposition(mOwner);

      ProgressionHappened progEvent;
      mSpace->SendEvent(Events::ProgressionHappened, &progEvent);

      mOwner->GetComponent<Transform>()->SetWorldTranslation(0, -200, 0);
      mDeleteFlag = true;

      auto emitter = mOwner->GetComponent<WWiseEmitter>();

      if (emitter)
      {
        emitter->PlayEvent("UI_Quest_Progress");
      }
    }
  }
}//end yte