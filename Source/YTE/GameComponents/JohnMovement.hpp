/******************************************************************************/
/*!
\file   JohnMovement.hpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-03-25
\brief
    This component handles the logic for moving john around the map

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once
#ifndef YTE_Gameplay_JohnMovement_hpp
#define YTE_Gameplay_JohnMovement_hpp

#include "YTE/Core/Composition.hpp"
#include "YTE/GameComponents/JohnDialogue.hpp"

namespace YTE
{
  class JohnMovement : public Component
  {
  public:
    YTEDeclareType(JohnMovement);
    JohnMovement(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;

    void OnQuestStart(QuestStart *aEvent);
  private:
    std::vector<glm::vec3> mDocks; // or however we hold onto dock positions
    int mDockIndex;
    glm::vec3 mStartPos;
  };
}//end yte
#endif