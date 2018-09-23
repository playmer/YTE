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
  YTEDefineEvent(KeyPress);
  YTEDefineEvent(KeyRelease);
  YTEDefineEvent(KeyPersist);
  YTEDefineEvent(CharacterTyped);

  YTEDefineType(KeyboardEvent)
  {
    RegisterType<KeyboardEvent>();
    TypeBuilder<KeyboardEvent> builder;
    builder.Field<&KeyboardEvent::Key>("Key", PropertyBinding::Get);
  }

  YTEDefineType(Keyboard)
  {
    RegisterType<Keyboard>();
    TypeBuilder<Keyboard> builder;
    builder.Function<&Keyboard::IsKeyPressed>("IsKeyPressed")
      .SetParameterNames("aKey")
      .SetDocumentation("Finds if the given button is pressed right now.");
    builder.Function<&Keyboard::IsKeyDown>("IsKeyDown")
      .SetParameterNames("aKey")
      .SetDocumentation("Finds if the given button is down right now.");
    builder.Function<&Keyboard::WasKeyDown>("WasKeyDown")
      .SetParameterNames("aKey")
      .SetDocumentation("Finds if the given button was down last frame.");
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

    for (size_t i = 0; i < EnumCast(Keys::Keys_Number); ++i)
    {
      if (mKeysCurrent[i] && mKeysPrevious[i])
      {
        keyEvent.Key = static_cast<Keys>(i);
        keyEvent.Keyboard = this;

        SendEvent(Events::KeyPersist, &keyEvent);
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

    SendEvent(*state, &keyboardEvent);
  }

  void Keyboard::ForceAllKeysUp()
  {
    for (size_t i = 0; i < EnumCast(Keys::Keys_Number); ++i)
    {
      UpdateKey(static_cast<Keys>(i), false);
    }
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