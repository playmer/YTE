/******************************************************************************/
/*!
\file   BasilTutorial.hpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-03-25
\brief
    This component handles the logic for running johns tutorial

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once
#ifndef YTE_Gameplay_BasilTutorial_hpp
#define YTE_Gameplay_BasilTutorial_hpp

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/GameComponents/JohnDialogue.hpp"

namespace YTE
{
  class BasilTutorial : public Component
  {
  public:
    YTEDeclareType(BasilTutorial);
    BasilTutorial(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;

    void OnTutorialUpdate(TutorialUpdate *aEvent);
  private:
    Quest::CharacterName mName = Quest::CharacterName::Basil;
  };
}//end yte
#endif