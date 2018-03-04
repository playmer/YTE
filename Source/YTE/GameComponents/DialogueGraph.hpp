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
  namespace AnimationNames 
  {
    static std::string WalkLoop = "NPC_Walk_Loop.fbx";
    static std::string WaveInit = "NPC_Wave_Init.fbx";
    static std::string WaveLoop = "NPC_Wave_Loop.fbx";
    static std::string WalkFull = "WalkFull_2017.fbx";
    static std::string Walk1 = "WalkPart1.fbx";
    static std::string Walk2 = "WalkPart2.fbx";
  }
  /*
  namespace SoundNames
  {
    static std::string SailingStart = "Sailing_Start";
  }
  */
#define DialogueDataType std::vector<std::string>
#define DialogueData(name, ...) DialogueDataType name{ __VA_ARGS__ }

  class DialogueNode
  {
  public:
    enum class NodeType { Anim, Input, Text, Sound };
    DialogueNode(NodeType aType, DialogueNode *aParent, int aStringCount, ...);
    //void SetActiveNode(DialogueNodeEvent *aEvent);
    //void ResponseCallback(DialogueResponseEvent *aEvent);
  private:
    NodeType mType;
    DialogueNode *mParent;
    DialogueDataType mData;

    void (DialogueNode::*mNodeLogic)();

    void PlayAnim();
    void GiveOptions();
    void RunText();
    void PlaySound();
  };
}//end yte
#endif

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