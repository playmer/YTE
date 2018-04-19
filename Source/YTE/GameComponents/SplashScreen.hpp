/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/04/18
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/Engine.hpp"

namespace YTE
{
  class SplashScreen : public Component
  {
  public:
    YTEDeclareType(SplashScreen);
    SplashScreen(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void Initialize() override;
    void Start() override;

    // PROPERTIES /////////////////////////////////////////
    ////////////////////////////////////////////////////////

    void OnFrameUpdate(LogicUpdate *);
    void OnLogicUpdate(LogicUpdate *);

  private:
    float mFadeValue;
    bool mReadyToExecute;

    Sprite *mDigiPenLogo;
    Sprite *mTeamLogo;
    Sprite *mControllerWarning;

    Space *mBlackoutLevel;
    GraphicsView *mView;
  };
}