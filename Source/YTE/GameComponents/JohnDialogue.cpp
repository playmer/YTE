/******************************************************************************/
/*!
\file   JohnDialogue.cpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-02-27

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/JohnDialogue.hpp"


namespace YTE
{
  YTEDefineType(JohnDialogue) { YTERegisterType(JohnDialogue); }

/******************************************************************************
  Conversation
		assumes the first element of the vector is the root
******************************************************************************/
  Conversation::Conversation(std::vector<DialogueNode> *aNodes)
		: mState(Conversation::State::Available)
  {
		for (DialogueNode i : *aNodes)
		{
			mNodeVec.push_back(i);
		}
  }

/******************************************************************************
  Quest
    guidelines:
      - Nodes are named nodeXY,
        -- X is the letter of the level
        -- Y is the number of the node on that level
      - Data objects are named dataXY,
        -- represents data of matching nodeXY

    Notes:
      Intro is a unique first quest.
			Every other quest has a strict structure { Hello, InProgress, TurnIn, Complete }
******************************************************************************/
  Quest::Quest(Quest::Name aName, std::vector<Conversation> *aConvos)
    : mName(aName), mState(Quest::State::Available), mConditionMet(false)
  {
		for (Conversation i : *aConvos)
		{
			mConversationVec.push_back(i);
		}
  }

