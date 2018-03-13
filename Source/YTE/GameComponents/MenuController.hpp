/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/02/05
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/Engine.hpp"

#include "YTE/GameComponents/InputInterpreter.hpp"

#include "YTE/Physics/Transform.hpp"

namespace YTE
{
	YTEDeclareEvent(MenuElementHover);
	YTEDeclareEvent(MenuElementTrigger);
	YTEDeclareEvent(MenuElementDeHover);

	class MenuElementHover : public Event
	{
	public:
		YTEDeclareType(MenuElementHover);
	};

  class MenuElementTrigger : public Event
  {
  public:
    YTEDeclareType(MenuElementTrigger);
  };

  class MenuElementDeHover : public Event
  {
  public:
    YTEDeclareType(MenuElementDeHover);
  };

  class MenuController : public Component
  {
  public:
    YTEDeclareType(MenuController);
    MenuController(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void Initialize() override;
    // void CloseMenu();

    // PROPERTIES /////////////////////////////////////////
    ///////////////////////////////////////////////////////
    void OnMenuStart(MenuStart *aEvent);
		void OnDirectMenuExit(MenuExit *aEvent);
    void OnMenuExit(MenuExit *aEvent);
    void OnMenuConfirm(MenuConfirm *aEvent);
    void OnMenuElementChange(MenuElementChange *aEvent);

	private:
    Composition* mParentMenu = nullptr;

    Transform *mMyTransform;
    glm::vec3 mViewScale;

    int mCurrMenuElement;
    int mNumElements;
    YTE::CompositionMap* mMenuElements;

		bool mIsDisplayed;

		bool mConstructing;
  };
}
