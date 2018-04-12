/******************************************************************************/
/*!
\file   DaisyMovement.hpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-03-25
\brief
    This component handles the logic for moving john around the map

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once
#ifndef YTE_Gameplay_DaisyMovement_hpp
#define YTE_Gameplay_DaisyMovement_hpp

#include "YTE/Core/Composition.hpp"
#include "YTE/GameComponents/JohnDialogue.hpp"

namespace YTE
{
  class DaisyMovement : public Component
  {
  public:
    YTEDeclareType(DaisyMovement);
    DaisyMovement(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;

    void OnQuestStart(QuestStart *aEvent);
    void MoveToNextDock();
  private:
    std::vector<glm::vec3> mDocks;
    glm::vec3 mStartPos;
    int mDockIndex;
  };
}//end yte
#endif