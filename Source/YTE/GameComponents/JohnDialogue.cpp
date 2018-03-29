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
  YTEDefineEvent(NoticeBoardHookup);
  YTEDefineEvent(QuestStart);
  YTEDefineEvent(UpdateActiveQuestState);

  YTEDefineType(NoticeBoardHookup) { YTERegisterType(NoticeBoardHookup); }
  YTEDefineType(QuestStart) { YTERegisterType(QuestStart); }
  YTEDefineType(UpdateActiveQuestState) { YTERegisterType(UpdateActiveQuestState); }

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
            // LEVEL ROOT
            DialogueData(dataR0, "John: What are you still doing here!", "John: I still need to cook the bowls and clean the food", "John: Please hurry!");
            mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataR0, 0);
            /*
            R0
            */
            enum { R0 };
            mNodeVec[R0].SetChildren(DialogueNodeChildren{});
            break;
          }
          case Conversation::Name::Completed:
          {
            // LEVEL C
            DialogueData(dataC0, "John : Just in time too.", "John : You really saved my tail feathers lamb");
            mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataC0, 0);

            // LEVEL B
            DialogueData(dataB0, AnimationNames::Idle);
            mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataB0, 0);

            // LEVEL A
            DialogueData(dataA0, "John : Oh thank goodness!");
            mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataA0, 0);

            // LEVEL ROOT
            DialogueData(dataR0, AnimationNames::Happy);
            mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataR0, 0);
            /*
            C0 - B0 - A0 - R0
            */
            enum { C0, B0, A0, R0 };
            mNodeVec[C0].SetChildren(DialogueNodeChildren{});
            mNodeVec[B0].SetChildren(DialogueNodeChildren{ &mNodeVec[C0] });
            mNodeVec[A0].SetChildren(DialogueNodeChildren{ &mNodeVec[B0] });
            mNodeVec[R0].SetChildren(DialogueNodeChildren{ &mNodeVec[A0] });
            break;
          }
          case Conversation::Name::PostQuest:
          {
            // LEVEL ROOT
            DialogueData(dataR0, "John: Thanks again", "John: Next time lunch is on the house!");
            mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataR0, 0);
            /*
            R0
            */
            enum { R0 };
            mNodeVec[R0].SetChildren(DialogueNodeChildren{});
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
            //@@@(JAY): Test Text Nodes that have more than 3 lines
            // LEVEL I
            DialogueData(dataI0, "John: In my dream there was a rock formation", "John: And it looked like a fruit!", "John: Go see if you can find this rock formation", "John: That'll jog my memory for the dish");
            mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataI0, 0);

            // LEVEL H
            DialogueData(dataH0, AnimationNames::Idle);
            mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataH0, 0);

            // LEVEL G
            DialogueData(dataG0, "John: But I've forgotten the primary ingredient!");
            mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataG0, 0);

            // LEVEL F
            DialogueData(dataF0, AnimationNames::Sad);
            mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataF0, 0);

            // LEVEL E
            DialogueData(dataE0, "John: I had a dream of a new dish");
            mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataE0, 0);

            // LEVEL D
            DialogueData(dataD0, AnimationNames::Idle);
            mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataD0, 0);

            // LEVEL C
            DialogueData(dataC0, "What's on the menu?");
            mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataC0, 0);

            // LEVEL B
            DialogueData(dataB0, "John: NO.");
            mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataB0, 0);

            // LEVEL A
            DialogueData(dataA0, AnimationNames::Angry);
            mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataA0, 0);

            // LEVEL ROOT
            DialogueData(dataR0, "John: Figs?...", "John: No.", "John: Dates?...");
            mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataR0, 0);

            /*
            I0 - H0 - G0 - F0 - E0 - D0 - C0 - B0 - A0 - R0
            */
            enum { I0, H0, G0, F0, E0, D0, C0, B0, A0, R0 };
            mNodeVec[I0].SetChildren(DialogueNodeChildren{});
            mNodeVec[H0].SetChildren(DialogueNodeChildren{ &mNodeVec[I0] });
            mNodeVec[G0].SetChildren(DialogueNodeChildren{ &mNodeVec[H0] });
            mNodeVec[F0].SetChildren(DialogueNodeChildren{ &mNodeVec[G0] });
            mNodeVec[E0].SetChildren(DialogueNodeChildren{ &mNodeVec[F0] });
            mNodeVec[D0].SetChildren(DialogueNodeChildren{ &mNodeVec[E0] });
            mNodeVec[C0].SetChildren(DialogueNodeChildren{ &mNodeVec[D0] });
            mNodeVec[B0].SetChildren(DialogueNodeChildren{ &mNodeVec[C0] });
            mNodeVec[A0].SetChildren(DialogueNodeChildren{ &mNodeVec[B0] });
            mNodeVec[R0].SetChildren(DialogueNodeChildren{ &mNodeVec[A0] });
            break;
          }
          case Conversation::Name::NoProgress:
          {
            // LEVEL ROOT
            DialogueData(dataR0, "John: Pear?", "John: ...", "John: Hahaha, ridiculous!");
            mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataR0, 0);
            /*
            R0
            */
            enum { R0 };
            mNodeVec[R0].SetChildren(DialogueNodeChildren{});
            break;
          }
          case Conversation::Name::Completed:
          {
            // LEVEL A
            DialogueData(dataA0, "John: OF COURSE!", "John: It was so simple", "John: It's all coming back to me!", "John: Where's my knife?");
            mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataA0, 0);

            // @@@(JAY): Test convos that start with an input node
            // LEVEL ROOT
            DialogueData(dataR0, "Found it! It was an apple!");
            mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataR0, 0);
            /*
            A0 - R0
            */
            enum { A0, R0 };
            mNodeVec[A0].SetChildren(DialogueNodeChildren{});
            mNodeVec[R0].SetChildren(DialogueNodeChildren{ &mNodeVec[A0] });
            break;
          }
          case Conversation::Name::PostQuest:
          {
            // LEVEL ROOT
            DialogueData(dataR0, "John: Thank you for your help", "John: But I have tons of prep work to do now");
            mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataR0, 0);
            /*
            R0
            */
            enum { R0 };
            mNodeVec[R0].SetChildren(DialogueNodeChildren{});
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
            // LEVEL B
            DialogueData(dataB0, "I'm on it!");
            mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataB0, 0);

            // LEVEL A
            DialogueData(dataA0, "John: What to do", "John: Lamb, thank cod!", "John: I have a big review with THE Basil Bouillon", "John: But I don't know what to make", "John: Go talk to him and see if you can't figure out his favorite food");
            mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataA0, 0);

            // LEVEL ROOT
            DialogueData(dataR0, "What's cooking?");
            mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataR0, 0);
            /*
            B0 - A0 - R0
            */
            enum { B0, A0, R0 };
            mNodeVec[B0].SetChildren(DialogueNodeChildren{ });
            mNodeVec[A0].SetChildren(DialogueNodeChildren{ &mNodeVec[B0] });
            mNodeVec[R0].SetChildren(DialogueNodeChildren{ &mNodeVec[A0] });
            break;
          }
          case Conversation::Name::NoProgress:
          {
            // LEVEL ROOT
            DialogueData(dataR0, "John: But please, be discrete.", "John: I don't want him thinking I care TOO much.");
            mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataR0, 0);
            /*
            R0
            */
            enum { R0 };
            mNodeVec[R0].SetChildren(DialogueNodeChildren{ });
            break;
          }
          case Conversation::Name::Completed:
          {
            // LEVEL A0
            DialogueData(dataA0, "John: Anything with tomato?", "John: Hmm...I didn't expect this", "John: Well thank you for your help!");
            mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataA0, 0);

            // LEVEL ROOT
            DialogueData(dataR0, "Tomato!");
            mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataR0, 0);
            /*
            A0 - R0
            */
            enum { A0, R0 };
            mNodeVec[A0].SetChildren(DialogueNodeChildren{});
            mNodeVec[R0].SetChildren(DialogueNodeChildren{ &mNodeVec[A0] });
            break;
          }
          case Conversation::Name::PostQuest:
          {
            // LEVEL B0
            DialogueData(dataB0, "John: That old fish Jiro will be so surprised");
            mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataB0, 0);

            // LEVEL A0
            DialogueData(dataA0, AnimationNames::Happy);
            mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataA0, 0);

            // LEVEL ROOT
            DialogueData(dataR0, "John: This will get me my third star for sure!");
            mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataR0, 0);
            /*
            B0 - A0 - R0
            */
            enum { B0, A0, R0 };
            mNodeVec[B0].SetChildren(DialogueNodeChildren{});
            mNodeVec[A0].SetChildren(DialogueNodeChildren{ &mNodeVec[B0] });
            mNodeVec[R0].SetChildren(DialogueNodeChildren{ &mNodeVec[A0] });
            break;
          }
        }
        break;
      }
      case Quest::Name::NotActive:
      {
          // Since we know NotActive quests will only have a Hello Conversation
        // LEVEL E0
        DialogueData(dataE0, "John: See you next time.");
        mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataE0, 0);

        // LEVEL D0
        DialogueData(dataD0, AnimationNames::WaveInit);
        mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataD0, 0);

        // LEVEL C0
        DialogueData(dataC0, "John: You just missed the lunch rush.", "John: Thanks for coming out though!");
        mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataC0, 0);

        // LEVEL B0
        DialogueData(dataB0, AnimationNames::Idle);
        mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataB0, 0);

        // LEVEL A0
        DialogueData(dataA0, "John: Welcome back lamb!");
        mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataA0, 0);

        // LEVEL ROOT
        DialogueData(dataR0, AnimationNames::Happy);
        mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataR0, 0);
        /*
        E0 - D0 - C0 - B0 - A0 - R0
        */
        enum { E0, D0, C0, B0, A0, R0 };
        mNodeVec[E0].SetChildren(DialogueNodeChildren{});
        mNodeVec[D0].SetChildren(DialogueNodeChildren{ &mNodeVec[E0] });
        mNodeVec[C0].SetChildren(DialogueNodeChildren{ &mNodeVec[D0] });
        mNodeVec[B0].SetChildren(DialogueNodeChildren{ &mNodeVec[C0] });
        mNodeVec[A0].SetChildren(DialogueNodeChildren{ &mNodeVec[B0] });
        mNodeVec[R0].SetChildren(DialogueNodeChildren{ &mNodeVec[A0] });
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
    : mName(aName), mState(Quest::State::NotActive), mCharacter(CharacterName::John)
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
        mConversationVec.emplace_back(Conversation::Name::Hello, Quest::Name::Fetch);
        mConversationVec.emplace_back(Conversation::Name::NoProgress, Quest::Name::Fetch);
        mConversationVec.emplace_back(Conversation::Name::Completed, Quest::Name::Fetch);
        mConversationVec.emplace_back(Conversation::Name::PostQuest, Quest::Name::Fetch);
        break;
      }
      case Quest::Name::Explore:
      {
        mConversationVec.emplace_back(Conversation::Name::Hello, Quest::Name::Explore);
        mConversationVec.emplace_back(Conversation::Name::NoProgress, Quest::Name::Explore);
        mConversationVec.emplace_back(Conversation::Name::Completed, Quest::Name::Explore);
        mConversationVec.emplace_back(Conversation::Name::PostQuest, Quest::Name::Explore);
        break;
      }
      case Quest::Name::Dialogue:
      {
        mConversationVec.emplace_back(Conversation::Name::Hello, Quest::Name::Dialogue);
        mConversationVec.emplace_back(Conversation::Name::NoProgress, Quest::Name::Dialogue);
        mConversationVec.emplace_back(Conversation::Name::Completed, Quest::Name::Dialogue);
        mConversationVec.emplace_back(Conversation::Name::PostQuest, Quest::Name::Dialogue);
        break;
      }
      case Quest::Name::NotActive:
      {
        mConversationVec.emplace_back(Conversation::Name::Hello, Quest::Name::NotActive);
      }
    }
  }

