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
******************************************************************************/
  Quest::Quest(Quest::Name aName)
    : mName(aName), mState(State::Available)
  {
    mConversationVec = *(new std::vector<Conversation>());
    //DialogueData(m, "HEY", "HI");
    DialogueNode(DialogueNode::NodeType::Text, nullptr, 2, "HEY", "HI");
    //DialogueData(node, "HEY HI", "HERES SOME TEXT");
    //DialogueNode d0(DialogueNode::NodeType::Text, node);
    /*
    switch (aName)
    {
      case Quest::Name::Cayenne:
      {
        DialogueNode d0(DialogueNode::NodeType::Anim, DialogueData(AnimationNames::WaveInit));
        Conversation c0(&d0);
        mConversationVec.push_back(c0);
        break;
      }
      case Quest::Name::GuessChew:
      {
        DialogueNode d0(DialogueNode::NodeType::Anim, DialogueData(AnimationNames::WaveInit));
        Conversation c0(&d0);
        mConversationVec.push_back(c0);
        break;
      }
      case Quest::Name::Ingredients:
      {
        DialogueNode d0(DialogueNode::NodeType::Anim, DialogueData(AnimationNames::WaveInit));
        Conversation c0(&d0);
        mConversationVec.push_back(c0);
        break;
      }
    }
    */
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