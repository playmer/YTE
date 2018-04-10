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

#include "YTE/WWise/WWiseEmitter.hpp"
#include "YTE/WWise/WWiseSystem.hpp"

#include "YTE/GameComponents/BoatController.hpp"
#include "YTE/GameComponents/DialogueDirector.hpp"
#include "YTE/GameComponents/Quest.hpp"

namespace YTE
{
  YTEDeclareEvent(TutorialUpdate);

  namespace DynamicDialogueArgs
  {
    namespace Quest
    {
      static const char *Fetch = "CJ_FetchingIngredients";
      static const char *Explore = "CJ_MiseEnPlace";
      static const char *Dialogue = "CJ_GuessChew";
      static const char *NotActive = "CJ_NoQuest";
    }
    namespace Conversation
    {
      static const char *Hello = "Hello";
      static const char *NoProgress = "NoProgress";
      static const char *Completed = "Completed";
      static const char *PostQuest = "Goodbye";
    }
    namespace Line
    {
      static const char *L01 = "L01";
      static const char *L02 = "L02";
      static const char *L03 = "L03";
      static const char *L04 = "L04";
      static const char *L05 = "L05";
      static const char *L06 = "L06";
      static const char *L07 = "L07";
      static const char *L08 = "L08";
      static const char *L09 = "L09";
      static const char *L10 = "L10";
    }
  }

  class TutorialUpdate : public Event
  {
  public:
    YTEDeclareType(TutorialUpdate);
    TutorialUpdate(Quest::CharacterName aCharacter) : mCharacter(aCharacter) {};
    Quest::CharacterName mCharacter;
  };

  class JohnDialogue : public Component
  {
  public:
    YTEDeclareType(JohnDialogue);
    JohnDialogue(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;
    void Start() override;
    // this cant be used until we know the location of the node
    void SetActiveNode(DialogueNode *aNode) { mActiveNode = aNode; };
    Quest::CharacterName GetName() { return mName; };

    // its over for me, this is the end, how did i end up here, probz cause im fucking stupid and should just end it all, the ultimate sacrifice for the betterment of humanity
    void RegisterDialogue();
    void DeregisterDialogue();
    const char **StatesToStrings();
  private:
    void OnCollisionStarted(CollisionStarted *aEvent);
    void OnCollisionEnded(CollisionEnded *aEvent);
    void OnDialogueStart(DialogueStart *aEvent);
    void OnDialogueExit(DialogueExit *aEvent);
    void OnDialogueContinue(DialogueNodeConfirm *aEvent);
    void OnQuestStart(QuestStart *aEvent);
    void OnUpdateActiveQuestState(UpdateActiveQuestState *aEvent);

    std::vector<Quest> mQuestVec;
    Quest *mActiveQuest;
    Conversation *mActiveConvo;
    DialogueNode *mActiveNode;
    Quest::CharacterName mName = Quest::CharacterName::John;

    WWiseEmitter *mSoundEmitter;
    WWiseSystem *mSoundSystem;
    u64 mSoundDialogueJohn;
  };
} //end yte
#endif
