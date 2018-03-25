/******************************************************************************/
/*!
* \author Joshua T. Fisher
* \date   6/7/2015
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "YTE/Platform/TargetDefinitions.hpp"

#ifdef YTE_Windows

#include <stdint.h>
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>

#include "YTE/Platform/Windows/WindowsInclude_Windows.hpp"
#include <Winuser.h>

#include "YTE/Platform/Keyboard.hpp"

namespace YTE
{
  Keys TranslateFromOsToOurKey(uint64_t aOsKey)
  {
    switch (aOsKey)
    {
#define ProcessKey(aOsKey, aOurKey) case (aOsKey) : return Keys::aOurKey;
#include "YTE/Platform/Windows/OsKeys_Windows.hpp"
#undef ProcessKey
    default: return Keys::Unknown;
    }
  }

  uint64_t TranslateFromOurToOSKey(Keys aOurKey)
  {
    switch (aOurKey)
    {
#define ProcessKey(aOsKey, aOurKey) case (Keys::aOurKey) : return aOsKey;
#include "YTE/Platform/Windows/OsKeys_Windows.hpp"
#undef ProcessKey
    default: return 0;
    }
  }

  bool CheckKey(Keys aKey)
  {
    auto key = TranslateFromOurToOSKey(aKey);

    bool state = GetAsyncKeyState(static_cast<int>(key)) & 0x8000;

    return state;
  }


  // Reference: https://www.codeproject.com/Articles/42/All-you-ever-wanted-to-know-about-the-Clipboard
  std::string GetClipboardText()
  {
    std::string toReturn;

    //open the clipboard
    if (OpenClipboard(nullptr))
    {
      HANDLE hData = GetClipboardData(CF_UNICODETEXT);
      std::u16string wstr = (char16_t*)GlobalLock(hData);
      GlobalUnlock(hData);
      CloseClipboard();

      // UTF-16/char16_t to UTF-8
      std::string u8_conv = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(wstr);
    }

    return toReturn;
  }

  void SetClipboardText(std::string &aText)
  {
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>> converter;
    std::wstring wstr = converter.from_bytes(aText);

    if (OpenClipboard(nullptr))
    {
      HGLOBAL clipbuffer;
      char *buffer;
      EmptyClipboard();

      clipbuffer = GlobalAlloc(GMEM_DDESHARE, (sizeof(std::wstring::value_type) * wstr.size()) + 1);
      buffer = (char*)GlobalLock(clipbuffer);
      strcpy(buffer, LPCSTR(wstr.size()));
      GlobalUnlock(clipbuffer);
      SetClipboardData(CF_UNICODETEXT, clipbuffer);
      CloseClipboard();
    }
  }
}
#endif
