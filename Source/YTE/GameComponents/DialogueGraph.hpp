/******************************************************************************/
/*!
\file   DialogueGraph.hpp
\author Jonathan Ackerman
\par    email : jonathan.ackerman\@digipen.edu
\date   2018 - 03 - 01
\brief
Definition for the dialogue graph. This is a class that defines the data structure
for a conversation.

All content(c) 2016 DigiPen(USA) Corporation, all rights reserved.
* /
/******************************************************************************/

#pragma once

#ifndef YTE_Gameplay_DialogueGraph_hpp
#define YTE_Gameplay_DialogueGraph_hpp

#include "YTE/Core/Engine.hpp"

namespace YTE
{
  /*
  YTEDeclareEvent(DialogueNodeEvent);
  YTEDeclareEvent(DialogueResponseEvent);

  YTEDeclareEvent(DialoguePrintText);
  YTEDeclareEvent(DialoguePlayAnim);
  YTEDeclareEvent(DialogueGetInput);


  class DialogueNodeEvent : public Event
  {
  public:
    YTEDeclareType(DialogueNodeEvent);
    DialogueNodeEvent() {  };
  };
  class DialogueResponseEvent : public Event
  {
  public:
    YTEDeclareType(DialogueResponseEvent);
    DialogueResponseEvent() {  };

    int ResponseNumber;
  };
  class DialoguePrintText : public Event
  {
  public:
    YTEDeclareType(DialoguePrintText);
    DialoguePrintText() {  };

    std::vector<std::string> Data;
  };
  class DialoguePlayAnim : public Event
  {
  public:
    YTEDeclareType(DialoguePlayAnim);
    DialoguePlayAnim() {  };

    std::vector<std::string> Data;
  };
  class DialogueGetInput : public Event
  {
  public:
    YTEDeclareType(DialogueGetInput);
    DialogueGetInput() {  };

    std::vector<std::string> Data;
  };
  */

  namespace AnimationNames {
    inline static const std::string WalkLoop = "NPC_Walk_Loop.fbx";
    inline static const std::string WaveInit = "NPC_Wave_Init.fbx";
    inline static const std::string WaveLoop = "NPC_Wave_Loop.fbx";
    inline static const std::string WalkFull = "WalkFull_2017.fbx";
    inline static const std::string Walk1 = "WalkPart1.fbx";
    inline static const std::string Walk2 = "WalkPart2.fbx";
  }

  class DialogueNode
  {
  public:
    enum class NodeType { Anim, Input, Text };
    DialogueNode(NodeType aType, std::vector<const std::string> aData, DialogueNode *aParent = NULL);
    //void SetActiveNode(DialogueNodeEvent *aEvent);
    //void ResponseCallback(DialogueResponseEvent *aEvent);
  private:
    NodeType mType;
    DialogueNode *mParent;
    void(DialogueNode::*mNodeLogic)(void);
    void PlayAnim(void);
    void GetInput(void);
    void RunText(void);
    std::vector<const std::string> mData;
  };
}//end yte
#endif