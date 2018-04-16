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

namespace YTE
{
  YTEDefineType(QuestProgressionTrigger) { YTERegisterType(QuestProgressionTrigger); }

  QuestProgressionTrigger::QuestProgressionTrigger(Composition *aOwner, 
                                                   Space *aSpace, 
                                                   RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    YTEUnusedArgument(aProperties);
  }

  void QuestProgressionTrigger::Initialize()
  {
    mOwner->YTERegister(Events::CollisionStarted, this, &QuestProgressionTrigger::OnCollisionStarted);
  }

  void QuestProgressionTrigger::OnCollisionStarted(CollisionStarted *aEvent)
  {
    if (aEvent->OtherObject->GetComponent<QuestLogic>() != nullptr)
    {
      ProgressionItemEvent item;
      mSpace->SendEvent(Events::ProgressionItemEvent, &item);
      mOwner->GetParent()->RemoveComposition(mOwner);
    }
  }
}//end yte