/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/04/17
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/Engine.hpp"

#include "YTE/Graphics/BaseModel.hpp"

namespace YTE
{
  class InheritVisibility : public Component
  {
  public:
    YTEDeclareType(InheritVisibility);
    InheritVisibility(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void Initialize() override;

    // PROPERTIES /////////////////////////////////////////
    ////////////////////////////////////////////////////////

    void OnParentVisibilityUpdate(UpdateVisibilityEvent *aEvent);

  private:
  };
}