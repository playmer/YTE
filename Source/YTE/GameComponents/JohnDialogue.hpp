/******************************************************************************/
/*!
\file   JohnDialogue.hpp
\author Jonathan Ackerman
\par    email : jonathan.ackerman\@digipen.edu
\date   2018 - 02 - 27
\brief
Unique component for John's dialogues. This component defines and runs logic
for all John conversations

All content(c) 2016 DigiPen(USA) Corporation, all rights reserved.
* /
/******************************************************************************/

#pragma once

#ifndef YTE_Gameplay_JohnDialogue_hpp
#define YTE_Gameplay_JohnDialogue_hpp

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/Engine.hpp"

namespace YTE
{
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

  class DialogueNode
  {
  public:
    enum class NodeType { Text, Anim, Input };
    DialogueNode(NodeType aType);
    void SetActiveNode(DialogueNodeEvent *aEvent);
    void ResponseCallback(DialogueResponseEvent *aEvent);
  private:
    NodeType mType;
  };

  class Conversation
  {
  public:
    enum class Name { Introduction, Hello, StartQuest, ProgressQuest, FinishQuest, Goodbye, NoQuest };
    Conversation(Conversation::Name aName);
  private:
    Conversation::Name mName;
    std::vector<std::string> mLines;
  };

  class Quest
  {
  public:
    enum class State { Available, InProgress, Completed };
    enum class Name { GuessChew, Ingredients, Cayenne };
    Quest(Quest::Name aName);

    Quest::Name GetName() { return mName; };
    Quest::State GetState() { return mState; };

    void SetState(Quest::State aState) { mState = aState; };
  private:
    Quest::Name mName;
    Quest::State mState;
    std::vector<Conversation> mConversationVec;
  };

  class JohnDialogue : public Component
  {
  public:
    YTEDeclareType(JohnDialogue);
    JohnDialogue(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;
    //int UserInput(std::string aResponse...);
    //std::string SetQuest();
  private:
    std::vector<Quest> mQuestVec;
    Quest mActiveQuest;
  };
} //end yte
#endif
