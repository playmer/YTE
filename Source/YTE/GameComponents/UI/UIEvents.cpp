/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/05/23
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/UI/UIEvents.hpp"

namespace YTE
{
  YTEDefineEvent(UIDisplayEvent);
  YTEDefineEvent(UIFocusEvent);
  YTEDefineEvent(UITriggerEvent);
  YTEDefineEvent(UIUpdateContentEvent);
  YTEDefineEvent(UIUpdateRegistrationEvent);

  YTEDefineType(UIDisplayEvent)
  {
    RegisterType<UIDisplayEvent>();
    TypeBuilder<UIDisplayEvent> builder;
  }

  YTEDefineType(UIFocusEvent)
  {
    RegisterType<UIFocusEvent>();
    TypeBuilder<UIFocusEvent> builder;
  }

  YTEDefineType(UITriggerEvent)
  {
    RegisterType<UITriggerEvent>();
    TypeBuilder<UITriggerEvent> builder;
  }

  YTEDefineType(UIUpdateContentEvent)
  {
    RegisterType<UIUpdateContentEvent>();
    TypeBuilder<UIUpdateContentEvent> builder;
  }

  YTEDefineType(UIUpdateRegistrationEvent)
  {
    RegisterType<UIUpdateRegistrationEvent>();
    TypeBuilder<UIUpdateRegistrationEvent> builder;
  }
}