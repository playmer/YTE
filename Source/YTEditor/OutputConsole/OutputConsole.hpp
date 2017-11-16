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

class QGridLayout;
class QTextEdit;

namespace YTEditor
{

  class OutputConsole : public QWidget
  {
  public:

    struct Color
    {
      static QColor Black;
      static QColor Red;
      static QColor Blue;
      static QColor Green;
    };

    OutputConsole(QWidget * aParent = nullptr);
    ~OutputConsole();

    void PrintToConsole(const char * aString);

    void PrintLn(const char *aFormat, ...);
    void PrintLnC(QColor aColor, const char *aFormat, ...);

  private:

    void SetWindowSettings();
    void ConstructInnerWidget();

    QGridLayout *mLayout;
    QTextEdit *mConsole;

  };
}