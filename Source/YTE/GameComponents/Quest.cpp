/******************************************************************************/
/*!
\file   Quest.cpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-04-04

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/Quest.hpp"

namespace YTE
{
  YTEDefineEvent(UpdateActiveQuestState);
  YTEDefineEvent(QuestStart);

  YTEDefineType(UpdateActiveQuestState) { YTERegisterType(UpdateActiveQuestState); }
  YTEDefineType(QuestStart) { YTERegisterType(QuestStart); }

  YTEDefineType(Quest) { YTERegisterType(Quest); }
  YTEDefineType(Conversation) { YTERegisterType(Conversation); }

  /******************************************************************************
  Conversation
  ******************************************************************************/
  Conversation::Conversation(Conversation::Name aName, Quest::Name aQuest, Quest::CharacterName aCharacter, Space *space)
    : mName(aName), mState(Conversation::State::Available), mSpace(space)
  {
    if (aCharacter == Quest::CharacterName::John)
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
              DialogueData(dataG0, "John: Nice to meet you, though.", "John: Hopefully, we bump into each other again!");
              mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataG0, 0, mSpace);

              // LEVEL F
              DialogueData(dataF0, AnimationNames::Idle);
              mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataF0, 0, mSpace);

              // LEVEL E
              DialogueData(dataE0, "John: ...but I'm all out now, sorry.");
              mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataE0, 0, mSpace);

              // LEVEL D
              DialogueData(dataD0, AnimationNames::Sad);
              mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataD0, 0, mSpace);

              // LEVEL C
              DialogueData(dataC0, "John: Today I was serving Gazpacho...");
              mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataC0, 0, mSpace);

              // LEVEL B
              DialogueData(dataB0, "Smells great!");
              mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataB0, 0, mSpace);

              // LEVEL A
              DialogueData(dataA0, "John: Welcome to my pop-up restaurant, Que Delicioso.");
              mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataA0, 0, mSpace);

              // LEVEL ROOT
              DialogueData(dataR0, AnimationNames::Wave);
              mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataR0, 0, mSpace);

              /*
              G0 - F0 - E0 - D0 - C0 - B0 - A0 - R0
              */
              enum { G0, F0, E0, D0, C0, B0, A0, R0 };
              mNodeVec[G0].SetChildren(DialogueNodeChildren{});
              mNodeVec[F0].SetChildren(DialogueNodeChildren{ &mNodeVec[G0] });
              mNodeVec[E0].SetChildren(DialogueNodeChildren{ &mNodeVec[F0] });
              mNodeVec[D0].SetChildren(DialogueNodeChildren{ &mNodeVec[E0] });
              mNodeVec[C0].SetChildren(DialogueNodeChildren{ &mNodeVec[D0] });
              mNodeVec[B0].SetChildren(DialogueNodeChildren{ &mNodeVec[C0] });
              mNodeVec[A0].SetChildren(DialogueNodeChildren{ &mNodeVec[B0] });
              mNodeVec[R0].SetChildren(DialogueNodeChildren{ &mNodeVec[A0] });
              break;
            }
            // Make this happen at the same time as the others
            case Conversation::Name::PostQuest:
            {
              // LEVEL A
              DialogueData(dataA0, "John: Have a great day!");
              mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataA0, 0, mSpace);

              // LEVEL ROOT
              DialogueData(dataR0, AnimationNames::Wave);
              mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataR0, 0, mSpace);
              /*
              A0 - R0
              */
              enum { A0, R0 };
              mNodeVec[A0].SetChildren(DialogueNodeChildren{});
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
              // LEVEL G
              DialogueData(dataG0, "John: I knew I could count on you!");
              mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataG0, 0, mSpace);
              DialogueData(dataG1, "John: Thanks!");
              mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataG1, 0, mSpace);

              //Level F
              DialogueData(dataF0, AnimationNames::Happy);
              mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataF0, 0, mSpace);
              DialogueData(dataF1, AnimationNames::Happy);
              mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataF1, 1, mSpace);

              // LEVEL E
              DialogueData(dataE0, "Your sous chef is on the job!", "Sure thing!");
              mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataE0, 0, mSpace);

              // LEVEL D
              DialogueData(dataD0, "John: Can you go out and look for them?");
              mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataD0, 0, mSpace);

              // LEVEL C
              DialogueData(dataC0, "Oh no!");// , "Your sous chef is on the job!", "Sure thing!");
              mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataC0, 0, mSpace);

              // LEVEL B
              DialogueData(dataB0, "John: ...but they haven't shown up yet.");
              mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataB0, 0, mSpace);

              // LEVEL A
              DialogueData(dataA0, AnimationNames::Angry);
              mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataA0, 0, mSpace);

              // LEVEL ROOT
              DialogueData(dataR0, "John: Perfect timing!", "John: I'm expecting a delivery of ingredients...");
              mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataR0, 0, mSpace);

              /*
              G0 - F0 - E0 - D0 - C0 - B0 - A0 - R0
              G1 - F1 -|
              */
              enum { G0, G1, F0, F1, E0, D0, C0, B0, A0, R0 };
              mNodeVec[G0].SetChildren(DialogueNodeChildren{});
              mNodeVec[G1].SetChildren(DialogueNodeChildren{});
              mNodeVec[F0].SetChildren(DialogueNodeChildren{ &mNodeVec[G0] });
              mNodeVec[F1].SetChildren(DialogueNodeChildren{ &mNodeVec[G1] });
              mNodeVec[E0].SetChildren(DialogueNodeChildren{ &mNodeVec[F0], &mNodeVec[F1] });
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
              DialogueData(dataR0, "John: What are you still doing here! ", "John: I still need to cook the bowls and clean the food! ", "John: Please hurry!");
              mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataR0, 0, mSpace);
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
              DialogueData(dataC0, "John : Just in time, too.", "John : You really saved my tail feathers, lamb!");
              mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataC0, 0, mSpace);

              // LEVEL B
              DialogueData(dataB0, AnimationNames::Idle);
              mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataB0, 0, mSpace);

              // LEVEL A
              DialogueData(dataA0, "John : Oh, thank goodness!");
              mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataA0, 0, mSpace);

              // LEVEL ROOT
              DialogueData(dataR0, AnimationNames::Happy);
              mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataR0, 0, mSpace);
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
              DialogueData(dataR0, "John: Thanks again.", "John: Next time, lunch is on the house!");
              mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataR0, 0, mSpace);
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
              DialogueData(dataI0, "John: In my dream, there was a rock formation.", "John: And it looked like a fruit!", "John: Go see if you can find this rock formation.", "John: That'll jog my memory for the dish!");
              mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataI0, 0, mSpace);

              // LEVEL H
              DialogueData(dataH0, AnimationNames::Idle);
              mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataH0, 0, mSpace);

              // LEVEL G
              DialogueData(dataG0, "John: ...but I've forgotten the primary ingredient!");
              mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataG0, 0, mSpace);

              // LEVEL F
              DialogueData(dataF0, AnimationNames::Sad);
              mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataF0, 0, mSpace);

              // LEVEL E
              DialogueData(dataE0, "John: I had a dream of a new dish...");
              mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataE0, 0, mSpace);

              // LEVEL D
              DialogueData(dataD0, AnimationNames::Idle);
              mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataD0, 0, mSpace);

              // LEVEL C
              DialogueData(dataC0, "What's on the menu?");
              mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataC0, 0, mSpace);

              // LEVEL B
              DialogueData(dataB0, "John: NO.");
              mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataB0, 0, mSpace);

              // LEVEL A
              DialogueData(dataA0, AnimationNames::Angry);
              mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataA0, 0, mSpace);

              // LEVEL ROOT
              DialogueData(dataR0, "John: Figs?...", "John: No.", "John: Dates?...");
              mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataR0, 0, mSpace);

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
              mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataR0, 0, mSpace);
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
              DialogueData(dataA0, "John: OF COURSE!", "John: It was so simple!", "John: It's all coming back to me!", "John: Where's my knife?");
              mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataA0, 0, mSpace);

              // @@@(JAY): Test convos that start with an input node
              // LEVEL ROOT
              DialogueData(dataR0, "Found it! It was an apple!");
              mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataR0, 0, mSpace);
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
              DialogueData(dataR0, "John: Thank you for your help.", "John: I have tons of prep work to do now!");
              mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataR0, 0, mSpace);
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
              mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataB0, 0, mSpace);

              // LEVEL A
              DialogueData(dataA0, "John: What to do...", "John: Lamb, thank cod!", "John: I have a big review with THE Basil Bouillon...", "John: ...but I don't know what to make!", "John: Go talk to him and see if you can figure out his favorite food.");
              mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataA0, 0, mSpace);

              // LEVEL ROOT
              DialogueData(dataR0, "What's cooking?");
              mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataR0, 0, mSpace);
              /*
              B0 - A0 - R0
              */
              enum { B0, A0, R0 };
              mNodeVec[B0].SetChildren(DialogueNodeChildren{});
              mNodeVec[A0].SetChildren(DialogueNodeChildren{ &mNodeVec[B0] });
              mNodeVec[R0].SetChildren(DialogueNodeChildren{ &mNodeVec[A0] });
              break;
            }
            case Conversation::Name::NoProgress:
            {
              // LEVEL ROOT
              DialogueData(dataR0, "John: But please, be discrete.", "John: I don't want him thinking I care TOO much.");
              mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataR0, 0, mSpace);
              /*
              R0
              */
              enum { R0 };
              mNodeVec[R0].SetChildren(DialogueNodeChildren{});
              break;
            }
            case Conversation::Name::Completed:
            {
              // LEVEL A0
              DialogueData(dataA0, "John: Anything with tomato?", "John: Hmm...I didn't expect this.", "John: Well, thank you for your help!");
              mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataA0, 0, mSpace);

              // LEVEL ROOT
              DialogueData(dataR0, "Tomato!");
              mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataR0, 0, mSpace);
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
              DialogueData(dataB0, "John: That old fish Jiro will be so surprised!");
              mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataB0, 0, mSpace);

              // LEVEL A0
              DialogueData(dataA0, AnimationNames::Happy);
              mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataA0, 0, mSpace);

              // LEVEL ROOT
              DialogueData(dataR0, "John: This will get me my third star for sure!");
              mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataR0, 0, mSpace);
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
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataE0, 0, mSpace);

          // LEVEL D0
          DialogueData(dataD0, AnimationNames::Wave);
          mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataD0, 0, mSpace);

          // LEVEL C0
          DialogueData(dataC0, "John: You just missed the lunch rush.", "John: Thanks for coming out though!");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataC0, 0, mSpace);

          // LEVEL B0
          DialogueData(dataB0, AnimationNames::Idle);
          mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataB0, 0, mSpace);

          // LEVEL A0
          DialogueData(dataA0, "John: Welcome back lamb!");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataA0, 0, mSpace);

          // LEVEL ROOT
          DialogueData(dataR0, AnimationNames::Happy);
          mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataR0, 0, mSpace);
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
    else if (aCharacter == Quest::CharacterName::Daisy)
    {
      switch (aQuest)
      {
      case Quest::Name::Introduction:
      {
        switch (aName)
        {
        case Conversation::Name::Hello:
        {
          // LEVEL C
          DialogueData(dataC0, "Daisy: I'm Daisy, the farmer 'round these parts.", "Daisy: Pleasure meeting you, lamb!");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataC0, 0, mSpace);

          // LEVEL B
          DialogueData(dataB0, "Howdy! It sure is.");
          mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataB0, 0, mSpace);

          // LEVEL A
          DialogueData(dataA0, "Daisy: Hidley Ho!", "Daisy: Beautiful day today.");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataA0, 0, mSpace);

          // LEVEL ROOT
          DialogueData(dataR0, AnimationNames::Wave);
          mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataR0, 0, mSpace);

          /*
          C0 - B0 - A0 - R0
          */
          enum { C0, B0, A0, R0 };
          mNodeVec[C0].SetChildren(DialogueNodeChildren{ });
          mNodeVec[B0].SetChildren(DialogueNodeChildren{ &mNodeVec[C0] });
          mNodeVec[A0].SetChildren(DialogueNodeChildren{ &mNodeVec[B0] });
          mNodeVec[R0].SetChildren(DialogueNodeChildren{ &mNodeVec[A0] });
          break;
        }
        case Conversation::Name::PostQuest:
        {
          // LEVEL C
          DialogueData(dataC0, "Daisy: Betcha didn't know that!");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataC0, 0, mSpace);

          // LEVEL B
          DialogueData(dataB0, AnimationNames::Happy);
          mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataB0, 0, mSpace);

          // LEVEL A
          DialogueData(dataA0, "Daisy: Good season for crops.");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataA0, 0, mSpace);

          // LEVEL ROOT
          DialogueData(dataR0, AnimationNames::Wave);
          mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataR0, 0, mSpace);
          /*
          C0 - B0 - A0 - R0
          */
          enum { C0, B0, A0, R0 };
          mNodeVec[C0].SetChildren(DialogueNodeChildren{ });
          mNodeVec[B0].SetChildren(DialogueNodeChildren{ &mNodeVec[C0] });
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
          // LEVEL I
          DialogueData(dataI0, "Daisy: Was that a pun?");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataI0, 0, mSpace);

          // LEVEL H
          DialogueData(dataH0, "Without a sprout!");
          mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataH0, 0, mSpace);

          // LEVEL G
          DialogueData(dataG0, "Daisy: Boy, all that fun made me forget!", "Daisy: Could you go and wrangle up some seeds for me?");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataG0, 0, mSpace);

          // LEVEL F
          DialogueData(dataF0, "Sure was; anything you need Daisy?");
          mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataF0, 0, mSpace);

          // LEVEL E
          DialogueData(dataE0, "Daisy: Wasn't that fun?");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataE0, 0, mSpace);

          // LEVEL D
          DialogueData(dataD0, "Daisy: That's it, catch the wind!");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataD0, 0, mSpace);
          DialogueData(dataD1, "Daisy: Gotta cut loose when you can!");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataD1, 1, mSpace);
          DialogueData(dataD2, "Daisy: Pull that thing!", "Daisy: Pull that other thing!", "Daisy: Sail faster!");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataD2, 2, mSpace);

          // LEVEL C
          DialogueData(dataC0, "TIGHTEN THE JIB!", "SUN OVER THE YARDARM!", "BRACE THE TILLER!");
          mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataC0, 0, mSpace);

          // LEVEL B
          DialogueData(dataB0, "Daisy: TOP SOIIIIIIL!!!", "Daisy: Your turn:");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataB0, 0, mSpace);

          // LEVEL A
          DialogueData(dataA0, AnimationNames::Happy);
          mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataA0, 0, mSpace);

          // LEVEL ROOT
          DialogueData(dataR0, "Daisy: Sun's out, fun shout:");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataR0, 0, mSpace);

          /*
          I0 - H0 - G0 - F0 - E0 - D0 - C0 - B0 - A0 - R0
                                |- D1 -|
                                |- D2 -|
          */
          enum { I0, H0, G0, F0, E0, D0, D1, D2, C0, B0, A0, R0 };
          mNodeVec[I0].SetChildren(DialogueNodeChildren{ });
          mNodeVec[H0].SetChildren(DialogueNodeChildren{ &mNodeVec[I0] });
          mNodeVec[G0].SetChildren(DialogueNodeChildren{ &mNodeVec[H0] });
          mNodeVec[F0].SetChildren(DialogueNodeChildren{ &mNodeVec[G0] });
          mNodeVec[E0].SetChildren(DialogueNodeChildren{ &mNodeVec[F0] });
          mNodeVec[D0].SetChildren(DialogueNodeChildren{ &mNodeVec[E0] });
          mNodeVec[D1].SetChildren(DialogueNodeChildren{ &mNodeVec[E0] });
          mNodeVec[D2].SetChildren(DialogueNodeChildren{ &mNodeVec[E0] });
          mNodeVec[C0].SetChildren(DialogueNodeChildren{ &mNodeVec[D0], &mNodeVec[D1], &mNodeVec[D2] });
          mNodeVec[B0].SetChildren(DialogueNodeChildren{ &mNodeVec[C0] });
          mNodeVec[A0].SetChildren(DialogueNodeChildren{ &mNodeVec[B0] });
          mNodeVec[R0].SetChildren(DialogueNodeChildren{ &mNodeVec[A0] });
          break;
        }
        case Conversation::Name::NoProgress:
        {
          // LEVEL ROOT
          DialogueData(dataR0, "Daisy: How's the seed collecting going?", "Daisy: Well, I'm gonna need a few more than that!");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataR0, 0, mSpace);
          /*
          R0
          */
          enum { R0 };
          mNodeVec[R0].SetChildren(DialogueNodeChildren{});
          break;
        }
        case Conversation::Name::Completed:
        {
          // LEVEL D
          DialogueData(dataD0, "Daisy: Far and away the best prize that life has to offer,", "Daisy: Is a chance to work hard at work worth doing.", "Daisy: Ted the Bear said that.", "Daisy: Plus, there's lemonade!");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataD0, 0, mSpace);

          // LEVEL C
          DialogueData(dataC0, "That's fun?");
          mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataC0, 0, mSpace);

          // LEVEL B
          DialogueData(dataB0, AnimationNames::Idle);
          mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataB0, 0, mSpace);

          // LEVEL A
          DialogueData(dataA0, "Daisy: Now for the fun part!", "Daisy: Hours in hot sun tilling soil!");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataA0, 0, mSpace);

          // LEVEL ROOT
          DialogueData(dataR0, AnimationNames::Happy);
          mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataR0, 0, mSpace);
          /*
          C0 - B0 - A0 - R0
          */
          enum { D0, C0, B0, A0, R0 };
          mNodeVec[D0].SetChildren(DialogueNodeChildren{ });
          mNodeVec[C0].SetChildren(DialogueNodeChildren{ &mNodeVec[D0] });
          mNodeVec[B0].SetChildren(DialogueNodeChildren{ &mNodeVec[C0] });
          mNodeVec[A0].SetChildren(DialogueNodeChildren{ &mNodeVec[B0] });
          mNodeVec[R0].SetChildren(DialogueNodeChildren{ &mNodeVec[A0] });
          break;
        }
        case Conversation::Name::PostQuest:
        {
          // LEVEL A
          DialogueData(dataA0, "Daisy: Yeah! Good to see ya.");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataA0, 0, mSpace);

          // LEVEL ROOT
          DialogueData(dataR0, "See ya next time Daisy!");
          mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataR0, 0, mSpace);
          /*
          A0 - R0
          */
          enum { A0, R0 };
          mNodeVec[A0].SetChildren(DialogueNodeChildren{ });
          mNodeVec[R0].SetChildren(DialogueNodeChildren{ &mNodeVec[A0] });
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
          // LEVEL B
          DialogueData(dataB0, "Daisy: I'm looking for a large, flat, open area.");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataB0, 0, mSpace);

          // LEVEL A
          DialogueData(dataA0, "Sure thing!");
          mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataA0, 0, mSpace);

          // LEVEL ROOT
          DialogueData(dataR0, "Daisy: I'm on the hunt for a new plot for a community garden.", "Daisy: Could you go out and see if you can find one?");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataR0, 0, mSpace);

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
          DialogueData(dataR0, "Daisy: Any luck?");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataR0, 0, mSpace);
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
          DialogueData(dataC0, "Daisy: Oh, this will work great!");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataC0, 0, mSpace);

          // LEVEL B
          DialogueData(dataB0, AnimationNames::Happy);
          mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataB0, 0, mSpace);

          // LEVEL A
          DialogueData(dataA0, "Daisy: Perfect! Let's see...");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataA0, 0, mSpace);

          // @@@(JAY): Test convos that start with an input node
          // LEVEL ROOT
          DialogueData(dataR0, "I found just the spot!");
          mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataR0, 0, mSpace);
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
          DialogueData(dataR0, "Daisy: Be sure to stop by when you find some time off work!");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataR0, 0, mSpace);
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
          // LEVEL C
          DialogueData(dataC0, "Daisy: Thank you kindly!");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataC0, 0, mSpace);

          // LEVEL B
          DialogueData(dataB0, AnimationNames::Happy);
          mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataB0, 0, mSpace);

          // LEVEL A
          DialogueData(dataA0, "You got it!");
          mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataA0, 0, mSpace);

          // LEVEL ROOT
          DialogueData(dataR0, "Daisy: Could you ask Chef John what ingredients he wants for next month?");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataR0, 0, mSpace);
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
        case Conversation::Name::NoProgress:
        {
          // LEVEL ROOT
          DialogueData(dataR0, "Daisy: What did John say?", "Daisy: ...well, besides cayenne.");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataR0, 0, mSpace);
          /*
          R0
          */
          enum { R0 };
          mNodeVec[R0].SetChildren(DialogueNodeChildren{});
          break;
        }
        case Conversation::Name::Completed:
        {
          // LEVEL A0
          DialogueData(dataA0, "Daisy: Must be another dish for Basil.", "Daisy: Not a problem. Thanks for helping, lamb!");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataA0, 0, mSpace);

          // LEVEL ROOT
          DialogueData(dataR0, "Tomato is top priority!");
          mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataR0, 0, mSpace);
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
          DialogueData(dataR0, "Daisy: See you around!");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataR0, 0, mSpace);
          /*
          R0
          */
          enum { R0 };
          mNodeVec[R0].SetChildren(DialogueNodeChildren{ });
          break;
        }
        }
        break;
      }
      case Quest::Name::NotActive:
      {
        // Since we know NotActive quests will only have a Hello Conversation
        // LEVEL B
        DialogueData(dataB0, "Daisy: Can't harvest too early, or you'll kill yer yield.");
        mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataB0, 0, mSpace);

        // LEVEL A
        DialogueData(dataA0, AnimationNames::Sad);
        mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataA0, 0, mSpace);

        // LEVEL ROOT
        DialogueData(dataR0, "Daisy: Top of the morning!");
        mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataR0, 0, mSpace);
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
    }
    else if (aCharacter == Quest::CharacterName::Basil)
    {
      switch (aQuest)
      {
      case Quest::Name::Introduction:
      {
        switch (aName)
        {
        case Conversation::Name::Hello:
        {
          // LEVEL F
          DialogueData(dataF0, "Basil: Unquestionably.", "Basil: Now, get going young one; I have a four course meal waiting for me.");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataF0, 0, mSpace);

          // LEVEL E
          DialogueData(dataE0, "Nice to meet you.");
          mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataE0, 0, mSpace);

          // LEVEL D
          DialogueData(dataD0, AnimationNames::Idle);
          mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataD0, 0, mSpace);

          // LEVEL C
          DialogueData(dataC0, "Basil: A FOOD FANATIC!");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataC0, 0, mSpace);

          // LEVEL B
          DialogueData(dataB0, AnimationNames::Angry);
          mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataB0, 0, mSpace);

          // LEVEL A
          DialogueData(dataA0, "Basil: Well hello there, little lamb.", "Basil: You are of course speaking with...", "Basil: The Great Basil Bouillon!", "Basil: A culinary connoisseur,", "Basil: A dietary director,");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataA0, 0, mSpace);

          // LEVEL ROOT
          DialogueData(dataR0, AnimationNames::Wave);
          mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataR0, 0, mSpace);

          /*
          F0 - E0 - D0 - C0 - B0 - A0 - R0
          */
          enum { F0, E0, D0, C0, B0, A0, R0 };
          mNodeVec[F0].SetChildren(DialogueNodeChildren{ });
          mNodeVec[E0].SetChildren(DialogueNodeChildren{ &mNodeVec[F0] });
          mNodeVec[D0].SetChildren(DialogueNodeChildren{ &mNodeVec[E0] });
          mNodeVec[C0].SetChildren(DialogueNodeChildren{ &mNodeVec[D0] });
          mNodeVec[B0].SetChildren(DialogueNodeChildren{ &mNodeVec[C0] });
          mNodeVec[A0].SetChildren(DialogueNodeChildren{ &mNodeVec[B0] });
          mNodeVec[R0].SetChildren(DialogueNodeChildren{ &mNodeVec[A0] });
          break;
        }
        case Conversation::Name::PostQuest:
        {
          // LEVEL A
          DialogueData(dataA0, "Basil: No, I won't share with you.", "Basil: Your palette is surely not refined enough to appreciate it.");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataA0, 0, mSpace);

          // LEVEL ROOT
          DialogueData(dataR0, AnimationNames::Wave);
          mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataR0, 0, mSpace);
          /*
          A0 - R0
          */
          enum { A0, R0 };
          mNodeVec[A0].SetChildren(DialogueNodeChildren{});
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
          // LEVEL C
          DialogueData(dataC0, "Basil: Yes, a bib, you think I'd risk getting sauce on this suit?");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataC0, 0, mSpace);
          DialogueData(dataC1, "Basil: My thoughts precisely!");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataC1, 0, mSpace);

          // LEVEL B
          DialogueData(dataB0, AnimationNames::Angry);
          mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataB0, 0, mSpace);
          DialogueData(dataB1, AnimationNames::Happy);
          mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataB1, 1, mSpace);

          // LEVEL A
          DialogueData(dataA0, "A bib?", "Sure thing, can't have your suit getting dirty.");
          mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataA0, 0, mSpace);

          // LEVEL ROOT
          DialogueData(dataR0, "Basil: Young lamb!", "Basil: The pleasure is yours to see me again.", "Basil: Be a lamb and acquire my bib. ", "Basil: I seem to have dropped it somewhere nearby.");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataR0, 0, mSpace);

          /*
          C0 - B0 - A0 - R0
          C1 - B1 -|
          */
          enum { C0, C1, B0, B1, A0, R0 };
          mNodeVec[C0].SetChildren(DialogueNodeChildren{ });
          mNodeVec[C1].SetChildren(DialogueNodeChildren{ });
          mNodeVec[B0].SetChildren(DialogueNodeChildren{ &mNodeVec[C0] });
          mNodeVec[B1].SetChildren(DialogueNodeChildren{ &mNodeVec[C1] });
          mNodeVec[A0].SetChildren(DialogueNodeChildren{ &mNodeVec[B0], &mNodeVec[B1] });
          mNodeVec[R0].SetChildren(DialogueNodeChildren{ &mNodeVec[A0] });
          break;
        }
        case Conversation::Name::NoProgress:
        {
          // LEVEL ROOT
          DialogueData(dataR0, "Basil: I suppose I could use your sail as a replacement...");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataR0, 0, mSpace);
          /*
          R0
          */
          enum { R0 };
          mNodeVec[R0].SetChildren(DialogueNodeChildren{});
          break;
        }
        case Conversation::Name::Completed:
        {
          // LEVEL D
          DialogueData(dataD0, "Basil: My sincerest gratitude, lamb.");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataD0, 0, mSpace);

          // LEVEL C
          DialogueData(dataC0, "Basil: It's a new genre I call Messy-Delicious.");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataC0, 0, mSpace);
          DialogueData(dataC1, "Basil: Well, I...You...", "Basil: You're allergic! Yeah, that's it.");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataC1, 1, mSpace);

          // LEVEL B
          DialogueData(dataB0, "Must be a really messy dish huh?", "Can I have some?");
          mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataB0, 0, mSpace);

          // LEVEL A
          DialogueData(dataA0, "Basil: Scrumcious, now I can have my meal without worry.");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataA0, 0, mSpace);

          // LEVEL ROOT
          DialogueData(dataR0, AnimationNames::Happy);
          mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataR0, 0, mSpace);
          /*
          D0 - C0 - B0 - A0 - R0
            |- C1 -|
          */
          enum { D0, C0, C1, B0, A0, R0 };
          mNodeVec[D0].SetChildren(DialogueNodeChildren{ });
          mNodeVec[C0].SetChildren(DialogueNodeChildren{ &mNodeVec[D0] });
          mNodeVec[C1].SetChildren(DialogueNodeChildren{ &mNodeVec[D0] });
          mNodeVec[B0].SetChildren(DialogueNodeChildren{ &mNodeVec[C0], &mNodeVec[C1] });
          mNodeVec[A0].SetChildren(DialogueNodeChildren{ &mNodeVec[B0] });
          mNodeVec[R0].SetChildren(DialogueNodeChildren{ &mNodeVec[A0] });
          break;
        }
        case Conversation::Name::PostQuest:
        {
          // LEVEL ROOT
          DialogueData(dataR0, "Basil: Ho ho my little sandwich,", "Basil: which side should I start on?");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataR0, 0, mSpace);
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
          // LEVEL C
          DialogueData(dataC0, "You got it!");
          mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataC0, 0, mSpace);

          // LEVEL B
          DialogueData(dataB0, "Basil: Yes, I appear to have had something spoiled.", "Basil: Would you go find the restaurant I ate at?", "Basil: Confront the chef and find out what he put in the dish!", "Basil: I must be sure to never eat anything from there again!");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataB0, 0, mSpace);

          // LEVEL A
          DialogueData(dataA0, "You're not looking too haute!");
          mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataA0, 0, mSpace);

          // LEVEL ROOT
          DialogueData(dataR0, AnimationNames::Sad);
          mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataR0, 0, mSpace);

          /*
          C0 - B0 - A0 - R0
          */
          enum { C0, B0, A0, R0 };
          mNodeVec[C0].SetChildren(DialogueNodeChildren{ });
          mNodeVec[B0].SetChildren(DialogueNodeChildren{ &mNodeVec[C0] });
          mNodeVec[A0].SetChildren(DialogueNodeChildren{ &mNodeVec[B0] });
          mNodeVec[R0].SetChildren(DialogueNodeChildren{ &mNodeVec[A0] });
          break;
        }
        case Conversation::Name::NoProgress:
        {
          // LEVEL A
          DialogueData(dataA0, "Basil: Keep it together, Basil.", "Basil: Remember Cabo, it's not as bad as Cabo...");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataA0, 0, mSpace);

          // LEVEL ROOT
          DialogueData(dataR0, AnimationNames::Sad);
          mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataR0, 0, mSpace);
          /*
          R0
          */
          enum { A0, R0 };
          mNodeVec[A0].SetChildren(DialogueNodeChildren{ });
          mNodeVec[R0].SetChildren(DialogueNodeChildren{ &mNodeVec[A0] });
          break;
        }
        case Conversation::Name::Completed:
        {
          // LEVEL C
          DialogueData(dataC0, "Basil: Well, it's for the best.", "Basil: They're someone else's problem now.");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataC0, 0, mSpace);

          // LEVEL B
          DialogueData(dataB0, "Looks like the restaurant moved on.");
          mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataB0, 0, mSpace);

          // LEVEL A
          DialogueData(dataA0, "Basil: Any luck?");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataA0, 0, mSpace);

          // LEVEL ROOT
          DialogueData(dataR0, AnimationNames::Sad);
          mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataR0, 0, mSpace);
          /*
          C0 - B0 - A0 - R0
          */
          enum { C0, B0, A0, R0 };
          mNodeVec[C0].SetChildren(DialogueNodeChildren{ });
          mNodeVec[B0].SetChildren(DialogueNodeChildren{ &mNodeVec[C0] });
          mNodeVec[A0].SetChildren(DialogueNodeChildren{ &mNodeVec[B0] });
          mNodeVec[R0].SetChildren(DialogueNodeChildren{ &mNodeVec[A0] });
          break;
        }
        case Conversation::Name::PostQuest:
        {
          // LEVEL A
          DialogueData(dataA0, "Basil: Where did I put that pink bottle?");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataA0, 0, mSpace);

          // LEVEL ROOT
          DialogueData(dataR0, AnimationNames::Sad);
          mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataR0, 0, mSpace);
          /*
          A0 - R0
          */
          enum { A0, R0 };
          mNodeVec[A0].SetChildren(DialogueNodeChildren{ });
          mNodeVec[R0].SetChildren(DialogueNodeChildren{ &mNodeVec[A0] });
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
          // LEVEL C
          DialogueData(dataC0, "Monsieur Bouillon: The newest trend in cuisine of course!", "Monsieur Bouillon: Haven't you heard?", "Monsieur Bouillon: Eating ingredients raw!", "Monsieur Bouillon: No chef could ever suit my palette, so why try?", "Monsieur Bouillon: Will you seek out Madame Daisy?", "Monsieur Bouillon: Request more raw horseradish!");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataC0, 0, mSpace);

          // LEVEL B
          DialogueData(dataB0, "Apologies! What are you up to?");
          mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataB0, 0, mSpace);

          // LEVEL A
          DialogueData(dataA0, "Basil: Excuse you, that's Monsieur Bouillon!");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataA0, 0, mSpace);

          // LEVEL ROOT
          DialogueData(dataR0, "Hey, Basil!");
          mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataR0, 0, mSpace);
          /*
          B0 - A0 - R0
          */
          enum { C0, B0, A0, R0 };
          mNodeVec[C0].SetChildren(DialogueNodeChildren{ });
          mNodeVec[B0].SetChildren(DialogueNodeChildren{ &mNodeVec[C0] });
          mNodeVec[A0].SetChildren(DialogueNodeChildren{ &mNodeVec[B0] });
          mNodeVec[R0].SetChildren(DialogueNodeChildren{ &mNodeVec[A0] });
          break;
        }
        case Conversation::Name::NoProgress:
        {
          // LEVEL B0
          DialogueData(dataB0, "Monsieur Bouillon: But it's the job, I suppose.");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataB0, 0, mSpace);

          // LEVEL A0
          DialogueData(dataA0, AnimationNames::Sad);
          mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataA0, 0, mSpace);

          // LEVEL ROOT
          DialogueData(dataR0, "Monsieur Bouillon: To be honest, I can't stand the taste.");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataR0, 0, mSpace);
          /*
          B0 - A0 - R0
          */
          enum { B0, A0, R0 };
          mNodeVec[B0].SetChildren(DialogueNodeChildren{ });
          mNodeVec[A0].SetChildren(DialogueNodeChildren{ &mNodeVec[B0] });
          mNodeVec[R0].SetChildren(DialogueNodeChildren{ &mNodeVec[A0] });
          break;
        }
        case Conversation::Name::Completed:
        {
          // LEVEL A
          DialogueData(dataA0, "Monsieur Bouillon: Please, call me Basil.", "Basil: Oh dear. Well, I can't say I'll miss that trend.", "Basil: Thank you, dearest lamb.", "Basil: Should you ever join the culinary industry, you have a friend in me.");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataA0, 0, mSpace);

          // LEVEL ROOT
          DialogueData(dataR0, "Horseradish season is over...I'm sorry!");
          mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataR0, 0, mSpace);
          /*
          B0 - A0 - R0
          */
          enum { A0, R0 };
          mNodeVec[A0].SetChildren(DialogueNodeChildren{ });
          mNodeVec[R0].SetChildren(DialogueNodeChildren{ &mNodeVec[A0] });
          break;
        }
        case Conversation::Name::PostQuest:
        {
          // LEVEL E
          DialogueData(dataE0, "Basil: You sure know how to cheer a critic up!");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataE0, 0, mSpace);

          // LEVEL D
          DialogueData(dataD0, AnimationNames::Happy);
          mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataD0, 0, mSpace);

          // LEVEL C
          DialogueData(dataC0, "There will always be a need for classy critiques!");
          mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataC0, 0, mSpace);

          // LEVEL B0
          DialogueData(dataB0, "Basil: I can't keep up with the kids and their blogs...");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataB0, 0, mSpace);

          // LEVEL A0
          DialogueData(dataA0, AnimationNames::Sad);
          mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataA0, 0, mSpace);
          // AHHHHHHHAHAHHAHH HWY DID I DO THIS BACKWARDS IM LITERALLY THE WORST PERSON TO HAVE CURSED THIS EARTH BY EXISTING
          // LEVEL ROOT
          DialogueData(dataR0, "Basil: Perhaps it is time I retire?");
          mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataR0, 0, mSpace);
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
        break;
      }
      case Quest::Name::NotActive:
      {
        // Since we know NotActive quests will only have a Hello Conversation
        // LEVEL E0
        DialogueData(dataE0, "Basil: WAIT A MINUTE!", "Basil: My critiques take time; you can't rush art!");
        mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataE0, 0, mSpace);

        // LEVEL D0
        DialogueData(dataD0, AnimationNames::Angry);
        mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataD0, 0, mSpace);

        // LEVEL C0
        DialogueData(dataC0, "Basil: As a matter of fact, I did!", "Basil: Chef John has done something heavenly with...");
        mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataC0, 0, mSpace);

        // LEVEL B0
        DialogueData(dataB0, AnimationNames::Happy);
        mNodeVec.emplace_back(DialogueNode::NodeType::Anim, dataB0, 0, mSpace);

        // LEVEL A0
        DialogueData(dataA0, "Had anything good lately?");
        mNodeVec.emplace_back(DialogueNode::NodeType::Input, dataA0, 0, mSpace);

        // LEVEL ROOT
        DialogueData(dataR0, "Basil: Hello again.");
        mNodeVec.emplace_back(DialogueNode::NodeType::Text, dataR0, 0, mSpace);
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
  Quest::Quest(Quest::Name aName, Quest::CharacterName aCharacter, Space *aSpace)
    : mName(aName), mCharacter(aCharacter), mState(Quest::State::NotActive), mSpace(aSpace)
  {
    switch (aName)
    {
      case Quest::Name::Introduction:
      {
        mConversationVec.emplace_back(Conversation::Name::Hello, Quest::Name::Introduction, aCharacter, mSpace);
        mConversationVec.emplace_back(Conversation::Name::PostQuest, Quest::Name::Introduction, aCharacter, mSpace);
        break;
      }
      case Quest::Name::Fetch:
      {
        mConversationVec.emplace_back(Conversation::Name::Hello, Quest::Name::Fetch, aCharacter, mSpace);
        mConversationVec.emplace_back(Conversation::Name::NoProgress, Quest::Name::Fetch, aCharacter, mSpace);
        mConversationVec.emplace_back(Conversation::Name::Completed, Quest::Name::Fetch, aCharacter, mSpace);
        mConversationVec.emplace_back(Conversation::Name::PostQuest, Quest::Name::Fetch, aCharacter, mSpace);
        break;
      }
      case Quest::Name::Explore:
      {
        mConversationVec.emplace_back(Conversation::Name::Hello, Quest::Name::Explore, aCharacter, mSpace);
        mConversationVec.emplace_back(Conversation::Name::NoProgress, Quest::Name::Explore, aCharacter, mSpace);
        mConversationVec.emplace_back(Conversation::Name::Completed, Quest::Name::Explore, aCharacter, mSpace);
        mConversationVec.emplace_back(Conversation::Name::PostQuest, Quest::Name::Explore, aCharacter, mSpace);
        break;
      }
      case Quest::Name::Dialogue:
      {
        mConversationVec.emplace_back(Conversation::Name::Hello, Quest::Name::Dialogue, aCharacter, mSpace);
        mConversationVec.emplace_back(Conversation::Name::NoProgress, Quest::Name::Dialogue, aCharacter, mSpace);
        mConversationVec.emplace_back(Conversation::Name::Completed, Quest::Name::Dialogue, aCharacter, mSpace);
        mConversationVec.emplace_back(Conversation::Name::PostQuest, Quest::Name::Dialogue, aCharacter, mSpace);
        break;
      }
      case Quest::Name::NotActive:
      {
        mConversationVec.emplace_back(Conversation::Name::Hello, Quest::Name::NotActive, aCharacter, mSpace);
      }
    }
  }
}//end yte