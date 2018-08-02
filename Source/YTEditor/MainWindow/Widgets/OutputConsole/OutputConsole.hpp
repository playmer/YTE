/******************************************************************************/
/*!
\file   OutputConsole.hpp
\author Joshua Fisher, Nicholas Ammann
\par    email: j.fisher\@digipen.edu, nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
The debug output console widget contained by the main window.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include <qwidget.h>

#include "YTE/Core/Engine.hpp"
#include "YTE/Core/EventHandler.hpp"
#include "YTEditor/MainWindow/Widgets/Widget.hpp"

#include "YTEditor/YTEditorMeta.hpp"

class QGridLayout;
class QTextEdit;

namespace YTEditor
{
  class MainWindow;
  class OutputConsole; 

  class LogHandler : public YTE::EventHandler
  {
  public:
    YTEditorDeclareType(LogHandler);
    LogHandler(OutputConsole *aConsole);

    void HandleLog(YTE::LogEvent *aEvent);

  private:
    OutputConsole *mConsole;
  };

  class OutputConsole : public Widget
  {
  public:

    struct Color
    {
      static QColor White;
      static QColor Black;
      static QColor Blue;
      static QColor Green;
      static QColor Yellow;
      static QColor Orange;
      static QColor Red;
    };

    OutputConsole(MainWindow *aMainWindow);
    ~OutputConsole();

    void PrintToConsole(const char *aString);

    void PrintLn(const char *aFormat, ...);
    void PrintLnC(QColor aColor, const char *aFormat, ...);

    QTextEdit* GetTextEdit() { return mConsole; }

    MainWindow* GetMainWindow() { return mMainWindow; }

    static std::string GetName();

    Widget::DockArea GetDefaultDockPosition() const override;

  private:
    void SetWindowSettings();
    void ConstructInnerWidget();

    QGridLayout *mLayout;
    QTextEdit *mConsole;
    MainWindow *mMainWindow;
    LogHandler mHandler;
  };
}