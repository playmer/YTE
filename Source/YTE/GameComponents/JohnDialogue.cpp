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
******************************************************************************/
  Conversation::Conversation(DialogueNode *aRoot)
    : mRoot(aRoot)
  {
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
  Quest::Quest(Quest::Name aName, Composition *aJohn)
    : mName(aName), mState(State::Available), mConditionMet(false)
  {
    mConversationVec = *(new std::vector<Conversation>());
    switch (aName)
    {
      case Quest::Name::Introduction:
      {
          // This is an array that will be used to hook up children pointers and will be overwritten many times
        std::vector<DialogueNode*> children;

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
        DialogueNode nodeD0(DialogueNode::NodeType::Text, nullptr, &dataD0, aJohn, 0);

        children.push_back(&nodeD0);

        // LEVEL C
        DialogueData(dataC0, "Perfect timing, my fish is still in the oven", "I need you to go fetch my missing ingredients");
        DialogueNode nodeC0(DialogueNode::NodeType::Text, &children, &dataC0, aJohn, 0);

        DialogueData(dataC1, "Suuuure...", "Well while you're here, I need you to fetch my missing ingredients");
        DialogueNode nodeC1(DialogueNode::NodeType::Text, &children, &dataC1, aJohn, 1);

        children.clear();
        children.push_back(&nodeC0);
        children.push_back(&nodeC1);

        // LEVEL B
        DialogueData(dataB0, "Oh I'm not ordering, I'm here to help", "Friendship!");
        DialogueNode nodeB0(DialogueNode::NodeType::Input, &children, &dataB0, aJohn, 0);

        children.clear();
        children.push_back(&nodeB0);

        // LEVEL A
        DialogueData(dataA0, "HOT BEHIND!", "Just one second I'm finishing this meal", "Okay what did you want to order?");
        DialogueNode nodeA0(DialogueNode::NodeType::Text, &children, &dataA0, aJohn, 0);

        children.clear();
        children.push_back(&nodeA0);

        // LEVEL ROOT
        DialogueData(data, AnimationNames::WaveInit);
        DialogueNode root(DialogueNode::NodeType::Anim, &children, &data, aJohn, 0);

        Conversation c0(&root);

					// The introduction quest only has one conversation
        mConversationVec.push_back(c0);

        // register the first node for when we start        
        root.mOwner->GetSpace()->YTERegister(Events::DialogueNodeConfirm, &root, &DialogueNode::NextNode);
        //delete &children;
        break;
      }
      case Quest::Name::GuessChew:
      {

      }
      case Quest::Name::Ingredients:
      {

      }
      case Quest::Name::Cayenne:
      {

      }
    }
		mActiveConvo = mConversationVec.begin();
  }
  
  void Quest::AddConvo(Conversation *aConvo)
  {
    mConversationVec.push_back(*aConvo);
  }

	// This needs to change to checking when you start a conversation not when it ends, since that is when we decide what conversation to run
	void Quest::ConvoCompleted()
	{
		switch (mState)
		{
			case Quest::State::Available:
			{
				++mActiveConvo;
				mState = State::InProgress;
				break;
			}
			case Quest::State::InProgress:
			{
				if (!mConditionMet)
				{
					mActiveConvo = mConversationVec.end() - 2;
				}
				else
				{
					
				}
				break;
			}
			case Quest::State::Completed:
			{
				mActiveConvo = mConversationVec.end() - 1;
				break;
			}
		}
	}

/******************************************************************************
  Dialogue Component
******************************************************************************/
  JohnDialogue::JohnDialogue(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    YTEUnusedArgument(aProperties);
      // Construct the Quest Vector
    /*mQuestVec = *(new std::vector<Quest> { 
      (Quest::Name::Introduction, aOwner),
      (Quest::Name::GuessChew, aOwner),
      (Quest::Name::Cayenne, aOwner),
      (Quest::Name::Ingredients, aOwner)
    });*/

    Quest intro(Quest::Name::Introduction, aOwner);
    mQuestVec.push_back(intro);
  }

  void JohnDialogue::RegisterJohn(CollisionStarted *aEvent)
  {
    YTEUnusedArgument(aEvent);
    mSpace->YTERegister(Events::DialogueStart, this, &JohnDialogue::OnDialogueStart);
  }
  void JohnDialogue::DeregisterJohn(CollisionEnded *aEvent)
  {
    YTEUnusedArgument(aEvent);
    mSpace->YTEDeregister(Events::DialogueStart, this, &JohnDialogue::OnDialogueStart);
  }

  void JohnDialogue::Initialize()
  {
    mOwner->YTERegister(Events::CollisionStarted, this, &JohnDialogue::RegisterJohn);
    mOwner->YTERegister(Events::CollisionEnded, this, &JohnDialogue::DeregisterJohn);
  }

  void JohnDialogue::OnDialogueStart(DialogueStart *aEvent)
  {
    YTEUnusedArgument(aEvent);
		//Conversation *temp = &(*(mActiveQuest.GetActiveConvo()));

    // this is just so that we start a conversation the same way as advancing
    DialogueNodeConfirm conf(0);
		mSpace->SendEvent(Events::DialogueNodeConfirm, &conf);
  }
} //end yte