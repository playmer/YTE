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
  YTEDefineEvent(DialogueNodeEvent);
  YTEDefineType(DialogueNodeEvent) { YTERegisterType(DialogueNodeEvent); }

  YTEDefineEvent(DialoguePrintText);
  YTEDefineType(DialoguePrintText) { YTERegisterType(DialoguePrintText); }

  YTEDefineEvent(DialoguePlayAnim);
  YTEDefineType(DialoguePlayAnim) { YTERegisterType(DialoguePlayAnim); }

  YTEDefineEvent(DialogueGetInput);
  YTEDefineType(DialogueGetInput) { YTERegisterType(DialogueGetInput); }

  YTEDefineType(JohnDialogue) { YTERegisterType(JohnDialogue); }

  DialogueNode::DialogueNode(NodeType aType)
    : mType(aType)
  {
    
  }

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

/******************************************************************************
  Conversation
******************************************************************************/
  Conversation::Conversation(Conversation::Name aName)
  {
    switch (aName)
    {
      case Conversation::Name::Introduction:
      {

      }
      case Conversation::Name::Hello:
      {

      }
      case Conversation::Name::StartQuest:
      {

      }
      case Conversation::Name::ProgressQuest:
      {

      }
      case Conversation::Name::FinishQuest:
      {

      }
      case Conversation::Name::Goodbye:
      {

      }
      case Conversation::Name::NoQuest:
      {

      }
    }
  }

/******************************************************************************
  Quest
******************************************************************************/
  Quest::Quest(Quest::Name aName)
    : mName(aName), mState(State::Available)
  {
    mConversationVec = *(new std::vector<Conversation>());
    mConversationVec.push_back(*(new Conversation(Conversation::Name::Introduction)));
    mConversationVec.push_back(*(new Conversation(Conversation::Name::Hello)));
    mConversationVec.push_back(*(new Conversation(Conversation::Name::StartQuest)));
    mConversationVec.push_back(*(new Conversation(Conversation::Name::ProgressQuest)));
    mConversationVec.push_back(*(new Conversation(Conversation::Name::FinishQuest)));
    mConversationVec.push_back(*(new Conversation(Conversation::Name::Goodbye)));
    mConversationVec.push_back(*(new Conversation(Conversation::Name::NoQuest)));
  }



/******************************************************************************
  Dialogue Component
******************************************************************************/
  JohnDialogue::JohnDialogue(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    mQuestVec = *(new std::vector<Quest>());
  }

  void JohnDialogue::Initialize()
  {
    
  }


} //end yte