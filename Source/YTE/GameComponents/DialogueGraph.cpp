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
  YTEDefineType(DialogueNode) { YTERegisterType(DialogueNode); }

  YTEDefineEvent(DialogueNodeReady);
  YTEDefineEvent(DialogueNodeConfirm);

  YTEDefineType(DialogueNodeReady) { YTERegisterType(DialogueNodeReady); }
  YTEDefineType(DialogueNodeConfirm) { YTERegisterType(DialogueNodeConfirm); }

  /*
  DialogueNode::DialogueNode(DialogueNode&& aNode)
    : mType(std::move(aNode.mType)), mId(std::move(aNode.mId)), mNodeLogic(std::move(aNode.mNodeLogic)), mData(std::move(aNode.mData)), mChildren(std::move(aNode.mChildren))
  {
  }
  */

  DialogueNode::DialogueNode(NodeType aType, DialogueDataType aData, int aId)
    : mType(aType), mData(std::move(aData)), mId(aId)
  {
    switch (aType)
    {
      case NodeType::Anim:
      {
        mNodeLogic = &DialogueNode::PlayAnim;
        break;
      }
      case NodeType::Input:
      {
        mNodeLogic = &DialogueNode::GiveOptions;
        break;
      }
      case NodeType::Text:
      {
        mNodeLogic = &DialogueNode::RunText;
        break;
      }
      case NodeType::Sound:
      {
        mNodeLogic = &DialogueNode::PlaySound;
        break;
      }
    }
  }

  DialogueNode *DialogueNode::GetChild(int pos)
  {
    if (mChildren.size() > 0)
    {
      return mChildren[pos];
    }
    else
    {
      return nullptr;
    }
  }
  /*
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
  }*/

  void DialogueNode::SetChildren(std::vector<DialogueNode*>&& aChildren)
  {
    mChildren = std::move(aChildren);
  }

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