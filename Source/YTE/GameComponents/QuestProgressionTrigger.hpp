/******************************************************************************/
/*!
\file   QuestProgressionTrigger.hpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-03-27
\brief
    This component acts as a trigger to advance the quest
    with QuestLogic on the player

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once
#ifndef YTE_Gameplay_QuestProgressionTrigger_hpp
#define YTE_Gameplay_QuestProgressionTrigger_hpp

#include "YTE/Core/Composition.hpp"
#include "YTE/Physics/BoxCollider.hpp"

namespace YTE
{
  class QuestProgressionTrigger : public Component
  {
  public:
    YTEDeclareType(QuestProgressionTrigger);
    QuestProgressionTrigger(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;

    bool IsCollisionTrigger() { return mIsCollisionTrigger; }
    void SetTriggerType(bool& aTriggerType) { mIsCollisionTrigger = aTriggerType; }

  private:
    bool mIsCollisionTrigger;
  };
}//end yte
#endif