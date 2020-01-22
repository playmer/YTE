/******************************************************************************/
/*!
* \author Joshua T. Fisher
* \date   6/7/2015
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include <stdarg.h>
#include <stdio.h>

#include "YTE/Core/Utilities.hpp"

#include "YTE/Platform/DialogBox.hpp"
#include "YTE/Platform/Window.hpp"

namespace YTE
{

  DebugErrorDialog CreateDebugErrorDialog(const char *aFormatString, ...)
  {
    return DebugErrorDialog::Continue;
  }
}
