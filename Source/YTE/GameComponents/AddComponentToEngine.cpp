/******************************************************************************/
/*!
\file   AddComponentToEngine.cpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-01-27

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/AddComponentToEngine.hpp"
#include "YTE/GameComponents/InputInterpreter.hpp"

namespace YTE
{
  YTEDefineType(AddComponentToEngine)
  {
    RegisterType<AddComponentToEngine>();
    TypeBuilder<AddComponentToEngine> builder;
  }

  AddComponentToEngine::AddComponentToEngine(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    UnusedArguments(aProperties);
  }

  void AddComponentToEngine::NativeInitialize()
  {
    if (nullptr == mOwner->GetEngine()->GetComponent<InputInterpreter>())
    {
      //auto inputInterpreter = mOwner->GetEngine()->AddComponent<InputInterpreter>();
      //inputInterpreter->Initialize();
    }
  }
}