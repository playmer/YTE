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
    auto daisy = mOwner->GetComponent<DaisyDialogue>();
    auto basil = mSpace->FindFirstCompositionByName("basil");
    auto john = mSpace->FindFirstCompositionByName("john");
    if (aEvent->mCharacter == mName)
    {
      if (daisy && john && basil)
      {
        basil->GetComponent<BasilDialogue>()->DeregisterDialogue();
        john->GetComponent<JohnDialogue>()->DeregisterDialogue();
        daisy->RegisterDialogue();

        /*
        basil->GetComponent<BasilDialogue>()->DeregisterDialogue();
        auto basilDirector = basil->GetComponent<DialogueDirector>();
        if (basilDirector)
        {
          basil->RemoveComponent(basilDirector);
        }

        john->GetComponent<JohnDialogue>()->DeregisterDialogue();
        auto johnDirector = john->GetComponent<DialogueDirector>();
        if (johnDirector)
        {
          john->RemoveComponent(johnDirector);
        }

        daisy->RegisterDialogue();
        mOwner->AddComponent<DialogueDirector>();
        */
        RequestDialogueStart nextDialogue;
        mSpace->SendEvent(Events::RequestDialogueStart, &nextDialogue);
      }
    }
  }

}//end yte