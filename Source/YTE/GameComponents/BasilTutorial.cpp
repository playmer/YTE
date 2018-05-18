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
#include "YTE/GameComponents/HudController.hpp"

namespace YTE
{
  YTEDefineType(BasilTutorial) { RegisterType<BasilTutorial>();
    TypeBuilder<BasilTutorial> builder; }

  BasilTutorial::BasilTutorial(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    UnusedArguments(aProperties);
  }

  void BasilTutorial::Initialize()
  {
    mSpace->RegisterEvent<&BasilTutorial::OnTutorialUpdate>(Events::TutorialUpdate, this);
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

        DialoguePossible diagEvent;
        diagEvent.isPossible = false;
        mSpace->SendEvent(Events::DialoguePossible, &diagEvent);

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