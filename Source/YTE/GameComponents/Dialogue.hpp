/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/03/15
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/EventHandler.hpp"

#include "YTE/GameComponents/InputInterpreter.hpp"
#include "YTE/GameComponents/BoatController.hpp"

#include "YTE/Graphics/Camera.hpp"


namespace YTE
{
  class Dialogue : public Component
  {
  public:

    YTEDeclareType(Dialogue);
    Dialogue(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;
    void Update(LogicUpdate *aEvent);

    bool mActive;

    void OnCollisionPersist(CollisionPersisted *aEvent);
    void OnCollisionStart(CollisionStarted *aEvent);
    void OnCollisionEnd(CollisionEnded *aEvent);

  private:

    void OnDialogueStart(RequestDialogueStart *aEvent);
    void OnConfirm(DialogueConfirm *aEvent);
    void OnExit(DialogueExit *aEvent);

    Composition *mSprite;

  };
} // End YTE namespace