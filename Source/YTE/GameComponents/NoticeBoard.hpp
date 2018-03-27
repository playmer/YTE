/******************************************************************************/
/*!
\file   NoticeBoard.hpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-03-25
\brief
    This component handles the logic for changing the dialogues and quest states

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once
#ifndef YTE_Gameplay_NoticeBoard_hpp
#define YTE_Gameplay_NoticeBoard_hpp

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/Physics/BoxCollider.hpp"

#include "YTE/GameComponents/InputInterpreter.hpp"
#include "YTE/GameComponents/JohnDialogue.hpp"


namespace YTE
{
  class Postcard
  {
  public:
    Postcard(Quest::CharacterName aCharacterName, Quest::Name aQuestName) : mCharacterName(aCharacterName), mQuestName(aQuestName) {};
    Quest::CharacterName GetCharacter() { return mCharacterName; };
    Quest::Name GetQuest() { return mQuestName; };
  private:
    Quest::CharacterName mCharacterName;
    Quest::Name mQuestName;
  };

  class NoticeBoard : public Component
  {
  public:
    YTEDeclareType(NoticeBoard);
    NoticeBoard(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;

    void OnCollisionStarted(CollisionStarted *aEvent);
    void OnCollisionEnded(CollisionEnded *aEvent);
    void OnRequestNoticeBoardStart(RequestNoticeBoardStart *aEvent);
    void OnNoticeBoardHookup(NoticeBoardHookup *aEvent);
  private:
    std::vector<Postcard> mPostcardVec;
    Postcard *mAssignedPostcard;
    std::map<Quest::CharacterName, Quest**> mActiveQuestMap;
  };
}//end yte
#endif