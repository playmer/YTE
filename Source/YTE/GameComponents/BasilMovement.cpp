/******************************************************************************/
/*!
\file   BasilMovement.cpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-03-25

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/BasilMovement.hpp"
#include "YTE/GameComponents/BasilDialogue.hpp"
#include "YTE/Physics/Transform.hpp"

namespace YTE
{
  YTEDefineType(BasilMovement) { YTERegisterType(BasilMovement); }

  BasilMovement::BasilMovement(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    YTEUnusedArgument(aProperties);

    mStartPos = glm::vec3(161, 0, 55);
    mDocks.emplace_back(260, 1, 450);
    mDockIndex = 0;
  }

  void BasilMovement::Initialize()
  {
    //mOwner->GetComponent<Transform>()->SetTranslation(mStartPos);
    mSpace->YTERegister(Events::QuestStart, this, &BasilMovement::OnQuestStart);
  }

  void BasilMovement::Start()
  {
    //mOwner->GetComponent<Transform>()->SetTranslation(mStartPos);
  }

  void BasilMovement::OnQuestStart(QuestStart *aEvent)
  {
    if (aEvent->mCharacter == mOwner->GetComponent<BasilDialogue>()->GetName())
    {
      mOwner->GetComponent<Transform>()->SetTranslation(mDocks[mDockIndex]);
      ++mDockIndex;
      if (mDockIndex > mDocks.size() - 1)
      {
        mDockIndex = 0;
      }
    }
  }
}//end yte