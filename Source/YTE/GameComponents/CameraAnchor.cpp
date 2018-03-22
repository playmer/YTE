/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/03/21
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/GameComponents/CameraAnchor.hpp"

namespace YTE
{
  YTEDefineEvent(AttachCamera);
  YTEDefineType(AttachCamera) { YTERegisterType(AttachCamera); }

  YTEDefineType(CameraAnchor)
  {
    YTERegisterType(CameraAnchor);

    std::vector<std::vector<Type*>> deps = { { TypeId<Transform>() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);

    YTEBindProperty(&GetIsActive, &SetIsActive, "IsActive")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .SetDocumentation("Flag for specifying if the current game camera should attach to this anchor or not");
  }

  CameraAnchor::CameraAnchor(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mIsActive(false)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void CameraAnchor::Initialize()
  {
    mSpace->YTERegister(Events::LogicUpdate, this, &CameraAnchor::OnStart);
    //mSpace->YTERegister(Events::CameraRotateEvent, this, &CameraAnchor::RotateCamera);
  }

  void CameraAnchor::Start()
  {
    /*if (mIsActive)
    {
      AttachCamera attach(mOwner);

      mSpace->SendEvent(Events::AttachCamera, &attach);
    }*/
  }

  void CameraAnchor::OnStart(LogicUpdate*)
  {
    if (mIsActive)
    {
      AttachCamera attach(mOwner);

      mSpace->SendEvent(Events::AttachCamera, &attach);
    }

    mSpace->YTEDeregister(Events::LogicUpdate, this, &CameraAnchor::OnStart);
  }

  void CameraAnchor::RotateCamera(CameraRotateEvent *aEvent)
  {
  }

  void CameraAnchor::OnDirectCamera(DirectCameraEvent *aEvent)
  {

  }

  void CameraAnchor::OnDialogueExit(DialogueExit *aEvent)
  {

  }
}
