/******************************************************************************/
/*!
\file   JohnTutorial.hpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-03-25
\brief
    This component handles the logic for running johns tutorial

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once
#ifndef YTE_Gameplay_JohnTutorial_hpp
#define YTE_Gameplay_JohnTutorial_hpp

#include "YTE/Core/Composition.hpp"
#include "YTE/GameComponents/JohnDialogue.hpp" /* TutorialUpdate */

namespace YTE
{
  class JohnTutorial : public Component
  {
  public:
    YTEDeclareType(JohnTutorial);
    JohnTutorial(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;

    void OnTutorialUpdate(TutorialUpdate *aEvent);
  private:
    Quest::CharacterName mName = Quest::CharacterName::John;
  };
}//end yte
#endif