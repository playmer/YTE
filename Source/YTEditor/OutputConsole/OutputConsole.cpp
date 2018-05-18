/******************************************************************************/
/*!
\file   OutputConsole.cpp
\author Joshua Fisher
\par    email: j.fisher\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
Implementation of the output console and its print functions.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include <stdarg.h> /* va_list, va_start, va_end*/
#include <stdio.h>

#include <qgridlayout.h>
#include <qtextedit.h>

#include "YTE/Core/Engine.hpp"
#include "YTE/Platform/TargetDefinitions.hpp"
#include "YTE/StandardLibrary/Utilities.hpp"
#include "YTE/Utilities/Utilities.hpp"

#include "YTEditor/MainWindow/MainWindow.hpp"
#include "YTEditor/OutputConsole/OutputConsole.hpp"

namespace YTEditor
{
  YTEDefineType(LogHandler)
  {
    YTE::RegisterType<LogHandler>();
  }

  LogHandler::LogHandler(OutputConsole *aConsole)
    : mConsole(aConsole)
  {
    auto engine = mConsole->GetMainWindow()->GetRunningEngine();

    engine->RegisterEvent<&LogHandler::HandleLog>(YTE::Events::LogEvent, this);
  }

  static QColor LogTypeToColor(YTE::LogType aType)
  {
    switch (aType)
    {
      case YTE::LogType::Success: return OutputConsole::Color::Green;
      case YTE::LogType::Information: return OutputConsole::Color::White;
      case YTE::LogType::Warning: return OutputConsole::Color::Yellow;
      case YTE::LogType::PerformanceWarning: return OutputConsole::Color::Orange;
      case YTE::LogType::Error: return OutputConsole::Color::Red;
      default: return OutputConsole::Color::White;
    }
  }

  void LogHandler::HandleLog(YTE::LogEvent *aEvent)
  {
    auto color = LogTypeToColor(aEvent->Type);
    auto textEdit = mConsole->GetTextEdit();
    textEdit->setTextColor(color);

    QString str = QString::fromUtf8(aEvent->Log.data(), static_cast<int>(aEvent->Log.size()));
    textEdit->append(str);
    
    textEdit->setTextColor(OutputConsole::Color::Black);
  }

  OutputConsole::OutputConsole(MainWindow *aMainWindow, QWidget *aParent)
    : QWidget(aParent)
    , mLayout(nullptr)
    , mConsole(nullptr)
    , mMainWindow(aMainWindow)
    , mHandler(this)
  {
    SetWindowSettings();
    ConstructInnerWidget();
  }

  OutputConsole::~OutputConsole()
  {
  }

  void OutputConsole::PrintToConsole(const char *aString)
  {
    mConsole->append(aString);
  }

  void OutputConsole::PrintLn(const char *aFormatString, ...)
  {
    va_list variadicArguments;
    va_start(variadicArguments, aFormatString);

    auto formatted = YTE::Format(aFormatString, variadicArguments);

    PrintToConsole(formatted.c_str());
  }


  static QColor White;
  static QColor Black;
  static QColor Blue;
  static QColor Green;
  static QColor Yellow;
  static QColor Orange;
  static QColor Red;

  QColor OutputConsole::Color::White{ 255, 255, 255 };
  QColor OutputConsole::Color::Black{ 0, 0, 0 };
  QColor OutputConsole::Color::Blue{ 0, 0, 255 };
  QColor OutputConsole::Color::Green{ 0, 255, 0 };
  QColor OutputConsole::Color::Yellow{ 255, 255, 0 };
  QColor OutputConsole::Color::Orange{ 255, 128, 0 };
  QColor OutputConsole::Color::Red{ 255, 0, 0 };

  void OutputConsole::PrintLnC(QColor aColor, const char *aFormatString, ...)
  {
    mConsole->setTextColor(aColor);

    va_list variadicArguments;
    va_start(variadicArguments, aFormatString);

    auto formatted = YTE::Format(aFormatString, variadicArguments);

    PrintToConsole(formatted.c_str());

    mConsole->setTextColor(Color::Black);
  }


  void OutputConsole::SetWindowSettings()
  {
    setMinimumHeight(100);
  }

  void OutputConsole::ConstructInnerWidget()
  {
    mLayout = new QGridLayout(this);
    mConsole = new QTextEdit();;

    mConsole->setReadOnly(true);
    mLayout->addWidget(mConsole);
  }

}
