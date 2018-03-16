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
  YTEDeclareEvent(DialogueNodeReady);
  YTEDeclareEvent(DialogueNodeConfirm);

	YTEDeclareEvent(AdvanceConversation);

	class AdvanceConversation : public Event
	{
	public:
		YTEDeclareType(AdvanceConversation);
		AdvanceConversation() {  };
	};

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
  class DialogueNodeConfirm;


  class DialogueNode : public EventHandler
  {
  public:
		YTEDeclareType(DialogueNode);
    enum class NodeType { Anim, Input, Text, Sound };
      // Ctor that uses multiple const char* as variadic args
    //DialogueNode(NodeType aType, DialogueNode *aChildren, int aStringCount, ...);
      // Ctor that uses DialogueData Ctor, more readable use this one
    DialogueNode(NodeType aType, std::vector<DialogueNode*> *aChildren, DialogueDataType *aData);
    DialogueNode(NodeType aType, std::vector<DialogueNode*> *aChildren, DialogueDataType *aData, Composition *aOwner, int aId);
    //void SetActiveNode(DialogueNodeEvent *aEvent);
    //void ResponseCallback(DialogueResponseEvent *aEvent);
		void NextNode(DialogueNodeConfirm *aEvent);

    Composition *mOwner;
  private:
    NodeType mType;
    std::vector<DialogueNode*> mChildren;
    int mId;
    DialogueDataType mData;
      // @@@(Jay): This is 1000% temporary
    

    void (DialogueNode::*mNodeLogic)();

    void PlayAnim();
    void GiveOptions();
    void RunText();
    void PlaySound();
  };

  //EVENT TO LISTEN TO NODE WILL LOOK KINDA LIKE THIS
  class DialogueNodeReady : public Event
  {
  public:
    YTEDeclareType(DialogueNodeReady);
    DialogueNodeReady(std::vector<std::string> aNewMessage) {
      for (auto s : aNewMessage)
      {
        ContentMessages.push_back(s);
      }
    }
    std::vector<std::string> ContentMessages;
    DialogueNode::NodeType DialogueType;
  };

  class DialogueNodeConfirm : public Event
  {
  public:
    YTEDeclareType(DialogueNodeConfirm);
    DialogueNodeConfirm(int aSelection) { Selection = aSelection; }

    int Selection;
  };
}//end yte
#endif

 /*
 YTEDeclareEvent(DialogueResponseEvent);

 YTEDeclareEvent(DialoguePrintText);
 YTEDeclareEvent(DialoguePlayAnim);
 YTEDeclareEvent(DialogueGetInput);


 


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