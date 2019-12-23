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
    YTEditor_Shared LogHandler(OutputConsole* aConsole);

    YTEditor_Shared void HandleLog(YTE::LogEvent* aEvent);

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

    YTEditor_Shared OutputConsole(YTELevelEditor* aWorkspace);
    YTEditor_Shared ~OutputConsole();

    YTEditor_Shared void PrintToConsole(const char *aString);

    YTEditor_Shared void PrintLn(const char *aFormat, ...);
    YTEditor_Shared void PrintLnC(QColor aColor, const char *aFormat, ...);

    QTextEdit* GetTextEdit() { return mConsole; }

    YTEditorMainWindow* GetMainWindow() { return mMainWindow; }

    YTEditor_Shared static std::string GetName();

    YTEditor_Shared ToolWindowManager::AreaReference GetToolArea() override;

  private:
    void SetWindowSettings();
    void ConstructInnerWidget();

    QGridLayout* mLayout;
    QTextEdit* mConsole;
    YTEditorMainWindow* mMainWindow;
    LogHandler mHandler;
  };
}