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
    mSpace->YTERegister(Events::LogicUpdate, this, &CameraAnchor::OnStart);
    mOwner->YTERegister(Events::DirectCameraEvent, this, &CameraAnchor::OnDirectCamera);

    if (mIsDefault)
    {
      mSpace->YTERegister(Events::DialogueExit, this, &CameraAnchor::OnDialogueExit);
      mSpace->YTERegister(Events::StartGame, this, &CameraAnchor::OnStartGame);
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

    mSpace->YTEDeregister(Events::LogicUpdate, this, &CameraAnchor::OnStart);
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
