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
  YTEDeclareEvent(ActiveQuestBroadcast);
  class ActiveQuestBroadcast : public Event
  {
  public:
    YTEDeclareType(ActiveQuestBroadcast);
    ActiveQuestBroadcast(Quest *aQuest) : mActiveQuest(aQuest) {};
    Quest *mActiveQuest;
  };

  class Conversation; /* forward decl for Quest class */
  /////////////////////////////////////////////////////////////////////////////////////
  // Data Structure Classes
  /////////////////////////////////////////////////////////////////////////////////////
  class Quest
  {
  public:
    enum class State { NotActive, Received, Briefed, Accomplished, Completed, TurnedIn };
    enum class Name { Introduction, Fetch, Explore, Dialogue };
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

    std::vector<Quest> mQuestVec;
    Quest *mActiveQuest;
    Conversation *mActiveConvo;
    DialogueNode *mActiveNode;
  };

} //end yte
#endif
