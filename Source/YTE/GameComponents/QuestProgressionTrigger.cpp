/******************************************************************************/
/*!
\file   QuestProgressionTrigger.cpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-03-27

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/QuestProgressionTrigger.hpp"

namespace YTE
{
  YTEDefineType(QuestProgressionTrigger) { YTERegisterType(QuestProgressionTrigger); }

  QuestProgressionTrigger::QuestProgressionTrigger(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    YTEBindProperty(&QuestProgressionTrigger::IsCollisionTrigger, &QuestProgressionTrigger::SetTriggerType, "IsCollisionTrigger")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();
  }

  void QuestProgressionTrigger::Initialize()
  {
  }
}//end yte