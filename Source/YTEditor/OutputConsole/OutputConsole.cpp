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

#include "YTE/Platform/TargetDefinitions.hpp"
#include "YTE/StandardLibrary/Utilities.hpp"
#include "YTE/Utilities/Utilities.h"

#include "OutputConsole.hpp"

OutputConsole::OutputConsole(QWidget *aParent)
  : QWidget(aParent), mLayout(nullptr), mConsole(nullptr)
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

QColor OutputConsole::Color::Black{ 0,0,0 };
QColor OutputConsole::Color::Red{ 255,0,0 };
QColor OutputConsole::Color::Blue{ 0,0,255 };
QColor OutputConsole::Color::Green{ 0,255,0 };

void OutputConsole::PrintLnC(QColor aColor,const char *aFormatString, ...)
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
