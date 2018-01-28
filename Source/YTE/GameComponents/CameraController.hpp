/******************************************************************************/
/*!
\file   CameraController.hpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-01-27
\brief
This component controls the logic for the camera. Eventually this will handle
input from the controller as well as cinematic moves when starting dialogue
etc.

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include "YTE/Core/Component.hpp"
#include "YTE/Core/Engine.hpp"

namespace YTE
{
  class CameraController : public Component
  {
  public:
    YTEDeclareType(CameraController);
    CameraController(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;
  private:

  };
}//end yte namespace