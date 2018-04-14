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

  YTEDeclareEvent(PlayAnimationEvent);

  class PlayAnimationEvent : public Event
  {
  public:
    YTEDeclareType(PlayAnimationEvent);
    std::string animationName;
  };

  class DialogueNodeReady;
  class DialogueNodeConfirm;

  namespace AnimationNames 
  {
    static std::string WalkLoop = "NPC_Walk_Loop.fbx";
    static std::string WaveInit = "NPC_Wave_Init.fbx";
    static std::string WaveLoop = "NPC_Wave_Loop.fbx";
    static std::string WalkFull = "WalkFull_2017.fbx";
    static std::string Walk1 = "WalkPart1.fbx";
    static std::string Walk2 = "WalkPart2.fbx";
    static std::string Sad = "Sad";
    static std::string Idle = "Idle";
    static std::string Happy = "Joy";
    static std::string Angry = "Angry";
    static std::string Wave = "Wave";
  }
  /*
  namespace SoundNames
  {
    static std::string SailingStart = "Sailing_Start";
  }
  */

#define DialogueDataType std::vector<std::string>
#define DialogueData(name, ...) DialogueDataType name{ __VA_ARGS__ }
#define DialogueNodeChildren std::vector<DialogueNode*>
//#define DialogueNodeChildren(...) DialogueNodeChildType{ __VA_ARGS__ }

  class DialogueNode
  {
  public:
    YTEDeclareType(DialogueNode);
    enum class NodeType { Anim, Input, Text, Sound };

    DialogueNode(NodeType aType, DialogueDataType aData, int aId, Space *space);
    //DialogueNode& operator=(const DialogueNode& aNode) = default;
    //DialogueNode(DialogueNode&& aNode);

    void ActivateNode();
    DialogueNode *GetChild(int pos);
    //void SetChildren(int aCount, DialogueNode *aChild, ...);
    void SetChildren(std::vector<DialogueNode*>&& aChildren);
    NodeType GetNodeType() { return mType; };
    DialogueDataType GetNodeData() { return mData; };

  private:
    NodeType mType;
    int mId;
    void (DialogueNode::*mNodeLogic)();
    DialogueDataType mData;
    std::vector<DialogueNode*> mChildren;

    Space *mSpace;

    void PlayAnim();
    void GiveOptions();
    void RunText();
    void PlaySound();
  };

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
