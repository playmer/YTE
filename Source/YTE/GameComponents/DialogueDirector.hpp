/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/03/15
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/EventHandler.hpp"

#include "YTE/GameComponents/InputInterpreter.hpp"
#include "YTE/GameComponents/BoatController.hpp" /* Colliders, */
#include "YTE/GameComponents/DialogueGraph.hpp"
#include "YTE/GameComponents/JohnDialogue.hpp"
#include "YTE/GameComponents/NoticeBoard.hpp" /* piggybacking off noticeboardhookup */
#include "YTE/GameComponents/HudController.hpp" /* piggybacking off postcard update to know when tutorial ends */

#include "YTE/Graphics/Camera.hpp"

namespace YTE
{
  /////////////////////////////////////////////////////////////////////////////////////
  // Events
  /////////////////////////////////////////////////////////////////////////////////////
  YTEDeclareEvent(UISelectEvent);
  YTEDeclareEvent(UIFocusSwitchEvent);
  YTEDeclareEvent(UIDisplayEvent);
  YTEDeclareEvent(UIUpdateContent);

  class UISelectEvent : public Event
  {
  public:
    YTEDeclareType(UISelectEvent);
    UISelectEvent(int aSelection) { SelectionIndex = aSelection; }
    int SelectionIndex;
    int NumOptions = 0;
  };

  class UIFocusSwitchEvent : public Event
  {
  public:
    YTEDeclareType(UIFocusSwitchEvent);
    UIFocusSwitchEvent(bool aIsPassiveFocused) { IsPassiveFocused = aIsPassiveFocused; }
    bool IsPassiveFocused = false;
  };

  class UIDisplayEvent : public Event
  {
  public:
    YTEDeclareType(UIDisplayEvent);
    UIDisplayEvent(bool aShouldDisplay) { ShouldDisplay = aShouldDisplay; }

    bool ShouldDisplay;
    int DisplayIndex = -1;
  };

  class UIUpdateContent : public Event
  {
  public:
    YTEDeclareType(UIUpdateContent);
    UIUpdateContent(bool aIsPassive, const std::string& aNewMessage) 
    { 
      IsPassive = aIsPassive;
      ContentMessage = aNewMessage; 
    }

    bool IsPassive;
    int SelectionIndex = -1;
    std::string ContentMessage;
  };
  
  YTEDeclareEvent(DialogueStart);

  class DialogueStart : public Event
  {
  public:
    YTEDeclareType(DialogueStart);
  };

  YTEDeclareEvent(PlaySoundEvent);

  /* PlaySoundEvent(int aSize, int aOption, bool aIsText) */
  class PlaySoundEvent : public Event
  {
  public:
    YTEDeclareType(PlaySoundEvent);
    PlaySoundEvent(int aSize, int aOption, bool aIsText) : size(aSize), option(aOption), isText(aIsText) {};
    int size;
    int option;
    bool isText;
  };

  /////////////////////////////////////////////////////////////////////////////////////
  // Class
  /////////////////////////////////////////////////////////////////////////////////////
  class DialogueDirector : public Component
  {
  public:

    YTEDeclareType(DialogueDirector);
    DialogueDirector(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;

    void OnRequestDialogueStart(RequestDialogueStart *);
    void OnDialogueNodeReady(DialogueNodeReady *aEvent);
    void OnDialogueSelect(DialogueSelect *aEvent);
    void OnDialogueConfirm(DialogueConfirm *);
    void OnDialogueExit(DialogueExit *);
    void OnCollisionStart(CollisionStarted *aEvent);
    void OnCollisionEnd(CollisionEnded *aEvent);
    void OnTutorialUpdate(TutorialUpdate *aEvent);
    void OnPostcardUpdate(PostcardUpdate *aEvent);
    void OnNoticeBoardHookup(NoticeBoardHookup *aEvent);

    // i am the god of this universe, i decree that this one function shall be public, question me and be smitten
    void DeregisterDirector();
  private:
    void RegisterDirector();

    Space *mDialogueSpace;
    Composition *mCameraAnchor;

    Transform *mLambTransform;
    bool mIsLambSet = false;

    std::vector<std::string> mCurNodeData;
    DialogueNode::NodeType mCurNodeType;
    int mCurNodeDataIndex;

    int mLastSelectionIndex;
    int mMaxSelectionIndex;

    bool mIsRegistered = false;
    bool mIsTutorial = true;
    Quest::CharacterName mTutorialRegisteredCharacter = Quest::CharacterName::John;
    Quest **mJohnQuestHandle;
    Quest **mDaisyQuestHandle;
    Quest **mBasilQuestHandle;
  };
}
