/******************************************************************************/
/*!
\file   QuestLogic.cpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-03-25

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/QuestLogic.hpp"

namespace YTE
{
  YTEDefineType(QuestLogic) { YTERegisterType(QuestLogic); }

  QuestLogic::QuestLogic(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    YTEUnusedArgument(aProperties);
  }

  void QuestLogic::Initialize()
  {
  }


}//end yte