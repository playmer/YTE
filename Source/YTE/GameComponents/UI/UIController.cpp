/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/05/23
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/Core/Space.hpp"
#include "YTE/GameComponents/UI/UIEvents.hpp"

#include "YTE/GameComponents/UI/UIController.hpp"

namespace YTE
{
  YTEDefineType(UIController)
  {
    RegisterType<UIController>();
    TypeBuilder<UIController> builder;

    builder.Property<&GetDisplayRegistered, &SetDisplayRegistered>("DisplayEventRegistered")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .SetDocumentation("If set, this component registers for UIDisplayEvents");

    builder.Property<&GetFocusRegistered, &SetFocusRegistered>("FocusEventRegistered")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .SetDocumentation("If set, this component registers for UIFocusEvents");

    builder.Property<&GetTriggerRegistered, &SetTriggerRegistered>("TriggerEventRegistered")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .SetDocumentation("If set, this component registers for UITriggerEvents");

    builder.Property<&GetUpdateRegistered, &SetUpdateRegistered>("UpdateContentEventRegistered")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .SetDocumentation("If set, this component registers for UIUpdateContentEvents");
  }

  UIController::UIController(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mRegisteredEvents{ false }
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void UIController::Initialize()
  {
    if (mRegisteredEvents[UIEvents::Display])
    {
      mSpace->RegisterEvent<&UIController::OnUIDisplayEvent>(Events::UIDisplayEvent, this);
    }
    if (mRegisteredEvents[UIEvents::Focus])
    {
      mSpace->RegisterEvent<&UIController::OnUIFocusEvent>(Events::UIFocusEvent, this);
    }
    if (mRegisteredEvents[UIEvents::Trigger])
    {
      mSpace->RegisterEvent<&UIController::OnUITriggerEvent>(Events::UITriggerEvent, this);
    }
    if (mRegisteredEvents[UIEvents::UpdateContent])
    {
      mSpace->RegisterEvent<&UIController::OnUIUpdateContentEvent>(Events::UIUpdateContentEvent, this);
    }

    mOwner->RegisterEvent<&UIController::UpdateEventRegistrations>(Events::UIUpdateRegistrationEvent, this);
  }

  void UIController::Start()
  {

  }

  // Event Callbacks /////////////////////////////////////////////////////////////////
  void UIController::UpdateEventRegistrations(UIUpdateRegistrationEvent *aEvent)
  {
    if (aEvent->DisplayEvent != mRegisteredEvents[UIEvents::Display])
    {
      SetDisplayRegistered(aEvent->DisplayEvent);
    }
    if (aEvent->FocusEvent != mRegisteredEvents[UIEvents::Focus])
    {
      SetFocusRegistered(aEvent->FocusEvent);
    }
    if (aEvent->TriggerEvent != mRegisteredEvents[UIEvents::Trigger])
    {
      SetTriggerRegistered(aEvent->TriggerEvent);
    }
    if (aEvent->UpdateContentEvent != mRegisteredEvents[UIEvents::UpdateContent])
    {
      SetUpdateRegistered(aEvent->UpdateContentEvent);
    }
  }

  void UIController::OnUIDisplayEvent(UIDisplayEvent *aEvent)
  {

  }

  void UIController::OnUIFocusEvent(UIFocusEvent *aEvent)
  {

  }

  void UIController::OnUITriggerEvent(UITriggerEvent *aEvent)
  {

  }

  void UIController::OnUIUpdateContentEvent(UIUpdateContentEvent *aEvent)
  {

  }

  // Properties /////////////////////////////////////////////////////////////////////
  void UIController::SetDisplayRegistered(bool aShouldRegister)
  {
    mRegisteredEvents[UIEvents::Display] = aShouldRegister;

    if (aShouldRegister)
    {
      mSpace->RegisterEvent<&UIController::OnUIDisplayEvent>(Events::UIDisplayEvent, this);
    }
    else
    {
      mSpace->DeregisterEvent<&UIController::OnUIDisplayEvent>(Events::UIDisplayEvent, this);
    }
  }

  void UIController::SetFocusRegistered(bool aShouldRegister)
  {
    mRegisteredEvents[UIEvents::Focus] = aShouldRegister;

    if (aShouldRegister)
    {
      mSpace->RegisterEvent<&UIController::OnUIFocusEvent>(Events::UIFocusEvent, this);
    }
    else
    {
      mSpace->DeregisterEvent<&UIController::OnUIFocusEvent>(Events::UIFocusEvent, this);
    }
  }

  void UIController::SetTriggerRegistered(bool aShouldRegister)
  {
    mRegisteredEvents[UIEvents::Trigger] = aShouldRegister;
    
    if (aShouldRegister)
    {
      mSpace->RegisterEvent<&UIController::OnUITriggerEvent>(Events::UITriggerEvent, this);
    }
    else
    {
      mSpace->DeregisterEvent<&UIController::OnUITriggerEvent>(Events::UITriggerEvent, this);
    }
  }

  void UIController::SetUpdateRegistered(bool aShouldRegister)
  {
    mRegisteredEvents[UIEvents::UpdateContent] = aShouldRegister;
    
    if (aShouldRegister)
    {
      mSpace->RegisterEvent<&UIController::OnUIUpdateContentEvent>(Events::UIUpdateContentEvent, this);
    }
    else
    {
      mSpace->DeregisterEvent<&UIController::OnUIUpdateContentEvent>(Events::UIUpdateContentEvent, this);
    }
  }
}