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
#include "YTE/GameComponents/StarMovement.hpp"

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
    auto basil = mOwner->GetComponent<BasilDialogue>();
    auto john = mSpace->FindFirstCompositionByName("john");
    auto daisy = mSpace->FindFirstCompositionByName("daisy");
    if (aEvent->mCharacter == mName)
    {
      if (daisy && john && basil)
      {
        john->GetComponent<JohnDialogue>()->DeregisterDialogue();
        daisy->GetComponent<DaisyDialogue>()->DeregisterDialogue();
        basil->RegisterDialogue();

        /*
        john->GetComponent<JohnDialogue>()->DeregisterDialogue();
        auto johnDirector = john->GetComponent<DialogueDirector>();
        if (johnDirector)
        {
          john->RemoveComponent(johnDirector);
        }

        daisy->GetComponent<DaisyDialogue>()->DeregisterDialogue();
        auto daisyDirector = daisy->GetComponent<DialogueDirector>();
        if (daisyDirector)
        {
          daisy->RemoveComponent(daisyDirector);
        }

        basil->RegisterDialogue();
        mOwner->AddComponent<DialogueDirector>();
        */
        //RequestDialogueStart nextDialogue;
        //mSpace->SendEvent(Events::RequestDialogueStart, &nextDialogue);
      }
    }
  }

}//end yte