/******************************************************************************/
/*!
\file   AddComponentToEngine.hpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-01-27
\brief
This component put on an object in the level will add the listed components to
the engine. This is primarily so that the InputInterpreter can have access to 
multiple spaces and there currently isnt a way for components to be added to
the engine in the editor.

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once
#ifndef YTE_Gameplay_AddComponentToEngine_hpp
#define YTE_Gameplay_AddComponentToEngine_hpp

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/Engine.hpp"

namespace YTE
{
  class AddComponentToEngine : public Component
  {
  public:
    YTEDeclareType(AddComponentToEngine);
    AddComponentToEngine(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    //void Initialize() override;
  };

} // end YTE namespace

#endif