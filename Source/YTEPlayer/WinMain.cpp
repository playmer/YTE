/******************************************************************************/
/*!
 * \author Joshua T. Fisher
 * \date   2015-10-26
 *
 * \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
 */
/******************************************************************************/
#ifdef NDEBUG
#include "YTE/Platform/Windows/WindowsInclude_Windows.hpp"

int main();

#pragma warning( push )
#pragma warning( disable : 28252 )
#pragma warning( disable : 28253 )
int CALLBACK WinMain(_In_ HINSTANCE, _In_ HINSTANCE, _In_ LPSTR, _In_ int)
{
  return main();
}
#pragma warning( pop ) 

#endif