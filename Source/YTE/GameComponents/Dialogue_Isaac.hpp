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
  /////////////////////////////////////////////////////////////////////////////////////
  // Events
  /////////////////////////////////////////////////////////////////////////////////////
  YTEDeclareEvent(DialogueStart);

  class DialogueStart : public Event
  {
  public:
    YTEDeclareType(DialogueStart);
    //Composition *camera;
  };

  /////////////////////////////////////////////////////////////////////////////////////
  // Class
  /////////////////////////////////////////////////////////////////////////////////////
  class DialogueIsaac : public Component
  {
  public:

    YTEDeclareType(DialogueIsaac);
    DialogueIsaac(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;
    void Update(LogicUpdate *aEvent);

    bool mActive;

    void OnRequestDialogueStart(RequestDialogueStart *aEvent);
    void OnDialogueConfirm(DialogueConfirm *aEvent);
    void OnDialogueExit(DialogueExit *aEvent);
    void OnCollisionPersist(CollisionPersisted *aEvent);
    void OnCollisionStart(CollisionStarted *aEvent);
    void OnCollisionEnd(CollisionEnded *aEvent);

  private:
    glm::vec3 mDockAnchorPosition;
    //Composition *mSprite;

  };
}
