/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/04/11
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/Engine.hpp"

#include "YTE/GameComponents/Menu/MenuController.hpp"

namespace YTE
{
  class LaunchLevel : public Component
  {
  public:
    YTEDeclareType(LaunchLevel);
    LaunchLevel(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void Initialize() override;

    // PROPERTIES /////////////////////////////////////////
    const std::string& GetLevelToLaunch() const { return mLevelToLaunch; }
    void SetLevelToLaunch(const std::string& aLevelToLaunch) { mLevelToLaunch = aLevelToLaunch; }
    ////////////////////////////////////////////////////////

    void OnElementTrigger(MenuElementTrigger *);

  private:
    std::string mLevelToLaunch;
  };
}