/******************************************************************************/
/*!
\file   DaisyTutorial.cpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-03-25

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/DaisyTutorial.hpp"
#include "YTE/GameComponents/DaisyDialogue.hpp"
#include "YTE/GameComponents/BasilDialogue.hpp"

namespace YTE
{
  YTEDefineType(DaisyTutorial) { YTERegisterType(DaisyTutorial); }

  DaisyTutorial::DaisyTutorial(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    YTEUnusedArgument(aProperties);
  }

  void DaisyTutorial::Initialize()
  {
    mSpace->YTERegister(Events::TutorialUpdate, this, &DaisyTutorial::OnTutorialUpdate);
  }

  void DaisyTutorial::OnTutorialUpdate(TutorialUpdate *aEvent)
  {
    auto john = mOwner->GetCompositions()->FindFirst("john")->second->GetComponent<JohnDialogue>();
    auto basil = mOwner->GetCompositions()->FindFirst("basil")->second->GetComponent<BasilDialogue>();
    auto daisy = mOwner->GetCompositions()->FindFirst("daisy")->second->GetComponent<DaisyDialogue>();
    if (aEvent->mCharacter == mName)
    {
      if (daisy)
      {
        daisy->RegisterDialogue();
        john->DeregisterDialogue();
        basil->DeregisterDialogue();

        DialogueStart diaStart;
        mSpace->SendEvent(Events::DialogueStart, &diaStart);
      }
    }
  }

}//end yte