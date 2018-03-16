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
    Conversation(DialogueNode *aRoot);
		DialogueNode *GetRoot() { return mRoot; };
  private:
    DialogueNode *mRoot;
  };

  class Quest
  {
  public:
    enum class State { Available, InProgress, Completed };
    enum class Name { Introduction, GuessChew, Ingredients, Cayenne };
    Quest() {};
    Quest(Quest::Name aName, Composition *aJohn);

    Quest::Name GetName() { return mName; };
    Quest::State GetState() { return mState; };
		std::vector<Conversation>::iterator GetActiveConvo() { return mActiveConvo; };

    void SetState(Quest::State aState) { mState = aState; };

    void AddConvo(Conversation *aConvo);
		void ConvoCompleted();
  private:
    Quest::Name mName;
    Quest::State mState;
		bool mConditionMet;
    std::vector<Conversation> mConversationVec;
		std::vector<Conversation>::iterator mActiveConvo;
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
  private:
    void OnDialogueStart(DialogueStart *aEvent);
    void RegisterJohn(CollisionStarted *aEvent);
    void DeregisterJohn(CollisionEnded *aEvent);

    std::vector<Quest> mQuestVec;
    Quest mActiveQuest;
  };

} //end yte
#endif
