/******************************************************************************/
/*!
\file   DialogueGraph.cpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018 - 03 - 01

All content (c) 2018 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/DialogueGraph.hpp"

namespace YTE
{
  YTEDefineEvent(AdvanceConversation);
  YTEDefineType(AdvanceConversation) { YTERegisterType(AdvanceConversation); }

	YTEDefineType(DialogueNode) { YTERegisterType(DialogueNode); }

  YTEDefineEvent(DialogueNodeReady);
  YTEDefineEvent(DialogueNodeConfirm);

  YTEDefineType(DialogueNodeReady) { YTERegisterType(DialogueNodeReady); }
  YTEDefineType(DialogueNodeConfirm) { YTERegisterType(DialogueNodeConfirm); }

  /*

  YTEDefineEvent(DialoguePrintText);
  YTEDefineType(DialoguePrintText) { YTERegisterType(DialoguePrintText); }

  YTEDefineEvent(DialoguePlayAnim);
  YTEDefineType(DialoguePlayAnim) { YTERegisterType(DialoguePlayAnim); }

  YTEDefineEvent(DialogueGetInput);
  YTEDefineType(DialogueGetInput) { YTERegisterType(DialogueGetInput); }

  YTEDefineType(JohnDialogue) { YTERegisterType(JohnDialogue); }
  */
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

	// Data is being shallow copieddddddddd
	DialogueNode::DialogueNode(NodeType aType, DialogueDataType *aData, int aId)
    : mType(aType), mId(aId)
  {
		for (auto i : *aData)
		{
			mData.push_back(i);
		}
    switch (aType)
    {
			case NodeType::Anim:
			{
				// set the function pointer
				mNodeLogic = &DialogueNode::PlayAnim;
				break;
			}
			case NodeType::Input:
			{
				// set the function pointer
				mNodeLogic = &DialogueNode::GiveOptions;
				break;
			}
			case NodeType::Text:
			{
				// set the function pointer
				mNodeLogic = &DialogueNode::RunText;
				break;
			}
			case NodeType::Sound:
			{
				// set the function pointer
				mNodeLogic = &DialogueNode::PlaySound;
				break;
			}
    }
  }

	void DialogueNode::SetChildren(int aCount, DialogueNode *aNode, ...)
	{
		mChildren = *(new std::vector<DialogueNode*>);
		if (aCount > 0)
		{
			va_list ap;
			va_start(ap, aCount);
			for (int i = 0; i < aCount; ++i)
			{
				mChildren.push_back(va_arg(ap, DialogueNode*));
			}
			va_end(ap);
		}
	}

  /*

	(JAY): dont delete this yet, we're not using it but its a cool thing i want to look at later, ty!

  DialogueNode::DialogueNode(NodeType aType, DialogueNode *aParent, int aStringCount, ...)
    : mType(aType), mParent(aParent)
  {
    mData = *(new std::vector<std::string>);
    va_list ap;
    va_start(ap, aStringCount);
    for (int i = 0; i < aStringCount; ++i)
    {
      mData.push_back(va_arg(ap, std::string));
    }
    va_end(ap);

    switch (aType)
    {
      case NodeType::Anim:
      {
          // set the function pointer
        mNodeLogic = &DialogueNode::PlayAnim;
        break;
      }
      case NodeType::Input:
      {
          // set the function pointer
        mNodeLogic = &DialogueNode::GiveOptions;
        break;
      }
      case NodeType::Text:
      {
          // set the function pointer
        mNodeLogic = &DialogueNode::RunText;
        break;
      }
      case NodeType::Sound:
      {
          // set the function pointer
        mNodeLogic = &DialogueNode::PlaySound;
        break;
      }
    }
  }
  */

	void DialogueNode::ActivateNode()
	{
    mNodeLogic;
	}

	// Functors
  void DialogueNode::PlayAnim()
  {
    //mOwner->GetComponent<Animator>();
  }
  void DialogueNode::GiveOptions()
  {
    //DialogueNodeReady options(mData);
    //mOwner->GetSpace()->SendEvent(Events::DialogueNodeReady, &options);
  }
  void DialogueNode::RunText()
  {
    //DialogueNodeReady options(mData);
    //mOwner->GetSpace()->SendEvent(Events::DialogueNodeReady, &options);
  }
  void DialogueNode::PlaySound()
  {
    //mOwner->GetComponent<WWiseEmitter>()
  }

}//end yte