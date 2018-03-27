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
  YTEDefineEvent(QuestStart);
  YTEDefineEvent(UpdateQuestState);
  YTEDefineEvent(AssignPostcard);

  YTEDefineType(AssignPostcard) { YTERegisterType(AssignPostcard); }
  YTEDefineType(QuestStart) { YTERegisterType(QuestStart); }
  YTEDefineType(UpdateQuestState) { YTERegisterType(UpdateQuestState); }
  YTEDefineType(QuestLogic) { YTERegisterType(QuestLogic); }

  QuestLogic::QuestLogic(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    YTEUnusedArgument(aProperties);
  }

  void QuestLogic::Initialize()
  {
    mSpace->YTERegister(Events::AssignPostcard, this, &QuestLogic::OnAssignPostcard);
  }

  void QuestLogic::OnAssignPostcard(AssignPostcard *aEvent)
  {
    mReceivedPostcard = aEvent->mPostcard;
  }

  void QuestLogic::OnStartQuest(QuestStart *aEvent)
  {

  }

  void QuestLogic::OnUpdateQuestState(UpdateQuestState *aEvent)
  {

  }
}//end yte