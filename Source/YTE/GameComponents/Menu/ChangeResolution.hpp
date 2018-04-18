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

#include "YTE/Graphics/SpriteText.hpp"
#include "YTE/Graphics/Material.hpp"

#include "YTE/GameComponents/Menu/MenuController.hpp"

namespace YTE
{
  namespace Resolutions
  {
    enum Type { R1152x648, R1280x720, R1920x1080, R2160x1440, COUNT };
    static std::string Names[] = { "1152x648", "1280x720", "1920x1080", "2160x1440" };
  }

  class ChangeResolution : public Component
  {
    struct ArrowSprites
    {
      Sprite *mLeft = nullptr;
      Sprite *mRight = nullptr;
    };

  public:
    YTEDeclareType(ChangeResolution);
    ChangeResolution(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void Initialize() override;
    void Start() override;

    // PROPERTIES /////////////////////////////////////////
    ////////////////////////////////////////////////////////

    void OnStickFlickEvent(OptionsFlickEvent *aEvent);
    void OnConfirm(OptionsConfirmEvent *aEvent);

    void OnElementTrigger(MenuElementTrigger *);

  private:
    Transform *mMyTransform;

    ArrowSprites mArrows;
    SpriteText *mFieldText;

    Resolutions::Type mCurrResolution;

    u32 mCurrWidth;
    u32 mCurrHeight;
  };
}