/******************************************************************************/
/*!
\file   CameraController.cpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-01-27

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/CameraController.hpp"

namespace YTE
{
  YTEDefineType(CameraController)
  {
    YTERegisterType(CameraController);
  }

  CameraController::CameraController(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    YTEUnusedArgument(aProperties);
    //will probably have some props
  }

  void CameraController::Initialize()
  {

  }
}// end yte namespace