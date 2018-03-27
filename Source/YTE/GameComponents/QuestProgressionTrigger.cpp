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
#include "YTE/GameComponents/QuestLogic.hpp"
#include "YTE/GameComponents/JohnDialogue.hpp"

namespace YTE
{
  YTEDefineType(QuestProgressionTrigger) { YTERegisterType(QuestProgressionTrigger); }

  QuestProgressionTrigger::QuestProgressionTrigger(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    YTEBindProperty(&QuestProgressionTrigger::GetTriggerType, &QuestProgressionTrigger::SetTriggerType, "IsCollisionTrigger")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    std::vector<std::vector<Type*>> deps =
    {
      { TypeId<BoxCollider>() }
    };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);
  }

  void QuestProgressionTrigger::Initialize()
  {
    if (mIsCollisionTrigger)
    {
      mOwner->YTERegister(Events::CollisionStarted, this, &QuestProgressionTrigger::OnCollisionStarted);
    }
    else
    {

    }
  }

  void QuestProgressionTrigger::OnCollisionStarted(CollisionStarted *aEvent)
  {
    QuestLogic *questLogic = aEvent->OtherObject->GetComponent<QuestLogic>();
    if (questLogic != nullptr)
    {
        // Fetch and Explore triggers behave the same
      UpdateActiveQuestState update(questLogic->GetPostcard()->GetCharacter(), Quest::State::Accomplished);
      mSpace->SendEvent(Events::UpdateActiveQuestState, &update);
    }
  }
}//end yte