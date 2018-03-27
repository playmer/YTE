/******************************************************************************/
/*!
\file   QuestLogic.hpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-03-25
\brief
    This component handles the logic for changing the dialogues and quest states

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once
#ifndef YTE_Gameplay_QuestLogic_hpp
#define YTE_Gameplay_QuestLogic_hpp

#include "YTE/Core/Composition.hpp"

namespace YTE
{
  class QuestLogic : public Component
  {
  public:
    YTEDeclareType(QuestLogic);
    QuestLogic(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;
  private:
  };
}//end yte
#endif