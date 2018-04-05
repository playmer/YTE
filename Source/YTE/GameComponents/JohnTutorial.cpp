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
#include "YTE/GameComponents/DialogueDirector.hpp"

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
    RequestDialogueStart tutorialStart;
    mSpace->SendEvent(Events::RequestDialogueStart, &tutorialStart);
  }

}//end yte