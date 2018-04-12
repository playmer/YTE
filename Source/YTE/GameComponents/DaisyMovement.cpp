/******************************************************************************/
/*!
\file   DaisyMovement.cpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-03-25

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/DaisyMovement.hpp"
#include "YTE/GameComponents/DaisyDialogue.hpp"
#include "YTE/Physics/Transform.hpp"

namespace YTE
{
  YTEDefineType(DaisyMovement) { YTERegisterType(DaisyMovement); }

  DaisyMovement::DaisyMovement(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    YTEUnusedArgument(aProperties);

    mStartPos = glm::vec3(161, 0, 55);
    mDocks.emplace_back(260, 1, 425);
    mDockIndex = 0;
  }

  void DaisyMovement::Initialize()
  {
    //mOwner->GetComponent<Transform>()->SetTranslation(mStartPos);
    mSpace->YTERegister(Events::QuestStart, this, &DaisyMovement::OnQuestStart);
  }

  void DaisyMovement::MoveToNextDock()
  {
    mOwner->GetComponent<Transform>()->SetTranslation(mDocks[mDockIndex]);
    ++mDockIndex;
    if (mDockIndex > mDocks.size() - 1)
    {
      mDockIndex = 0;
    }
  }

  void DaisyMovement::OnQuestStart(QuestStart *aEvent)
  {
    if (aEvent->mCharacter == Quest::CharacterName::Daisy)
    {
      MoveToNextDock();
    }
    else
    {
      auto dialogue = mOwner->GetComponent<DaisyDialogue>();
      if (dialogue->mIntroDone || dialogue->GetActiveQuest()->GetName() == Quest::Name::Introduction)
      {
        MoveToNextDock();
      }
    }
  }
}//end yte