/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/05/23
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include "YTE/Core/Component.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/GameComponents/UI/UIEvents.hpp"

namespace YTE
{
  class UIController : public Component
  {
    enum UIEvents { Display, Focus, Trigger, UpdateContent, Count };

  public:
    YTEDeclareType(UIController);
    UIController(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void Initialize() override;
    void Start() override;

    // Properties /////////////////////////////////////////////////////////
    bool GetDisplayRegistered() { return mRegisteredEvents[UIEvents::Display]; }
    bool GetFocusRegistered() { return mRegisteredEvents[UIEvents::Focus]; }
    bool GetTriggerRegistered() { return mRegisteredEvents[UIEvents::Trigger]; }
    bool GetUpdateRegistered() { return mRegisteredEvents[UIEvents::UpdateContent]; }

    void SetDisplayRegistered(bool aShouldRegister);
    void SetFocusRegistered(bool aShouldRegister);
    void SetTriggerRegistered(bool aShouldRegister);
    void SetUpdateRegistered(bool aShouldRegister);
    ///////////////////////////////////////////////////////////////////////

    void UpdateEventRegistrations(UIUpdateRegistrationEvent *aEvent);

    void OnUIDisplayEvent(UIDisplayEvent *aEvent);
    void OnUIFocusEvent(UIFocusEvent *aEvent);
    void OnUITriggerEvent(UITriggerEvent *aEvent);
    void OnUIUpdateContentEvent(UIUpdateContentEvent *aEvent);

  private:
    bool mRegisteredEvents[4];
  };
}
