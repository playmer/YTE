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
  YTEDefineType(JohnDialogue)
  {
    YTERegisterType(JohnDialogue);
  }

  /*
  void DialogueNode::SetActiveNode(DialogueNodeEvent *aEvent)
  {
    mOwner->GetSpace()->YTEDeregister(Events::DialogueNodeEvent, this, &DialogueNode::SetActiveNode);
    mOwner->GetSpace()->YTERegister(Events::DialogueResponseEvent, this, &DialogueNode::Callback);
    switch (mType)
    {
      case NodeType::Text:
      {
        DialoguePrintText node;
        node.Data.push_back("here is the string data");
        mOwner->GetSpace()->SendEvent("DialoguePrintText", &node);
      }
      case NodeType::Anim:
      {
        DialoguePlayAnim node;
        node.Data.push_back("here is the string data");
        mOwner->GetSpace()->SendEvent("DialoguePlayAnim", &node);
      }
      case NodeType::Input:
      {
        DialogueGetInput node;
        node.Data.push_back("here is the string data");
        mOwner->GetSpace()->SendEvent("DialogueGetInput", &node);
      }
    }
  }

  void DialogueNode::ResponseCallback(DialogueResponseEvent *aEvent)
  {
    mOwner->GetSpace()->YTEDeregister(Events::DialogueResponseEvent, this, &DialogueNode::ResponseCallback);
    DialogueNodeEvent next;
    SendEvent("DialogueNodeEvent", &next);
  }
  */
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
  }
  /*
  void Quest::AddConvo(Conversation *aConvo)
  {
    mConversationVec.push_back(*aConvo);
  }
  */

/******************************************************************************
  Dialogue Component
******************************************************************************/
  JohnDialogue::JohnDialogue(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    YTEUnusedArgument(aProperties);
    mQuestVec = *(new std::vector<Quest>());
  }

  // Here I begin the long and tedious process of hardcoding John's quests/convos
  void JohnDialogue::Initialize()
  {
    /*
    // Construct the quest
    Quest q1(Quest::Name::GuessChew);
    // Root node
    DialogueData testvec2;
    testvec2.push_back(AnimationNames::WaveLoop);
    DialogueNode convoRoot(DialogueNode::NodeType::Anim, testvec2);
    // Construct the conversation
    Conversation c1(&convoRoot);
    // Start adding nodes
    const std::string test = "HEY NICE TO MEET YOU";
    DialogueData testvec;
    testvec.push_back(test);
    DialogueNode n1(DialogueNode::NodeType::Text, testvec, &convoRoot);
    // Add the dialogue graph to the quest
    q1.AddConvo(&c1);
    */
  }


} //end yte