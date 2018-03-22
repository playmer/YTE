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
  /////////////////////////////////////////////////////////////////////////////////////
  // Data Structure Classes
  /////////////////////////////////////////////////////////////////////////////////////
  class Conversation
  {
  public:
		enum class State { Available, Completed, EarlyExit};
		Conversation() {};
    Conversation(std::vector<DialogueNode> *aNodes);

		DialogueNode *GetRoot() { return &mNodeVec[0]; };
		Conversation::State GetState() { return mState; };
		void SetState(Conversation::State aState) { mState = aState; };
  private:
		std::vector<DialogueNode> mNodeVec;
		Conversation::State mState;
  };

  class Quest
  {
  public:
    enum class State { Available, InProgress, Completed };
    enum class Name { Introduction, GuessChew, Ingredients, Cayenne };
    Quest() {};
    Quest(Quest::Name aName, std::vector<Conversation> *aConvos);

    Quest::Name GetName() { return mName; };
		std::vector<Conversation> *GetConversations() { return &mConversationVec; };
    Quest::State GetState() { return mState; };
    void SetState(Quest::State aState) { mState = aState; };
  private:
    Quest::Name mName;
    Quest::State mState;
		bool mConditionMet;

    std::vector<Conversation> mConversationVec;
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
		// this cant be used until we know the location of the node
		void SetActiveNode(DialogueNode *aNode) { mActiveNode = aNode; };
  private:
    void OnDialogueStart(DialogueStart *aEvent);
		void OnDialogueExit(DialogueExit *aEvent);
    void RegisterJohn(CollisionStarted *aEvent);
    void DeregisterJohn(CollisionEnded *aEvent);
		void OnDialogueContinue(DialogueNodeConfirm *aEvent);

    std::vector<Quest> mQuestVec;
		Quest *mActiveQuest;
		Conversation *mActiveConvo;
		DialogueNode *mActiveNode;
  };

} //end yte
#endif
