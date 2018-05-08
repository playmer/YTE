/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   11/28/2015
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "YTE/Core/Composition.hpp"
#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Platform/DeviceEnums.hpp"

#include "YTE/Physics/Reactive.hpp"

namespace YTE
{
  YTEDefineEvent(MouseEnter);
  YTEDefineEvent(MouseExit);

  YTEDefineType(MouseEnter)
  {
    RegisterType<MouseEnter>();
    TypeBuilder<MouseEnter> builder;
    builder.Field<&MouseEnter::Name>( "Name", PropertyBinding::Get);
  }

  YTEDefineType(MouseExit)
  {
    RegisterType<MouseExit>();
    TypeBuilder<MouseExit> builder;
    builder.Field<&MouseExit::Name>( "Name", PropertyBinding::Get);
  }

  YTEDefineType(Reactive)
  {
    RegisterType<Reactive>();
    TypeBuilder<Reactive> builder;
  }

  Reactive::Reactive(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  };

  void Reactive::Initialize()
  {
    mSpace->RegisterEvent<&Reactive::OnLogicUpdate>(Events::LogicUpdate, this);
    mOwner->RegisterEvent<&Reactive::OnCollisionStarted>(Events::CollisionStarted, this);
    mOwner->RegisterEvent<&Reactive::OnCollisionEnded>(Events::CollisionEnded, this);

    mSpace->GetEngine()->GetWindow()->mMouse.RegisterEvent<&Reactive::OnMousePress>(Events::MousePress, this);
    mSpace->GetEngine()->GetWindow()->mMouse.RegisterEvent<&Reactive::OnMouseRelease>(Events::MouseRelease, this);

    mMenuCollider = mOwner->GetComponent<MenuCollider>();
    mIsMouseEntered = false;
  }

  void Reactive::OnCollisionStarted(CollisionStarted *aEvent)
  {
    YTEUnusedArgument(aEvent);
    mIsMouseEntered = true;

      // Send a MouseEntered event
    MouseEnter mouseEnter;
    mOwner->SendEvent(Events::MouseEnter, &mouseEnter);
  }

  void Reactive::OnCollisionEnded(CollisionEnded *aEvent)
  {
    YTEUnusedArgument(aEvent);
    mIsMouseEntered = false;

      // Send a MouseExit event
    MouseExit mouseExit;
    mOwner->SendEvent(Events::MouseExit, &mouseExit);
  }

  void Reactive::OnMousePress(MouseButtonEvent *aEvent)
  {
    YTEUnusedArgument(aEvent);

      // Receive the global mouse click event, determine if it's relevant, then send it out again locally
    if (mIsMouseEntered)
    {
      mOwner->SendEvent(Events::MousePress, aEvent);
    }
  }

  void Reactive::OnMouseRelease(MouseButtonEvent *aEvent)
  {
    YTEUnusedArgument(aEvent);

      // Receive the global mouse release event, determine if it's relevant, then send it out again locally
    if (mIsMouseEntered)
    {
      mOwner->SendEvent(Events::MouseRelease, aEvent);
    }
  }

  void Reactive::OnLogicUpdate(LogicUpdate *aEvent)
  {
    YTEUnusedArgument(aEvent);

    //TODO (Josh): Fix this.
    //  // Check for collision between owner and mouse position
    //  // Respond based on the collision events that are sent
    //Graphics::GraphicsSystem *underseer = mSpace->GetUniverse()->GetComponent<Graphics::GraphicsSystem>();
    //auto mousePos = mSpace->GetUniverse()->GetWindow()->mMouse.GetCursorPosition();
    //auto mousePosMenu = underseer->ScreenToNDC(mousePos);
    //
    //glm::vec2 screenScale = underseer->ScaleMenuSpriteDimensions(mMenuCollider->GetScale());
    //
    //mMenuCollider->PointToBox2D(screenScale, mousePosMenu);
  }
}
