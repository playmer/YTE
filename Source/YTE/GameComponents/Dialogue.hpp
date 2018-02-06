#pragma once

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/EventHandler.hpp"

#include "YTE/GameComponents/InputInterpreter.hpp"

#include "YTE/Graphics/Camera.hpp"


namespace YTE
{
  class Dialogue : public Component
  {
  public:

    YTEDeclareType(Dialogue);
    Dialogue(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;
    void Update(LogicUpdate *aEvent);

  private:

    void OnStart(DialogueStart *aEvent);
    void OnConfirm(DialogueConfirm *aEvent);
    void OnExit(DialogueExit *aEvent);


  };
} // End YTE namespace