#include <stdarg.h>
#include <stdio.h>

#include "SDL.h"

#include "YTE/Core/Utilities.hpp"

#include "YTE/Platform/DialogBox.hpp"
#include "YTE/Platform/Window.hpp"

namespace YTE
{
#ifdef YTE_Windows
  static int vasprintf(char **strPtr, const char *aFormatString, va_list argv)
  {
    int needed = vsnprintf((*strPtr = nullptr), 0, aFormatString, argv);

    if ((needed < 0) || ((*strPtr = (char *)malloc(1 + needed)) == nullptr))
    {
      return -1;
    }

    return vsprintf_s(*strPtr, 1 + needed, aFormatString, argv);
  }
#endif


  enum DebugErrorDialogSDL
  {
    Continue = 0,
    DebugBreak,
    Abort,
    ErrorOccured
  };

  DebugErrorDialog CreateDebugErrorDialog(const char *aFormatString, ...)
  {
    va_list variadicArguments;
    va_start(variadicArguments, aFormatString);

    char *stringBuffer;
    vasprintf(&stringBuffer, aFormatString, variadicArguments);

    const SDL_MessageBoxButtonData buttons[] = {
        { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, "Continue" },
        {                                       0, 1, "Break Into Debugger" },
        { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 2, "Abort" },
    };

    const SDL_MessageBoxColorScheme colorScheme = {
        { /* .colors (.r, .g, .b) */
            /* [SDL_MESSAGEBOX_COLOR_BACKGROUND] */
            { 255,   0,   0 },
            /* [SDL_MESSAGEBOX_COLOR_TEXT] */
            {   0, 255,   0 },
            /* [SDL_MESSAGEBOX_COLOR_BUTTON_BORDER] */
            { 255, 255,   0 },
            /* [SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND] */
            {   0,   0, 255 },
            /* [SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED] */
            { 255,   0, 255 }
        }
    };

    const SDL_MessageBoxData messageboxdata = {
        SDL_MESSAGEBOX_INFORMATION, /* .flags */
        NULL, /* .window */
        "Debug Assertion", /* .title */
        stringBuffer, /* .message */
        SDL_arraysize(buttons), /* .numbuttons */
        buttons, /* .buttons */
        &colorScheme /* .colorScheme */
    };

    int buttonid;

    if (SDL_ShowMessageBox(&messageboxdata, &buttonid) < 0)
    {
      SDL_Log("error displaying message box");
      free(stringBuffer);
      return DebugErrorDialog::ErrorOccured;
    }

    switch (buttonid)
    {
      case DebugErrorDialogSDL::Continue: return DebugErrorDialog::Continue;
      case DebugErrorDialogSDL::DebugBreak: return DebugErrorDialog::DebugBreak;
      case DebugErrorDialogSDL::Abort: return DebugErrorDialog::Abort;
    }

    free(stringBuffer);
    return DebugErrorDialog::Continue;
  }
}
