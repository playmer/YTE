/******************************************************************************/
/*!
\file   BasilDialogue.hpp
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

#ifndef YTE_Gameplay_BasilDialogue_hpp
#define YTE_Gameplay_BasilDialogue_hpp

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/GameComponents/JohnDialogue.hpp" /* TutorialUpdate */
#include "YTE/GameComponents/BoatController.hpp"
#include "YTE/GameComponents/Quest.hpp"

namespace YTE
{
  class DialogueStart;
  class PlaySoundEvent;

  class BasilDialogue : public Component
  {
  public:
    YTEDeclareType(BasilDialogue);
    BasilDialogue(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;
    void Start() override;
    // this cant be used until we know the location of the node
    void SetActiveNode(DialogueNode *aNode) { mActiveNode = aNode; };
    Quest::CharacterName GetName() { return mName; };
    Quest *GetActiveQuest() { return mActiveQuest; };

    void RegisterDialogue();
    void DeregisterDialogue();

    bool mIntroDone = false;
  private:
    void OnCollisionStarted(CollisionStarted *aEvent);
    void OnCollisionEnded(CollisionEnded *aEvent);
    void OnDialogueStart(DialogueStart *aEvent);
    void OnDialogueExit(DialogueExit *aEvent);
    void OnDialogueContinue(DialogueNodeConfirm *aEvent);
    void OnQuestStart(QuestStart *aEvent);
    void OnUpdateActiveQuestState(UpdateActiveQuestState *aEvent);
    void OnPlaySoundEvent(PlaySoundEvent *);

    std::vector<Quest> mQuestVec;
    Quest *mActiveQuest;
    Conversation *mActiveConvo;
    DialogueNode *mActiveNode;
    Quest::CharacterName mName = Quest::CharacterName::Basil;

    WWiseEmitter *mSoundEmitter;
    WWiseSystem *mSoundSystem;
    std::vector<std::vector<u64> > mDialogueConvos;
    std::vector<std::vector<u64> >::iterator mConvosIter;
    std::vector<std::vector<u64> >::iterator mPrevConvoIter;
    std::vector<u64>::iterator mLinesIter;
  };
} //end yte
#endif
