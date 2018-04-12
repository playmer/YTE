/******************************************************************************/
/*!
\file   QuestLogic.hpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-03-25
\brief
    This component handles the logic for changing the quest states

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once
#ifndef YTE_Gameplay_QuestLogic_hpp
#define YTE_Gameplay_QuestLogic_hpp

#include "YTE/Core/Composition.hpp"
#include "YTE/Physics/BoxCollider.hpp"

#include "YTE/GameComponents/JohnDialogue.hpp"
#include "YTE/GameComponents/NoticeBoard.hpp"

namespace YTE
{
  YTEDeclareEvent(ProgressionItemEvent);
  YTEDeclareEvent(ProgressionLocationEvent);
  YTEDeclareEvent(ProgressionDialogueEvent);

  class ProgressionItemEvent : public Event
  {
  public:
    YTEDeclareType(ProgressionItemEvent);
  };

  class ProgressionLocationEvent : public Event
  {
  public:
    YTEDeclareType(ProgressionLocationEvent);
  };

  class ProgressionDialogueEvent : public Event
  {
  public:
    YTEDeclareType(ProgressionDialogueEvent);
  };

  class QuestLogic : public Component
  {
  public:
    YTEDeclareType(QuestLogic);
    QuestLogic(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;

    void HookupPostcardHandle(Postcard **aHandle) { mPostcardHandle = aHandle; };
    Postcard *GetPostcard() { return *mPostcardHandle; };

    void OnProgressionItemEvent(ProgressionItemEvent *aEvent);
    void OnProgressionLocationEvent(ProgressionLocationEvent *aEvent);
    void OnProgressionDialogueEvent(ProgressionDialogueEvent *aEvent);

    void OnSpawnProgressionItem(SpawnProgressionItem *aEvent);
    void OnSpawnProgressionLocation(SpawnProgressionLocation *aEvent);
    void OnSpawnProgressionDialogue(SpawnProgressionDialogue *aEvent);
  private:
    Postcard **mPostcardHandle;
  };
}//end yte
#endif