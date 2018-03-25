/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/03/21
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#include "YTE/Core/Component.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/Physics/Transform.hpp"

#include "YTE/GameComponents/InputInterpreter.hpp"

namespace YTE
{
  YTEDeclareEvent(AttachCamera);

  class AttachCamera : public Event
  {
  public:
    YTEDeclareType(AttachCamera);
    AttachCamera(Composition *aOwner) { Anchor = aOwner; }

    Composition *Anchor;
  };

  class CameraAnchor : public Component
  {
  public:
    YTEDeclareType(CameraAnchor);
    CameraAnchor(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void Initialize() override;
    void Start() override;
    void OnStart(LogicUpdate*);

    // Properties /////////////////////////////////////////////////////////////////////
    const bool GetIsDefault() { return mIsDefault; }
    void SetIsDefault(const bool aIsDefault) { mIsDefault = aIsDefault; }
    ///////////////////////////////////////////////////////////////////////////////////

    void OnDirectCamera(DirectCameraEvent *aEvent);
    void OnDialogueExit(DialogueExit *aEvent);

  private:
    bool mIsDefault;
  };
}
