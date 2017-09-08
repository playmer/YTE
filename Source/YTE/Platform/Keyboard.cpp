/******************************************************************************/
/*!
 * \author Joshua T. Fisher
 * \date   2015-6-7
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
 */
/******************************************************************************/


#include "YTE/Platform/Keyboard.hpp"

namespace YTE
{
  DefineEvent(KeyPress);
  DefineEvent(KeyRelease);
  DefineEvent(KeyPersist);

  DefineType(Keyboard)
  {
    YTERegisterType(Keyboard);
    YTEAddFunction( &Keyboard::IsKeyPressed, YTENoOverload, "IsKeyPressed", YTEParameterNames("aKey"))->Description()
      = "Finds if the given button is pressed right now.";
    YTEAddFunction( &Keyboard::IsKeyDown, YTENoOverload, "IsKeyDown", YTEParameterNames("aKey"))->Description()
      = "Finds if the given button is down right now.";
    YTEAddFunction( &Keyboard::WasKeyDown, YTENoOverload, "WasKeyDown", YTEParameterNames("aKey"))->Description()
      = "Finds if the given button was down last frame.";
  }

  Keyboard::Keyboard()
  {
    std::memset(mArrayOne, 0, sizeof(mArrayOne));
    std::memset(mArrayTwo, 0, sizeof(mArrayTwo));

    mKeysCurrent = mArrayOne;
    mKeysPrevious = mArrayTwo;
  }

  void Keyboard::Update()
  {
    KeyboardEvent keyEvent;

    for (size_t i = 0; i < static_cast<size_t>(Keys::Keys_Number); ++i)
    {
      if (mKeysCurrent[i] && mKeysPrevious[i])
      {
        keyEvent.Key = static_cast<Keys>(i);
        keyEvent.Keyboard = this;

        Trigger(Events::KeyPersist, &keyEvent);
      }

      mKeysPrevious[i] = mKeysCurrent[i];
    }
  }

  void Keyboard::UpdateKey(Keys aKey, bool aDown)
  {
    size_t index = static_cast<size_t>(aKey);
        
      // Key got resent.
    if (mKeysCurrent[index] == aDown)
    {
      return;
    }
      
    mKeysPrevious[index] = mKeysCurrent[index];

    mKeysCurrent[index] = aDown;

    const std::string *state;

      // Key has been pressed.
    if (aDown)
    {
      state = &Events::KeyPress;
    }
      // Key has been released
    else
    {
      state = &Events::KeyRelease;
    }
      
    KeyboardEvent keyboardEvent;
    keyboardEvent.Key = aKey;
    keyboardEvent.Keyboard = this;
      
    Trigger(*state, &keyboardEvent);
  }

  bool Keyboard::IsKeyPressed(Keys aKey)
  {
    bool isKeyDown = IsKeyDown(aKey);
    bool WasKeyNotDown = !WasKeyDown(aKey);

    return isKeyDown && WasKeyNotDown;
  }

  bool Keyboard::IsKeyDown(Keys aKey)
  {
    return mKeysCurrent[static_cast<int>(aKey)];
  }
  bool Keyboard::WasKeyDown(Keys aKey)
  {
    return mKeysPrevious[static_cast<int>(aKey)];
  }
}
