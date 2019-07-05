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

#include <QWidget>
#include <QGridLayout>
#include <QTextEdit>

#include "YTE/Core/Engine.hpp"
#include "YTE/Core/EventHandler.hpp"

#include "YTEditor/Framework/ForwardDeclarations.hpp"
#include "YTEditor/Framework/Widget.hpp"


#include "YTEditor/YTELevelEditor/ForwardDeclarations.hpp"

#include "YTEditor/YTEditorMeta.hpp"

namespace YTEditor
{
  class LogHandler : public YTE::EventHandler
  {
  public:
    YTEditorDeclareType(LogHandler);
    LogHandler(OutputConsole* aConsole);

    void HandleLog(YTE::LogEvent *aEvent);

  private:
    OutputConsole* mConsole;
  };

  class OutputConsole : public Framework::Widget<QWidget>
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

    OutputConsole(YTELevelEditor* aWorkspace);
    ~OutputConsole();

    void PrintToConsole(const char *aString);

    void PrintLn(const char *aFormat, ...);
    void PrintLnC(QColor aColor, const char *aFormat, ...);

    QTextEdit* GetTextEdit() { return mConsole; }

    YTEditorMainWindow* GetMainWindow() { return mMainWindow; }

    static std::string GetName();

    ToolWindowManager::AreaReference GetToolArea() override;

  private:
    void SetWindowSettings();
    void ConstructInnerWidget();

    QGridLayout* mLayout;
    QTextEdit* mConsole;
    YTEditorMainWindow* mMainWindow;
    LogHandler mHandler;
  };
}