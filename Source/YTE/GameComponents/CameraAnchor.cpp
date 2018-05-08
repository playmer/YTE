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
  YTEDefineType(AttachCamera) { RegisterType<AttachCamera>(); }

  YTEDefineType(CameraAnchor)
  {
    RegisterType<CameraAnchor>();

    std::vector<std::vector<Type*>> deps = { { TypeId<Transform>() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);

    YTEBindProperty(&GetIsDefault, &SetIsDefault, "IsDefault")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .SetDocumentation("Flag for setting the default anchor -- that is, if no anchors are active, enable this one");
  }

  CameraAnchor::CameraAnchor(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mIsDefault(false)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void CameraAnchor::Initialize()
  {
    mSpace->RegisterEvent<&CameraAnchor::OnStart>(Events::LogicUpdate, this);
    mOwner->RegisterEvent<&CameraAnchor::OnDirectCamera>(Events::DirectCameraEvent, this);

    if (mIsDefault)
    {
      mSpace->RegisterEvent<&CameraAnchor::OnDialogueExit>(Events::DialogueExit, this);
      mSpace->RegisterEvent<&CameraAnchor::OnStartGame>(Events::StartGame, this);
    }
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
    /*if (mIsDefault)
    {
      AttachCamera attach(mOwner);

      mSpace->SendEvent(Events::AttachCamera, &attach);
    }*/

    mSpace->DeregisterEvent<&CameraAnchor::OnStart>(Events::LogicUpdate,  this);
  }

  void CameraAnchor::OnDirectCamera(DirectCameraEvent *)
  {
    AttachCamera attach(mOwner);
    mSpace->SendEvent(Events::AttachCamera, &attach);
  }

  void CameraAnchor::OnDialogueExit(DialogueExit *)
  {
    AttachCamera attach(mOwner);
    mSpace->SendEvent(Events::AttachCamera, &attach);
  }

  void CameraAnchor::OnStartGame(StartGame*)
  {
    AttachCamera attach(mOwner);
    mSpace->SendEvent(Events::AttachCamera, &attach);
  }
}
