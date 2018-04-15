/******************************************************************************/
/*!
\file   Quest.hpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-03-27
\brief
    This is the definition of the Quest class used by Dialogue components

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once
#ifndef YTE_Gameplay_Quest_hpp
#define YTE_Gameplay_Quest_hpp

#include "YTE/Core/Engine.hpp"

#include "YTE/GameComponents/DialogueGraph.hpp"

namespace YTE
{
  YTEDeclareEvent(UpdateActiveQuestState);
  YTEDeclareEvent(QuestStart);

  class Conversation; /* forward decl for Quest class */

  class Quest
  {
  public:
    enum class State { NotActive, Received, Briefed, Accomplished, Completed, TurnedIn };
    enum class Name { Introduction, Fetch, Explore, Dialogue, NotActive };
    enum class CharacterName { John, Daisy, Basil };

    YTEDeclareType(Quest);
    Quest(Quest::Name aName, Quest::CharacterName aCharacter, Space *aSpace);

    Quest::Name GetName() { return mName; };
    Quest::CharacterName GetCharacter() { return mCharacter; };
    std::vector<Conversation> *GetConversations() { return &mConversationVec; };
    Quest::State GetState() { return mState; };
    void SetState(Quest::State aState) { mState = aState; };
  private:
    Quest::State mState;

    Space *mSpace;

    Quest::Name mName;
    CharacterName mCharacter;
    std::vector<Conversation> mConversationVec;
  };

  class Conversation
  {
  public:
    enum class State { Available, Completed, EarlyExit };
    enum class Name { Hello, NoProgress, Completed, PostQuest };

    YTEDeclareType(Conversation);
    Conversation(Conversation::Name aName, Quest::Name aQuest, Quest::CharacterName aCharacter, Space *space);

    // Root is the last element in the vector due to bottom up construction
    DialogueNode *GetRoot() { return &mNodeVec.back(); };
    Conversation::State GetState() { return mState; };
    Conversation::Name GetName() { return mName; };
    void SetState(Conversation::State aState) { mState = aState; };
  private:
    Conversation::State mState;

    Conversation::Name mName;
    std::vector<DialogueNode> mNodeVec;

    Space *mSpace;
  };

  class UpdateActiveQuestState : public Event
  {
  public:
    YTEDeclareType(UpdateActiveQuestState);
    UpdateActiveQuestState(Quest::CharacterName aCharacter, Quest::State aState) : mCharacter(aCharacter), mState(aState) {};
    Quest::CharacterName mCharacter;
    Quest::State mState;
  };

  class QuestStart : public Event
  {
  public:
    YTEDeclareType(QuestStart);
    QuestStart(Quest::CharacterName aCharacter, Quest::Name aQuest) : mCharacter(aCharacter), mQuest(aQuest) {};
    Quest::CharacterName mCharacter;
    Quest::Name mQuest;
  };
}//end yte
#endif