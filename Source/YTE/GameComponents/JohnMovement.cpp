/******************************************************************************/
/*!
\file   JohnMovement.cpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-03-25

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/JohnMovement.hpp"
#include "YTE/Physics/Transform.hpp"

namespace YTE
{
  YTEDefineType(JohnMovement) { YTERegisterType(JohnMovement); }

  JohnMovement::JohnMovement(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    YTEUnusedArgument(aProperties);

    mStartPos = glm::vec3(0, 0, 0);
    mDocks.emplace_back(1, 1, 1);
    mDocks.emplace_back(2, 2, 2);
    mDocks.emplace_back(3, 3, 3);
    mDockIndex = 0;
  }

  void JohnMovement::Initialize()
  {
    mOwner->GetComponent<Transform>()->SetTranslation(mStartPos);
  }

  void JohnMovement::OnQuestStart(QuestStart *aEvent)
  {
    if (aEvent->mCharacter == mOwner->GetComponent<JohnDialogue>()->GetName())
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