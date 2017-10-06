/******************************************************************************/
/*!
* \author Joshua T. Fisher
* \date   6/7/2015
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include <stdarg.h> /* va_list, va_start, va_end*/
#include <stdio.h>

#include "YTE/Core/Utilities.hpp"

#include "YTE/Platform/Windows/WindowsInclude_Windows.hpp"

#include "YTE/Utilities/String/String.h"

#include "YTE/Platform/DialogBox.hpp"
#include "YTE/Platform/Window.hpp"

namespace YTE
{
  HWND GetRootWindow()
  {
    HWND window = GetActiveWindow();
    HWND parent = NULL;
    do
    {
      parent = GetParent(window);

      if (parent != NULL)
      {
        window = parent;
      }
    } while (parent != NULL);

    return window;
  }

  class DebugDialog
  {
  public:
    DebugDialog() : mDone(false) {};

    void SetDone(bool aDone) { mDone = aDone; }
    bool Done() { return mDone; }
    void SetCommand(DebugErrorDialog aCommand) { mCommand = aCommand; }
    DebugErrorDialog Command() { return mCommand; }

  private:
    DebugErrorDialog mCommand;
    bool mDone;
  };

  inline DebugDialog *GetDialogPointer(HWND aWindowHandle)
  {
    return reinterpret_cast<DebugDialog *>(GetWindowLongPtr(aWindowHandle, GWLP_USERDATA));
  }

  inline void SetDialogPointer(HWND aWindowHandle, DebugDialog *aDialog)
  {
    SetWindowLongPtr(aWindowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(aDialog));
  }


  static LRESULT CALLBACK WindowsMessagePump(HWND aWindowHandle, UINT aMessage, WPARAM aWParam, LPARAM aLParam)
  {
    DebugDialog *dialog = GetDialogPointer(aWindowHandle);

    switch (aMessage)
    {
        // Window Creation
      case WM_CREATE:
      {
        // This aLParam is our Window pointer
        DebugDialog* dialogToSet = (DebugDialog*)((CREATESTRUCT*)(aLParam))->lpCreateParams;

        // Set it on the user data section of the window
        SetDialogPointer(aWindowHandle, dialogToSet);
        break;
      }
      case WM_COMMAND:
      {
        if (110 <= (u64)LOWORD(aWParam) && (u64)LOWORD(aWParam) <= 112)
        {
          dialog->SetCommand((DebugErrorDialog)LOWORD(aWParam));
          dialog->SetDone(true);
        }
        break;
      }
    }
    // Probably want to only do this sometimes not every time we pump.
    return DefWindowProc(aWindowHandle, aMessage, aWParam, aLParam);
  }

  #define IDC_MAIN_EDIT	102			// Edit box identifier


#ifdef Windows
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


  DebugErrorDialog CreateDebugErrorDialog(const char *aFormatString, ...)
  {
    static bool doOnce = false;
    static WNDCLASS windowsData;

    if (doOnce == false)
    {
      doOnce = true;

      windowsData.style = CS_HREDRAW | CS_VREDRAW;
      windowsData.lpfnWndProc = WindowsMessagePump;
      windowsData.cbClsExtra = 0;
      windowsData.cbWndExtra = 0;
      windowsData.hInstance = GetModuleHandle(nullptr);
      windowsData.hIcon = LoadIcon(NULL, IDI_APPLICATION);
      windowsData.hCursor = LoadCursor(NULL, IDC_ARROW);
      windowsData.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
      windowsData.lpszMenuName = TEXT("Debug Assertion");
      windowsData.lpszClassName = TEXT("Debug Assertion");


      RegisterClass(&windowsData);
    }


    va_list variadicArguments;
    va_start(variadicArguments, aFormatString);

    char *stringBuffer;
    vasprintf(&stringBuffer, aFormatString, variadicArguments);


    DebugDialog dialog;

    HWND mainDebugWindow = CreateWindow(windowsData.lpszClassName, windowsData.lpszMenuName,
                                        WS_POPUP | WS_VISIBLE || WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                                        CW_USEDEFAULT, CW_USEDEFAULT, 600, 210,
                                        GetRootWindow(), NULL, windowsData.hInstance, &dialog);


    CreateWindow("BUTTON", "Continue",
                 WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                 4, 140, 150, 24,
                 mainDebugWindow, (HMENU)DebugErrorDialog::Continue, windowsData.hInstance, 0);

    CreateWindow("BUTTON", "Debug Break",
                 WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                 225, 140, 150, 24,
                 mainDebugWindow, (HMENU)DebugErrorDialog::DebugBreak, windowsData.hInstance, 0);

    CreateWindow("BUTTON", "Abort",
                 WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                 430, 140, 150, 24,
                 mainDebugWindow, (HMENU)DebugErrorDialog::Abort, windowsData.hInstance, 0);

    HWND hEdit = CreateWindow("EDIT", "",
                              WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY,
                              4, 4, 576, 120,
                              mainDebugWindow, (HMENU)IDC_MAIN_EDIT, windowsData.hInstance, 0);

    SendMessage(hEdit, WM_SETTEXT, NULL, (LPARAM)stringBuffer);

    SCROLLINFO scroll;
    scroll.cbSize = sizeof(SCROLLINFO);
    scroll.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
    scroll.nPage = 0;
    scroll.nMin = 0;
    scroll.nMax = 500;
    scroll.nPos = 0;
    SetScrollInfo(hEdit, SB_VERT, &scroll, TRUE);


    ShowWindow(mainDebugWindow, SW_SHOWNORMAL);
    UpdateWindow(mainDebugWindow);

    bool previous = SetCursorView(true);

    MSG message;
    while (PeekMessage(&message, mainDebugWindow, 0, 0, PM_REMOVE) || true)
    {
      TranslateMessage(&message);
      
      DispatchMessage(&message);

      if (dialog.Done())
      {
        DestroyWindow(mainDebugWindow);
        SetCursorView(previous);

        free(stringBuffer);
        return dialog.Command();
      }
    }

    free(stringBuffer);

    SetCursorView(previous);
    return DebugErrorDialog::Continue;
  }
}
