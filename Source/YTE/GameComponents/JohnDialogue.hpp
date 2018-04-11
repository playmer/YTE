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

#include "YTE/GameComponents/BoatController.hpp" /* inputinterpret's DialogueConfirm */
#include "YTE/GameComponents/DialogueDirector.hpp"
#include "YTE/GameComponents/Quest.hpp"

namespace YTE
{
  YTEDeclareEvent(TutorialUpdate);

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
    Quest::CharacterName mName = Quest::CharacterName::John;

    WWiseEmitter *mSoundEmitter;
    WWiseSystem *mSoundSystem;
    std::vector<std::map<std::string, u64> > mDialogueConvos;
    std::vector<std::map<std::string, u64> >::iterator mConvosIter;
    std::map<std::string, u64>::iterator mLinesIter;
  };
} //end yte
#endif
