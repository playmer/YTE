/******************************************************************************/
/*!
\file   QuestLogic.hpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-03-25
\brief
    This component handles the logic for changing the quest states

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once
#ifndef YTE_Gameplay_QuestLogic_hpp
#define YTE_Gameplay_QuestLogic_hpp

#include "YTE/Core/Composition.hpp"
#include "YTE/Physics/BoxCollider.hpp"
#include "YTE/GameComponents/NoticeBoard.hpp"

namespace YTE
{
  class QuestLogic : public Component
  {
  public:
    YTEDeclareType(QuestLogic);
    QuestLogic(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;

    void HookupPostcardHandle(Postcard **aHandle) { mPostcardHandle = aHandle; };
    Postcard *GetPostcard() { return *mPostcardHandle; };

    void OnCollisionStarted(CollisionStarted *aEvent);
  private:
    Postcard **mPostcardHandle;
  };
}//end yte
#endif