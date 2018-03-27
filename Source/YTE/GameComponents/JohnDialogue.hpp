/******************************************************************************/
/*!
\file   JohnDialogue.hpp
\author Jonathan Ackerman
\par    email : jonathan.ackerman\@digipen.edu
\date   2018 - 02 - 27
\brief
Unique component for John's dialogues. This component defines and runs logic
for all John conversations

All content(c) 2016 DigiPen(USA) Corporation, all rights reserved.
* /
/******************************************************************************/
#pragma once

#ifndef YTE_Gameplay_JohnDialogue_hpp
#define YTE_Gameplay_JohnDialogue_hpp

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/GameComponents/BoatController.hpp"
#include "YTE/GameComponents/DialogueGraph.hpp"
#include "YTE/GameComponents/DialogueDirector.hpp"

namespace YTE
{
  class Quest; /* forward decl for event */

  YTEDeclareEvent(NoticeBoardHookup);
  YTEDeclareEvent(UpdateActiveQuestState);
  YTEDeclareEvent(QuestStart);

  class NoticeBoardHookup : public Event
  {
  public:
    YTEDeclareType(NoticeBoardHookup);
    NoticeBoardHookup(Quest **aQuest) : mActiveQuestHandle(aQuest) {};
    Quest **mActiveQuestHandle;
  };

  class Conversation; /* forward decl for Quest class */
  /////////////////////////////////////////////////////////////////////////////////////
  // Data Structure Classes
  /////////////////////////////////////////////////////////////////////////////////////
  class Quest
  {
  public:
    enum class State { NotActive, Received, Briefed, Accomplished, Completed, TurnedIn };
    enum class Name { Introduction, Fetch, Explore, Dialogue, NotActive };
    enum class CharacterName { John, Daisy, Basil };
    Quest(Quest::Name aName);

    Quest::Name GetName() { return mName; };
    Quest::CharacterName GetCharacter() { return mCharacter; };
    std::vector<Conversation> *GetConversations() { return &mConversationVec; };
    Quest::State GetState() { return mState; };
    void SetState(Quest::State aState) { mState = aState; };
  private:
    Quest::Name mName;
    Quest::State mState;
    CharacterName mCharacter;
    std::vector<Conversation> mConversationVec;
  };

  class Conversation
  {
  public:
    enum class State { Available, Completed, EarlyExit };
    enum class Name { Hello, NoProgress, Completed, PostQuest };
    Conversation(Conversation::Name aName, Quest::Name aQuest);
    //Conversation(std::vector<DialogueNode> *aNodes);

      // Root is the last element in the vector due to bottom up construction
    DialogueNode *GetRoot() { return &mNodeVec.back(); };
    Conversation::State GetState() { return mState; };
    void SetState(Conversation::State aState) { mState = aState; };
  private:
    Conversation::Name mName;
    Conversation::State mState;
    std::vector<DialogueNode> mNodeVec;
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

  /////////////////////////////////////////////////////////////////////////////////////
  // Component
  /////////////////////////////////////////////////////////////////////////////////////
  class JohnDialogue : public Component
  {
  public:
    YTEDeclareType(JohnDialogue);
    JohnDialogue(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;
    void Start() override;
    // this cant be used until we know the location of the node
    void SetActiveNode(DialogueNode *aNode) { mActiveNode = aNode; };
  private:
    void RegisterJohn(CollisionStarted *aEvent);
    void DeregisterJohn(CollisionEnded *aEvent);
    void OnDialogueStart(DialogueStart *aEvent);
    void OnDialogueExit(DialogueExit *aEvent);
    void OnDialogueContinue(DialogueNodeConfirm *aEvent);
    void OnQuestStart(QuestStart *aEvent);
    void OnUpdateActiveQuestState(UpdateActiveQuestState *aEvent);

    std::vector<Quest> mQuestVec; // maybe this shoulda been const or something, how do i distinguish members that need to change from members that should never change (Data vs Accessor)
    Quest *mActiveQuest;
    Conversation *mActiveConvo;
    DialogueNode *mActiveNode;
    Quest::CharacterName mName = Quest::CharacterName::John;
  };
} //end yte
#endif
