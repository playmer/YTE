/******************************************************************************/
/*!
* \author Joshua T. Fisher
* \date   6/7/2015
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "YTE/Platform/TargetDefinitions.hpp"

#include <stdint.h>
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>

#include "YTE/Platform/Keyboard.hpp"

namespace YTE
{
  Keys TranslateFromOsToOurKey(uint64_t aOsKey)
  {
    return Keys::Unknown;
  }

  uint64_t TranslateFromOurToOSKey(Keys aOurKey)
  {
    return 0;
  }

  bool CheckKey(Keys aKey)
  {
    return false;
  }

  // Reference: https://www.codeproject.com/Articles/42/All-you-ever-wanted-to-know-about-the-Clipboard
  std::string GetClipboardText()
  {
    return std::string{};
  }


  void SetClipboardText(std::string &aText)
  {
    
  }
}
