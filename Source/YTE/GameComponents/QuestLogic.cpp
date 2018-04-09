/******************************************************************************/
/*!
\file   QuestLogic.cpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-03-25

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/QuestLogic.hpp"
#include "YTE/GameComponents/JohnDialogue.hpp"
#include "YTE/GameComponents/QuestProgressionTrigger.hpp"

namespace YTE
{
  YTEDefineType(QuestLogic) { YTERegisterType(QuestLogic); }

  QuestLogic::QuestLogic(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    YTEUnusedArgument(aProperties);
  }

  void QuestLogic::Initialize()
  {
    mOwner->YTERegister(Events::CollisionStarted, this, &QuestLogic::OnCollisionStarted);
  }

  void QuestLogic::OnCollisionStarted(CollisionStarted *aEvent)
  {
    QuestProgressionTrigger *progression = aEvent->OtherObject->GetComponent<QuestProgressionTrigger>();
    if (progression != nullptr)
    {
      if (progression->IsCollisionTrigger())
      {
        UpdateActiveQuestState update((*mPostcardHandle)->GetCharacter(), Quest::State::Accomplished);
        mSpace->SendEvent(Events::UpdateActiveQuestState, &update);
      }
    }
  }

}//end yte