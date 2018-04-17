/******************************************************************************/
/*!
\file   StarMovement.hpp
\author Nicholas Ammann
\par    email : nicholas.ammann\@digipen.edu
\date   2018 - 04 - 15

All content(c) 2016 DigiPen(USA) Corporation, all rights reserved.
* /
/******************************************************************************/
#pragma once

#ifndef YTE_Gameplay_StarMovement_hpp
#define YTE_Gameplay_StarMovement_hpp

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/GameComponents/JohnDialogue.hpp" /* TutorialUpdate */
#include "YTE/GameComponents/Quest.hpp"             /* Quest, QuestStart, UpdateActiveQuestState */

namespace YTE
{
  class DialogueStart;

  class StarMovement : public Component
  {
  public:
    YTEDeclareType(StarMovement);
    StarMovement(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;

    void Update(LogicUpdate *aEvent);

    void SetActive(glm::vec3 aPos);

  private:
    void OnDialogueStart(DialogueStart *aEvent);
    void OnDialogueExit(DialogueExit *aEvent);
    void OnDialogueContinue(DialogueNodeConfirm *aEvent);
    void OnQuestStart(QuestStart *aEvent);
    void OnUpdateActiveQuestState(UpdateActiveQuestState *aEvent);

    Transform *mStarTransform;
    Transform *mNoticeBoardTransform;
    Transform *mJohnTransform;
    Transform *mDaisyTransform;
    Transform *mBasilTransform;

    Transform *mLightBeamTransform;
    Transform *mBoatTransform;

    double mTimer;

    enum CurrentAnchor
    {
      John,
      Daisy,
      Basil,
      NoticeBoard,
      None
    };

    CurrentAnchor mCurrentAnchor;

  };
} //end yte
#endif
