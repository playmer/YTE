/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/03/20
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/Engine.hpp"

#include "YTE/Physics/Transform.hpp"

#include "YTE/GameComponents/Menu/MenuController.hpp"

namespace YTE
{
  class ButtonAnimate : public Component
  {
  public:
    YTEDeclareType(ButtonAnimate);
    ButtonAnimate(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void Initialize() override;

    // PROPERTIES //////////////////////////////////////////
    const float GetScaleFactor() const { return mScaleFactor; }
    void SetScaleFactor(const float aScaleFactor) { mScaleFactor = aScaleFactor; }
    ////////////////////////////////////////////////////////

    void OnStart(LogicUpdate *);
    void OnButtonHover(MenuElementHover* aEvent);
    void OnButtonDeHover(MenuElementDeHover* aEvent);

  private:
    Transform *mMyTransform;

    glm::vec3 mNeutralScale;
    float mScaleFactor;

    bool mConstructing;
  };
}
