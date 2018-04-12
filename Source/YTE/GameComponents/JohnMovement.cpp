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

    mStartPos = glm::vec3(161, 0, 55);
    mDocks.emplace_back(260, 1, 375);
    mDockIndex = 0;
  }

  void JohnMovement::Initialize()
  {
    mSpace->YTERegister(Events::QuestStart, this, &JohnMovement::OnQuestStart);
  }

  void JohnMovement::Start()
  {
   // mOwner->GetComponent<Transform>()->SetTranslation(mStartPos);
    //mSpace->YTERegister(Events::QuestStart, this, &JohnMovement::OnQuestStart);
  }

  void JohnMovement::OnQuestStart(QuestStart *aEvent)
  {
    if (aEvent->mCharacter == Quest::CharacterName::John)
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