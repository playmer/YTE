/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/05/23
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/EventHandler.hpp"

namespace YTE
{
  /////////////////////////////////////////////////////////////////////////////////////
  // Events
  /////////////////////////////////////////////////////////////////////////////////////
  YTEDeclareEvent(UIDisplayEvent);
  YTEDeclareEvent(UIFocusEvent);
  YTEDeclareEvent(UITriggerEvent);
  YTEDeclareEvent(UIUpdateContentEvent);
  YTEDeclareEvent(UIUpdateRegistrationEvent);

    // Sent when a UI element's visibility should be changed (for example, to contextually show/hide some UI element)
  class UIDisplayEvent : public Event
  {
  public:
    YTEDeclareType(UIDisplayEvent);

    UIDisplayEvent(bool aShouldDisplay) { ShouldDisplay = aShouldDisplay; }

    bool ShouldDisplay;
  };

    // Sent when a UI element goes in or out of input focus (for example, hovering on a button with the mouse)
  class UIFocusEvent : public Event
  {
  public:
    YTEDeclareType(UIFocusEvent);

    UIFocusEvent(bool aIsInFocus) { IsInFocus = aIsInFocus; }

    bool IsInFocus;
  };

    // Sent when a UI element should trigger some other behavior (for example, clicking a UI button to launch a new menu)
  class UITriggerEvent : public Event
  {
  public:
    YTEDeclareType(UITriggerEvent);

    UITriggerEvent(bool aIsTriggered) { IsTriggered = aIsTriggered; }

    bool IsTriggered;
  };

    // Sent when the content of a UI element should be changed (for example, to decrease a HUD's representation of the player health)
  class UIUpdateContentEvent : public Event
  {
  public:
    YTEDeclareType(UIUpdateContentEvent);

    UIUpdateContentEvent(const std::string& aContent) { Content = aContent; }

      // Only support strings for now (it seems like those would be the only types used for a HUD?)
      // May need to be changed depending on future demands
    std::string Content;
  };

    // Sent to update which events a UI element should listen for (for example, disabling the options menu from responding while in the main menu)
    // Can be thought of as a meta-event in comparison to the basic 4
  class UIUpdateRegistrationEvent : public Event
  {
  public:
    YTEDeclareType(UIUpdateRegistrationEvent);

    bool DisplayEvent = false;
    bool FocusEvent = false;
    bool TriggerEvent = false;
    bool UpdateContentEvent = false;
  };
}
