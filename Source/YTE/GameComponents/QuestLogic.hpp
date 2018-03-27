/******************************************************************************/
/*!
\file   QuestLogic.hpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-03-25
\brief
    This component handles the logic for changing the dialogues and quest states

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once
#ifndef YTE_Gameplay_QuestLogic_hpp
#define YTE_Gameplay_QuestLogic_hpp

#include "YTE/Core/Composition.hpp"
#include "YTE/GameComponents/JohnDialogue.hpp" /* Quest */

namespace YTE
{
  YTEDeclareEvent(UpdateQuestState);
  YTEDeclareEvent(QuestStart);
  YTEDeclareEvent(AssignPostcard);

  class Postcard; /* forward decl */

  class AssignPostcard : public Event
  {
  public:
    YTEDeclareType(AssignPostcard);
    AssignPostcard(Postcard *aPostcard) : mPostcard(aPostcard) {};
    Postcard *mPostcard;
  };

  class UpdateQuestState : public Event
  {
  public:
    YTEDeclareType(UpdateQuestState);
    UpdateQuestState(Quest::State aState) : mState(aState) {};
    Quest::State mState;
  };
  class QuestStart : public Event
  {
  public:
    YTEDeclareType(QuestStart);
    QuestStart(Quest::Name aName) : mName(aName) {};
    Quest::Name mName;
  };

  class QuestLogic; /* forward decl for the Postcard class */

  class Postcard
  {
  public:
    Postcard(Quest::CharacterName aCharacterName, Quest::Name aQuestName) : mCharacterName(aCharacterName), mQuestName(aQuestName) {};
    Quest::CharacterName GetCharacter() { return mCharacterName; };
    Quest::Name GetQuest() { return mQuestName; };
  private:
    Quest::CharacterName mCharacterName;
    Quest::Name mQuestName;
  };

  class QuestLogic : public Component
  {
  public:
    YTEDeclareType(QuestLogic);
    QuestLogic(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;

    Postcard *GetCurrentPostcard() { return mReceivedPostcard; };
    void OnAssignPostcard(AssignPostcard *aEvent);
    void OnStartQuest(QuestStart *aEvent);
    void OnUpdateQuestState(UpdateQuestState *aEvent);
  private:
    Postcard *mReceivedPostcard;
  };
}//end yte
#endif