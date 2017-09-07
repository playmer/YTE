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

#include "YTE/Physics/Reactive.h"

namespace YTE
{
  DefineType(Reactive)
  {
    YTERegisterType(Reactive);
  }

  Reactive::Reactive(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    DeserializeByType<Reactive*>(aProperties, 
      this, Reactive::GetStaticType());
  };

  void Reactive::Initialize()
  {
    mSpace->RegisterListener(Events::LogicUpdate, *this, &Reactive::OnLogicUpdate);
    mOwner->RegisterListener(Events::CollisionStarted, *this, &Reactive::OnCollisionStarted);
    mOwner->RegisterListener(Events::CollisionEnded, *this, &Reactive::OnCollisionEnded);

    mSpace->GetEngine()->GetWindow()->mMouse.RegisterListener(Events::MousePress, *this, &Reactive::OnMousePress);
    mSpace->GetEngine()->GetWindow()->mMouse.RegisterListener(Events::MouseRelease, *this, &Reactive::OnMouseRelease);

    mMenuCollider = mOwner->GetComponent<MenuCollider>();
    mIsMouseEntered = false;
  }

  void Reactive::OnCollisionStarted(CollisionStarted *aEvent)
  {
    mIsMouseEntered = true;

      // Send a MouseEntered event
    MouseEnter mouseEnter;
    mOwner->Trigger(Events::MouseEnter, &mouseEnter);
  }

  void Reactive::OnCollisionEnded(CollisionEnded *aEvent)
  {
    mIsMouseEntered = false;

      // Send a MouseExit event
    MouseExit mouseExit;
    mOwner->Trigger(Events::MouseExit, &mouseExit);
  }

  void Reactive::OnMousePress(MouseButtonEvent *aEvent)
  {
      // Receive the global mouse click event, determine if it's relevant, then send it out again locally
    if (mIsMouseEntered)
    {
      mOwner->Trigger(Events::MousePress, aEvent);
    }
  }

  void Reactive::OnMouseRelease(MouseButtonEvent *aEvent)
  {
      // Receive the global mouse release event, determine if it's relevant, then send it out again locally
    if (mIsMouseEntered)
    {
      mOwner->Trigger(Events::MouseRelease, aEvent);
    }
  }

  void Reactive::OnLogicUpdate(LogicUpdate *aEvent)
  {
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
