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
  /*
  YTEDefineEvent(DialogueNodeEvent);
  YTEDefineType(DialogueNodeEvent) { YTERegisterType(DialogueNodeEvent); }

  YTEDefineEvent(DialoguePrintText);
  YTEDefineType(DialoguePrintText) { YTERegisterType(DialoguePrintText); }

  YTEDefineEvent(DialoguePlayAnim);
  YTEDefineType(DialoguePlayAnim) { YTERegisterType(DialoguePlayAnim); }

  YTEDefineEvent(DialogueGetInput);
  YTEDefineType(DialogueGetInput) { YTERegisterType(DialogueGetInput); }

  YTEDefineType(JohnDialogue) { YTERegisterType(JohnDialogue); }
  */
  DialogueNode::DialogueNode(NodeType aType, DialogueData aData, DialogueNode *aParent)
    : mType(aType), mParent(aParent), mData(aData)
  {
    switch (aType)
    {
      case NodeType::Anim:
      {
          // set the function pointer
        mNodeLogic = PlayAnim;
        break;
      }
      case NodeType::Input:
      {
          // set the function pointer
        mNodeLogic = GetInput;
        break;
      }
      case NodeType::Text:
      {
          // set the function pointer
        mNodeLogic = RunText;
        break;
      }
    }
  }

  void DialogueNode::PlayAnim()
  {
    
  }
  void DialogueNode::GetInput()
  {

  }
  void DialogueNode::RunText()
  {

  }

}//end yte