/******************************************************************************/
/*!
\file   BasilTutorial.cpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-03-25

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/BasilTutorial.hpp"
#include "YTE/GameComponents/BasilDialogue.hpp"
#include "YTE/GameComponents/DaisyDialogue.hpp"

namespace YTE
{
  YTEDefineType(BasilTutorial) { YTERegisterType(BasilTutorial); }

  BasilTutorial::BasilTutorial(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    YTEUnusedArgument(aProperties);
  }

  void BasilTutorial::Initialize()
  {
    mSpace->YTERegister(Events::TutorialUpdate, this, &BasilTutorial::OnTutorialUpdate);
  }

  void BasilTutorial::OnTutorialUpdate(TutorialUpdate *aEvent)
  {
    auto john = mOwner->GetComponent<JohnDialogue>();
    auto basil = mOwner->GetComponent<BasilDialogue>();
    auto daisy = mOwner->GetComponent<DaisyDialogue>();
    if (aEvent->mCharacter == mName)
    {
      if (basil)
      {
        basil->RegisterDialogue();
        john->DeregisterDialogue();
        daisy->DeregisterDialogue();

        DialogueStart diaStart;
        mSpace->SendEvent(Events::DialogueStart, &diaStart);
      }
    }
  }

}//end yte