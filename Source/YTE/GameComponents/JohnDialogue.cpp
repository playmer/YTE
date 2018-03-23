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
	Conversation::Conversation(Conversation::Name aName, Quest::Name aQuest)
		: mName(aName), mState(Conversation::State::Available)
	{
		switch (aQuest)
		{
			case Quest::Name::Introduction:
			{
				switch (aName)
				{
					case Conversation::Name::Hello:
					{
						// LEVEL G
						DialogueData(dataG0, "John: Nice to meet you though.", "John: Hopefully we bump into each other again");
						mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataG0, 0);

						// LEVEL F
						DialogueData(dataF0, AnimationNames::Idle);
						mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataF0, 0);

						// LEVEL E
						DialogueData(dataE0, "John: ...but I'm all out now, sorry.");
						mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataE0, 0);

						// LEVEL D
						DialogueData(dataD0, AnimationNames::Sad);
						mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataD0, 0);

						// LEVEL C
						DialogueData(dataC0, "John: Today I was serving Gazpacho...");
						mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataC0, 0);

						// LEVEL B
						DialogueData(dataB0, "Smells great!");
						mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataB0, 0);

						// LEVEL A
						DialogueData(dataA0, "John: Welcome to my pop up restaurant, Que Delicioso.");
						mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataA0, 0);

						// LEVEL ROOT
						DialogueData(dataR0, AnimationNames::WaveInit);
						mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataR0, 0);
						
						/*
						G0 - F0 - E0 - D0 - C0 - B0 - A0 - R0
						*/
						enum { G0, F0, E0, D0, C0, B0, A0, R0};
						mNodeVec[G0].SetChildren(DialogueNodeChildren{});
						mNodeVec[F0].SetChildren(DialogueNodeChildren{&mNodeVec[G0]});
						mNodeVec[E0].SetChildren(DialogueNodeChildren{&mNodeVec[F0]});
						mNodeVec[D0].SetChildren(DialogueNodeChildren{&mNodeVec[E0]});
						mNodeVec[C0].SetChildren(DialogueNodeChildren{&mNodeVec[D0]});
						mNodeVec[B0].SetChildren(DialogueNodeChildren{&mNodeVec[C0]});
						mNodeVec[A0].SetChildren(DialogueNodeChildren{&mNodeVec[B0]});
						mNodeVec[R0].SetChildren(DialogueNodeChildren{&mNodeVec[A0]});
						break;
					}
					case Conversation::Name::PostQuest:
					{
						// LEVEL B
						DialogueData(dataB0, "You too!");
						mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataB0, 0);

						// LEVEL A
						DialogueData(dataA0, "John: Have a great day");
						mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataA0, 0);

						// LEVEL ROOT
						DialogueData(dataR0, AnimationNames::WaveInit);
						mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataR0, 0);

						/*
						B0 - A0 - R0
						*/
						enum { B0, A0, R0 };
						mNodeVec[B0].SetChildren(DialogueNodeChildren{ });
						mNodeVec[A0].SetChildren(DialogueNodeChildren{ &mNodeVec[B0] });
						mNodeVec[R0].SetChildren(DialogueNodeChildren{ &mNodeVec[A0] });
						break;
					}
				}
				break;
			}
			case Quest::Name::Fetch:
			{
				switch (aName)
				{
					case Conversation::Name::Hello:
					{
						// LEVEL E
						DialogueData(dataE0, "John: Can you go out and look for them?");
						mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataE0, 0);
						DialogueData(dataE1, "John: I knew I could count on you!");
						mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataE1, 1);
						DialogueData(dataE2, "John: Thanks!");
						mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataE2, 2);

						// LEVEL D
						DialogueData(dataD0, AnimationNames::Sad);
						mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataD0, 0);
						DialogueData(dataD1, AnimationNames::Happy);
						mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataD1, 1);
						DialogueData(dataD2, AnimationNames::Happy);
						mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataD2, 2);

						// LEVEL C
						DialogueData(dataC0, "Oh no!", "Your sous chef is on the job!", "Sure thing");
						mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataC0, 0);

						// LEVEL B
						DialogueData(dataB0, "John: ...but they haven't shown up yet");
						mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataB0, 0);

						// LEVEL A
						DialogueData(dataA0, AnimationNames::Angry);
						mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataA0, 0);

						// LEVEL ROOT
						DialogueData(dataR0, "John: Perfect timing!", "John: I'm expecting a delivery of ingredients...");
						mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataR0, 0);

						/*
						E0 - D0 - C0 - B0 - A0 - R0
						E1 - D1 -|
						E2 - D2 -|
						*/
						enum { E0, E1, E2, D0, D1, D2, C0, B0, A0, R0 };
						mNodeVec[E0].SetChildren(DialogueNodeChildren{ });
						mNodeVec[E1].SetChildren(DialogueNodeChildren{ });
						mNodeVec[E2].SetChildren(DialogueNodeChildren{ });
						mNodeVec[D0].SetChildren(DialogueNodeChildren{ &mNodeVec[E0] });
						mNodeVec[D1].SetChildren(DialogueNodeChildren{ &mNodeVec[E1] });
						mNodeVec[D2].SetChildren(DialogueNodeChildren{ &mNodeVec[E2] });
						mNodeVec[C0].SetChildren(DialogueNodeChildren{ &mNodeVec[D0], &mNodeVec[D1], &mNodeVec[D2] });
						mNodeVec[B0].SetChildren(DialogueNodeChildren{ &mNodeVec[C0] });
						mNodeVec[A0].SetChildren(DialogueNodeChildren{ &mNodeVec[B0] });
						mNodeVec[R0].SetChildren(DialogueNodeChildren{ &mNodeVec[A0] });
						break;
					}
					case Conversation::Name::NoProgress:
					{

						break;
					}
					case Conversation::Name::Completed:
					{

						break;
					}
					case Conversation::Name::PostQuest:
					{

						break;
					}
				}
				break;
			}
			case Quest::Name::Explore:
			{
				switch (aName)
				{
					case Conversation::Name::Hello:
					{
						break;
					}
					case Conversation::Name::NoProgress:
					{
						break;
					}
					case Conversation::Name::Completed:
					{
						break;
					}
					case Conversation::Name::PostQuest:
					{
						break;
					}
				}
				break;
			}
			case Quest::Name::Dialogue:
			{
				switch (aName)
				{
					case Conversation::Name::Hello:
					{
						break;
					}
					case Conversation::Name::NoProgress:
					{
						break;
					}
					case Conversation::Name::Completed:
					{
						break;
					}
					case Conversation::Name::PostQuest:
					{
						break;
					}
				}
				break;
			}
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
  Quest::Quest(Quest::Name aName)
    : mName(aName), mState(Quest::State::Available), mConditionMet(false)
  {
		switch (aName)
		{
			case Quest::Name::Introduction:
			{
				mConversationVec.emplace_back(Conversation::Name::Hello, Quest::Name::Introduction);
				mConversationVec.emplace_back(Conversation::Name::PostQuest, Quest::Name::Introduction);
				break;
			}
			case Quest::Name::Fetch:
			{

				break;
			}
			case Quest::Name::Explore:
			{

				break;
			}
			case Quest::Name::Dialogue:
			{

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

		mQuestVec.emplace_back(Quest::Name::Introduction);
		
		mActiveQuest = &mQuestVec[0];
		mActiveConvo = &mActiveQuest->GetConversations()->at(0);
		mActiveNode = mActiveConvo->GetRoot();
  }

  void JohnDialogue::RegisterJohn(CollisionStarted *aEvent)
  {
		if (aEvent->OtherObject->GetComponent<BoatController>() != nullptr)
		{
			mSpace->YTERegister(Events::DialogueStart, this, &JohnDialogue::OnDialogueStart);
			mSpace->YTERegister(Events::DialogueNodeConfirm, this, &JohnDialogue::OnDialogueContinue);
			mSpace->YTERegister(Events::DialogueExit, this, &JohnDialogue::OnDialogueExit);
		}
  }

  void JohnDialogue::DeregisterJohn(CollisionEnded *aEvent)
  {
		if (aEvent->OtherObject->GetComponent<BoatController>() != nullptr)
		{
			mSpace->YTEDeregister(Events::DialogueStart, this, &JohnDialogue::OnDialogueStart);
			mSpace->YTEDeregister(Events::DialogueNodeConfirm, this, &JohnDialogue::OnDialogueContinue);
			mSpace->YTEDeregister(Events::DialogueExit, this, &JohnDialogue::OnDialogueExit);
		}
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
			while (type == DialogueNode::NodeType::Anim || type == DialogueNode::NodeType::Sound)
			{
					mActiveNode->ActivateNode();
					mActiveNode = mActiveNode->GetChild(aEvent->Selection);
					if (mActiveNode != nullptr)
					{
						type = mActiveNode->GetNodeType();
					}
					else
					{
						DialogueExit diagExit;
						mSpace->SendEvent(Events::DialogueExit, &diagExit);
						mActiveConvo->SetState(Conversation::State::Completed);
						return;
					}
			}
			// For input and text we rely on the director responding
			if (type == DialogueNode::NodeType::Input || type == DialogueNode::NodeType::Text)
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