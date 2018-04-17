/******************************************************************************/
/*!
\file   JohnTutorial.cpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-03-25

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/JohnTutorial.hpp"
#include "YTE/GameComponents/InputInterpreter.hpp"
#include "YTE/GameComponents/BasilDialogue.hpp"
#include "YTE/GameComponents/DaisyDialogue.hpp"
#include "YTE/GameComponents/StarMovement.hpp"

namespace YTE
{
  YTEDefineType(JohnTutorial) { YTERegisterType(JohnTutorial); }

  JohnTutorial::JohnTutorial(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    YTEUnusedArgument(aProperties);
  }

  void JohnTutorial::Initialize()
  {
    // send a RDS event to start the first dialogue
    // DialogueDirector will pick this up from the space
      // man there has got to be a better way to visualize the event network, or at least a comment standard that lets you trace logic
    RequestDialogueStart tutorialStart;
    mSpace->SendEvent(Events::RequestDialogueStart, &tutorialStart);
    // DialogueDirector will send BoatDockEvent to stop boat, play a sound
    // DD will then send DialogueStart to the space and set input context to Dialogue
    mSpace->YTERegister(Events::TutorialUpdate, this, &JohnTutorial::OnTutorialUpdate);
  }

  void JohnTutorial::OnTutorialUpdate(TutorialUpdate *aEvent)
  {
    auto john = mOwner->GetComponent<JohnDialogue>();
    auto basil = mSpace->FindFirstCompositionByName("basil");
    auto daisy = mSpace->FindFirstCompositionByName("daisy");
    if (aEvent->mCharacter == mName)
    {
      // register and send the event to start the dialogue
      if (daisy && john && basil)
      {
        basil->GetComponent<BasilDialogue>()->DeregisterDialogue();
        daisy->GetComponent<DaisyDialogue>()->DeregisterDialogue();
        john->RegisterDialogue();

        /*
        basil->GetComponent<BasilDialogue>()->DeregisterDialogue();
        auto basilDirector = basil->GetComponent<DialogueDirector>();
        if (basilDirector)
        {
          basil->RemoveComponent(basilDirector);
        }

        daisy->GetComponent<DaisyDialogue>()->DeregisterDialogue();
        auto daisyDirector = daisy->GetComponent<DialogueDirector>();
        if (daisyDirector)
        {
          daisy->RemoveComponent(daisyDirector);
        }

        john->RegisterDialogue();
        mOwner->AddComponent<DialogueDirector>();
        */
        //RequestDialogueStart nextDialogue;
        //mSpace->SendEvent(Events::RequestDialogueStart, &nextDialogue);
      }
    }
  }

}//end yte