/******************************************************************************
  JohnDialogue Component
******************************************************************************/
  JohnDialogue::JohnDialogue(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    YTEUnusedArgument(aProperties);

    mQuestVec.emplace_back(Quest::Name::Introduction);
    mQuestVec.emplace_back(Quest::Name::Fetch);
    mQuestVec.emplace_back(Quest::Name::Explore);
    mQuestVec.emplace_back(Quest::Name::Dialogue);
    mQuestVec.emplace_back(Quest::Name::NotActive);
    
    mActiveQuest = &mQuestVec[(int)Quest::Name::NotActive];
    mActiveConvo = &mActiveQuest->GetConversations()->at(0);
    mActiveNode = mActiveConvo->GetRoot();
  }

  void JohnDialogue::Initialize()
  {
    mOwner->YTERegister(Events::CollisionStarted, this, &JohnDialogue::RegisterJohn);
    mOwner->YTERegister(Events::CollisionEnded, this, &JohnDialogue::DeregisterJohn);
    mSpace->YTERegister(Events::QuestStart, this, &JohnDialogue::OnQuestStart);
    mSpace->YTERegister(Events::UpdateActiveQuestState, this, &JohnDialogue::OnUpdateActiveQuestState);
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

  void JohnDialogue::Start()
  {
      // Send to the space a ptr to the activequest for the noticeboard
    NoticeBoardHookup firstQuest(&mActiveQuest);
    mSpace->SendEvent(Events::NoticeBoardHookup, &firstQuest);
  }

  void JohnDialogue::OnDialogueStart(DialogueStart *aEvent)
  {
    YTEUnusedArgument(aEvent);

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
    if (aEvent->isEarlyExit)
    {
      mActiveConvo->SetState(Conversation::State::EarlyExit);
    }
    else
    {
      if (mActiveQuest->GetState() == Quest::State::Received)
      {
        mActiveConvo->SetState(Conversation::State::Completed);
        UpdateActiveQuestState briefed(mName, Quest::State::Briefed);
        mSpace->SendEvent(Events::UpdateActiveQuestState, &briefed);
      }
      if (mActiveQuest->GetState() == Quest::State::Completed)
      {
        mActiveConvo->SetState(Conversation::State::Completed);
        UpdateActiveQuestState completed(mName, Quest::State::Completed);
        mSpace->SendEvent(Events::UpdateActiveQuestState, &completed);
      }
    }
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
    }
  }

  void JohnDialogue::OnQuestStart(QuestStart *aEvent)
  {
    if (aEvent->mCharacter == mName)
    {
      mActiveQuest = &mQuestVec[(int)aEvent->mQuest];
    }
    else
    {
      mActiveQuest = &mQuestVec[(int)Quest::Name::NotActive];
    }
    UpdateActiveQuestState received(mName, Quest::State::Received);
    mSpace->SendEvent(Events::UpdateActiveQuestState, &received);
      
    mActiveConvo = &( *( mActiveQuest->GetConversations() ) )[(int)Conversation::Name::Hello];
    mActiveNode = mActiveConvo->GetRoot();
  }

  void JohnDialogue::OnUpdateActiveQuestState(UpdateActiveQuestState *aEvent)
  {
    if (aEvent->mCharacter == mName)
    {
      mActiveQuest->SetState(aEvent->mState);
      if (aEvent->mState == Quest::State::Briefed)
      {
        mActiveConvo = &( *( mActiveQuest->GetConversations() ) )[(int)Conversation::Name::NoProgress];
        mActiveNode = mActiveConvo->GetRoot();
      }
      if (aEvent->mState == Quest::State::Accomplished)
      {
        mActiveConvo = &( *( mActiveQuest->GetConversations() ) )[(int)Conversation::Name::Completed];
        mActiveNode = mActiveConvo->GetRoot();
      }
      if (aEvent->mState == Quest::State::Completed)
      {
        mActiveConvo = &( *( mActiveQuest->GetConversations() ) )[(int)Conversation::Name::PostQuest];
        mActiveNode = mActiveConvo->GetRoot();
      }
    }
  }

} //end yte