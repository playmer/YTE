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
  YTEDefineEvent(RunConversation);
  YTEDefineType(RunConversation) { YTERegisterType(RunConversation); }

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
      The quest system should just iterate through the quests, not choose randomly
      that way the player always encounters the Introduction quest first. Going
      to need a public function to set quest state, so the quest system can mark
      Introduction as done without the player having to sail home.
******************************************************************************/
  Quest::Quest(Quest::Name aName)
    : mName(aName), mState(State::Available)
  {
    mConversationVec = *(new std::vector<Conversation>());
      // This is the ugly way
    //DialogueNode(DialogueNode::NodeType::Text, nullptr, 2, "HEY", "HI");
      // This way is more readable
    //DialogueData(data, "HEY", "HI");
    //DialogueNode root = DialogueNode(DialogueNode::NodeType::Text, nullptr, &data);
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
        DialogueNode nodeD0(DialogueNode::NodeType::Text, nullptr, &dataD0);

        children.push_back(&nodeD0);

        // LEVEL C
        DialogueData(dataC0, "Perfect timing, my fish is still in the oven", "I need you to go fetch my missing ingredients");
        DialogueNode nodeC0(DialogueNode::NodeType::Text, &children, &dataC0);

        DialogueData(dataC1, "Suuuure...", "Well while you're here, I need you to fetch my missing ingredients");
        DialogueNode nodeC1(DialogueNode::NodeType::Text, &children, &dataC1);

        children.clear();
        children.push_back(&nodeC0);
        children.push_back(&nodeC1);

        // LEVEL B
        DialogueData(dataB0, "Oh I'm not ordering, I'm here to help", "Friendship!");
        DialogueNode nodeB0(DialogueNode::NodeType::Input, &children, &dataB0);

        children.clear();
        children.push_back(&nodeB0);

        // LEVEL A
        DialogueData(dataA0, "HOT BEHIND!", "Just one second I'm finishing this meal", "Okay what did you want to order?");
        DialogueNode nodeA0(DialogueNode::NodeType::Text, &children, &dataA0);

        children.clear();
        children.push_back(&nodeA0);

        // LEVEL ROOT
        DialogueData(data, AnimationNames::WaveInit);
        DialogueNode root(DialogueNode::NodeType::Anim, &children, &data);

        delete &children;

        Conversation c0(&root);

        mConversationVec.push_back(c0);
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
  }
  
  void Quest::AddConvo(Conversation *aConvo)
  {
    mConversationVec.push_back(*aConvo);
  }
  

/******************************************************************************
  Dialogue Component
******************************************************************************/
  JohnDialogue::JohnDialogue(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    YTEUnusedArgument(aProperties);
      // Construct the Quest Vector
    mQuestVec = *(new std::vector<Quest> { 
      Quest::Name::Introduction,
      Quest::Name::GuessChew, 
      Quest::Name::Cayenne, 
      Quest::Name::Ingredients 
    });
  }

  void JohnDialogue::Initialize()
  {
    // @@@JAY: This register and deregister should happen on CollisionStart and CollisionEnd so that other characters dont listen when they shouldnt
    mOwner->YTERegister(Events::RunConversation, this, &JohnDialogue::StartConvo);
    // @@@JAY: Maybe make a macro for this :)
    // Construct the quest
    //Quest q1(Quest::Name::GuessChew);
    // Root node
    /*
    DialogueDataType testvec2;
    testvec2.push_back(AnimationNames::WaveLoop);
    DialogueNode convoRoot(DialogueNode::NodeType::Anim, testvec2);
    // Construct the conversation
    Conversation c1(&convoRoot);
    // Start adding nodes
    const std::string test = "HEY NICE TO MEET YOU";
    DialogueDataType testvec;
    testvec.push_back(test);
    DialogueNode n1(DialogueNode::NodeType::Text, testvec, &convoRoot);
    // Add the dialogue graph to the quest
    //q1.AddConvo(&c1);
    */
  }

  void JohnDialogue::StartConvo(RunConversation *aEvent)
  {
    // pick quest
    // register root node of conversation
  }


} //end yte