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
#include "YTE/GameComponents/QuestProgressionTrigger.hpp"

namespace YTE
{
  YTEDefineEvent(ProgressionItemEvent);
  YTEDefineEvent(ProgressionLocationEvent);
  YTEDefineEvent(ProgressionDialogueEvent);

  YTEDefineType(ProgressionItemEvent) { YTERegisterType(ProgressionItemEvent); }
  YTEDefineType(ProgressionLocationEvent) { YTERegisterType(ProgressionLocationEvent); }
  YTEDefineType(ProgressionDialogueEvent) { YTERegisterType(ProgressionDialogueEvent); }

  YTEDefineType(QuestLogic) { YTERegisterType(QuestLogic); }

  QuestLogic::QuestLogic(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    YTEUnusedArgument(aProperties);
  }

  void QuestLogic::Initialize()
  {
    mSpace->YTERegister(Events::SpawnProgressionItem, this, &QuestLogic::OnSpawnProgressionItem);
    mSpace->YTERegister(Events::SpawnProgressionLocation, this, &QuestLogic::OnSpawnProgressionLocation);
    mSpace->YTERegister(Events::SpawnProgressionDialogue, this, &QuestLogic::OnSpawnProgressionDialogue);

    mSpace->YTERegister(Events::ProgressionItemEvent, this, &QuestLogic::OnProgressionItemEvent);
    mSpace->YTERegister(Events::ProgressionLocationEvent, this, &QuestLogic::OnProgressionLocationEvent);
    mSpace->YTERegister(Events::ProgressionDialogueEvent, this, &QuestLogic::OnProgressionDialogueEvent);
  }

  void QuestLogic::OnProgressionItemEvent(ProgressionItemEvent *aEvent)
  {
    UpdateActiveQuestState update((*mPostcardHandle)->GetCharacter(), Quest::State::Accomplished);
    mSpace->SendEvent(Events::UpdateActiveQuestState, &update);
  }

  void QuestLogic::OnProgressionLocationEvent(ProgressionLocationEvent *aEvent)
  {

  }

  void QuestLogic::OnProgressionDialogueEvent(ProgressionDialogueEvent *aEvent)
  {

  }

  void QuestLogic::OnSpawnProgressionItem(SpawnProgressionItem *aEvent)
  {
    mSpace->AddCompositionAtPosition("ProgressionItem", "item", glm::vec3(0, 0, 0));
  }

  void QuestLogic::OnSpawnProgressionLocation(SpawnProgressionLocation *aEvent)
  {

  }

  void QuestLogic::OnSpawnProgressionDialogue(SpawnProgressionDialogue *aEvent)
  {

  }


}//end yte