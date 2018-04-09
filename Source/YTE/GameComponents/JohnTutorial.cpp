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
    auto basil = mOwner->GetComponent<BasilDialogue>();
    auto daisy = mOwner->GetComponent<DaisyDialogue>();
    if (aEvent->mCharacter == mName)
    {
      // register and send the event to start the dialogue
      if (john)
      {
        john->RegisterDialogue();
        basil->DeregisterDialogue();
        daisy->DeregisterDialogue();

        DialogueStart diaStart;
        mSpace->SendEvent(Events::DialogueStart, &diaStart);
      }
    }
  }

}//end yte