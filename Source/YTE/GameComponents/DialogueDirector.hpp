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
#include "YTE/GameComponents/BoatController.hpp"
#include "YTE/GameComponents/DialogueGraph.hpp"

#include "YTE/Graphics/Camera.hpp"

namespace YTE
{
  /////////////////////////////////////////////////////////////////////////////////////
  // Events
  /////////////////////////////////////////////////////////////////////////////////////
  YTEDeclareEvent(UISelectEvent);
  YTEDeclareEvent(UIConfirmEvent);
  YTEDeclareEvent(UIDisplayEvent);
  YTEDeclareEvent(UIUpdateContent);


  class UISelectEvent : public Event
  {
  public:
    YTEDeclareType(UISelectEvent);
    UISelectEvent(bool aShouldSelect) { SelectOrDeselect = aShouldSelect; }
    bool SelectOrDeselect;
  };

  class UIConfirmEvent : public Event
  {
  public:
    YTEDeclareType(UIConfirmEvent);
  };

  class UIDisplayEvent : public Event
  {
  public:
    YTEDeclareType(UIDisplayEvent);
    UIDisplayEvent(bool aShouldDisplay) { ShouldDisplay = aShouldDisplay; }
    bool ShouldDisplay;
  };

  class UIUpdateContent : public Event
  {
  public:
    YTEDeclareType(UIUpdateContent);
    UIUpdateContent(const std::string& aNewMessage) { ContentMessage = aNewMessage; }
    std::string ContentMessage;
  };
  
  YTEDeclareEvent(DialogueStart);

  class DialogueStart : public Event
  {
  public:
    YTEDeclareType(DialogueStart);
    //Composition *camera;
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
    void Update(LogicUpdate *aEvent);

    // Properties /////////////////////////////////////////////////////////////////////
    //const glm::vec3& GetCamAnchor() { return mCameraAnchorPosition; }
    //void SetCamAnchor(const glm::vec3& aCamAnchor) { mCameraAnchorPosition = aCamAnchor; }

    /*const glm::vec3& GetPlayerMark() { return mPlayerMark; }
    void SetPlayerMark(const glm::vec3& aMark) { mPlayerMark = aMark; }

    const glm::vec3& GetCharMark() { return mCharacterMark; }
    void SetCharMark(const glm::vec3& aMark) { mCharacterMark = aMark; }*/
    ///////////////////////////////////////////////////////////////////////////////////

    void OnRequestDialogueStart(RequestDialogueStart *aEvent);
    void OnDialogueNodeReady(DialogueNodeReady *aEvent);
    void OnDialogueSelect(DialogueSelect *aEvent);
    void OnDialogueConfirm(DialogueConfirm *aEvent);
    void OnDialogueExit(DialogueExit *aEvent);
    void OnCollisionPersist(CollisionPersisted *aEvent);
    void OnCollisionStart(CollisionStarted *aEvent);
    void OnCollisionEnd(CollisionEnded *aEvent);

  private:
    Space *mDialogueSpace;
    Composition *mCameraAnchor;

    //glm::vec3 mDockAnchorPosition;
    //
    //  // @@@TODO(Isaac): Can probably just send these to the dialogue UI space...each element can
    //  //                 check itself if it needs to update/what it should display
    //Composition* mCharacterDialogue;
    //Composition* mDialogueOption1;
    //Composition* mDialogueOption2;
    //Composition* mDialogueOption3;
    //Composition* mLastSelected;

    //std::vector<std::string> mCurNodeData;
    //DialogueNode::NodeType mCurNodeType;
    //int mCurNodeDataIndex;

    bool mActive;

      // @@@NICK: Currently unused, but may be needed to animate characters on dock?
      // Set the models to hit their "mark" during the RequestDialogueStart handler
    /*glm::vec3 mPlayerMark;
    glm::vec3 mCharacterMark;*/
  };
}
