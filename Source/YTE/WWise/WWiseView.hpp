// copyright All content 2016 DigiPen(USA) Corporation, all rights reserved.

#pragma once

#ifndef YTE_WWise_WWiseView_hpp
#define YTE_WWise_WWiseView_hpp

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/Engine.hpp"

#include "YTE/Platform/Keyboard.hpp"
#include "YTE/Platform/DeviceEnums.hpp"

#include "YTE/WWise/WWiseListener.hpp"

#include "YTE/Utilities/Utilities.hpp"

namespace YTE
{
  class WWiseListenerChanged : public Event
  {
  public:
    YTEDeclareType(WWiseListenerChanged);
    WWiseListener *NewListener;
  };

  YTEDeclareEvent(WWiseListenerChanged);

  class WWiseView : public Component
  {
  public:
    YTEDeclareType(WWiseView);
    WWiseView(Composition *aOwner, Space *aSpace, RSValue *aProperties)
      : Component(aOwner, aSpace)
    {
      UnusedArguments(aProperties);
    }

    WWiseListener* GetActiveListener()
    {
      return mLastActiveListener;
    }

    void SetActiveListener(WWiseListener *aListener)
    {
      mLastActiveListener = aListener;

      WWiseListenerChanged event;
      event.NewListener = aListener;

      SendEvent(Events::WWiseListenerChanged, &event);
    }

  private:
    WWiseListener *mLastActiveListener;
  };

} // end yte namespace

#endif