/******************************************************************************
  Dialogue Component
******************************************************************************/
  JohnDialogue::JohnDialogue(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    YTEUnusedArgument(aProperties);

		/*
		root
		|
		A0
		|
		B0
		|  \
		C0  C1
		|  /
		D0
		*/

		// LEVEL D
		DialogueData(dataD0, "TEMP Go here, find this");
		DialogueNode nodeD0(DialogueNode::NodeType::Text, &dataD0, 0);

		// LEVEL C
		DialogueData(dataC0, "Perfect timing, my fish is still in the oven", "I need you to go fetch my missing ingredients");
		DialogueNode nodeC0(DialogueNode::NodeType::Text, &dataC0, 0);

		DialogueData(dataC1, "Suuuure...", "Well while you're here, I need you to fetch my missing ingredients");
		DialogueNode nodeC1(DialogueNode::NodeType::Text, &dataC1, 1);

		// LEVEL B
		DialogueData(dataB0, "Oh I'm not ordering, I'm here to help", "Friendship!");
		DialogueNode nodeB0(DialogueNode::NodeType::Input, &dataB0, 0);

		// LEVEL A
		DialogueData(dataA0, "HOT BEHIND!", "Just one second I'm finishing this meal", "Okay what did you want to order?");
		DialogueNode nodeA0(DialogueNode::NodeType::Text, &dataA0, 0);

		// LEVEL ROOT
		DialogueData(data, AnimationNames::WaveInit);
		DialogueNode root(DialogueNode::NodeType::Anim, &data, 0);

		std::vector<DialogueNode> *nodes = new std::vector<DialogueNode>{ root, nodeA0, nodeB0, nodeC0, nodeC1, nodeD0 };
		nodes->at(0).SetChildren(1, &nodes->at(1));
		nodes->at(1).SetChildren(1, &nodes->at(2));
		nodes->at(2).SetChildren(2, &nodes->at(3), &nodes->at(4));
		nodes->at(3).SetChildren(1, &nodes->at(5));
		nodes->at(4).SetChildren(1, &nodes->at(5));
		nodes->at(5).SetChildren(0, nullptr);
			// construct the convos
		Conversation c0(nodes);
			// add them to a vector to pass to quest
		std::vector<Conversation> *convos = new std::vector<Conversation>;
		convos->push_back(c0);
			// construct the quest with the convo vector ptr
    Quest intro(Quest::Name::Introduction, convos);
    mQuestVec.push_back(intro);
		
		mActiveQuest = &mQuestVec[0];
		mActiveConvo = &mActiveQuest->GetConversations()->at(0);
		mActiveNode = mActiveConvo->GetRoot();
  }

  void JohnDialogue::RegisterJohn(CollisionStarted *aEvent)
  {
    YTEUnusedArgument(aEvent);
    mSpace->YTERegister(Events::DialogueStart, this, &JohnDialogue::OnDialogueStart);
		mSpace->YTERegister(Events::DialogueNodeConfirm, this, &JohnDialogue::OnDialogueContinue);
		mSpace->YTERegister(Events::DialogueExit, this, &JohnDialogue::OnDialogueExit);
  }

  void JohnDialogue::DeregisterJohn(CollisionEnded *aEvent)
  {
    YTEUnusedArgument(aEvent);
    mSpace->YTEDeregister(Events::DialogueStart, this, &JohnDialogue::OnDialogueStart);
		mSpace->YTEDeregister(Events::DialogueNodeConfirm, this, &JohnDialogue::OnDialogueContinue);
		mSpace->YTEDeregister(Events::DialogueExit, this, &JohnDialogue::OnDialogueExit);
  }

  void JohnDialogue::Initialize()
  {
    mOwner->YTERegister(Events::CollisionStarted, this, &JohnDialogue::RegisterJohn);
    mOwner->YTERegister(Events::CollisionEnded, this, &JohnDialogue::DeregisterJohn);
  }

  void JohnDialogue::OnDialogueStart(DialogueStart *aEvent)
  {
			// if we exited early just start the conversation over
		if (mActiveConvo->GetState() == Conversation::State::EarlyExit)
		{
			mActiveNode = mActiveConvo->GetRoot();
		}
		// @@@(JAY): This is temporary since we dont have another convo yet, prevents breaking
		else if (mActiveConvo->GetState() == Conversation::State::Completed)
		{
			mActiveNode = mActiveConvo->GetRoot();
		}

		DialogueNode::NodeType type = mActiveNode->GetNodeType();
			// For anims and sounds we wont hear back from the director so send an event to ourselves to begin
		if (type == DialogueNode::NodeType::Anim || type == DialogueNode::NodeType::Sound)
		{
				// Anims and Sounds always have 1 child
			DialogueNodeConfirm next(0);
			mSpace->SendEvent(Events::DialogueNodeConfirm, &next);
		}
			// For input and text we rely on the director responding
		else if (type == DialogueNode::NodeType::Input || type == DialogueNode::NodeType::Text)
		{
			DialogueNodeReady next(mActiveNode->GetNodeData());
			next.DialogueType = type;
			mSpace->SendEvent(Events::DialogueNodeReady, &next);
		}
  }

	void JohnDialogue::OnDialogueExit(DialogueExit *aEvent)
	{
		mActiveConvo->SetState(Conversation::State::EarlyExit);
	}

	void JohnDialogue::OnDialogueContinue(DialogueNodeConfirm *aEvent)
	{
		mActiveNode->ActivateNode();
		mActiveNode = mActiveNode->GetChild(aEvent->Selection);
		if (mActiveNode != nullptr)
		{
			DialogueNode::NodeType type = mActiveNode->GetNodeType();
			if (type == DialogueNode::NodeType::Anim || type == DialogueNode::NodeType::Sound)
			{
				// Anims and Sounds always have 1 child
				DialogueNodeConfirm next(0);
				mSpace->SendEvent(Events::DialogueNodeConfirm, &next);
			}
			// For input and text we rely on the director responding
			else if (type == DialogueNode::NodeType::Input || type == DialogueNode::NodeType::Text)
			{
				DialogueNodeReady next(mActiveNode->GetNodeData());
				next.DialogueType = type;
				mSpace->SendEvent(Events::DialogueNodeReady, &next);
			}
		}
		else
		{
			DialogueExit diagExit;
			mSpace->SendEvent(Events::DialogueExit, &diagExit);
			mActiveConvo->SetState(Conversation::State::Completed);
		}
	}
} //end yte