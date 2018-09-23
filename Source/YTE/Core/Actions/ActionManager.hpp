/******************************************************************************/
/*!
\author Evan T. Collier
\date   2015-10-26
All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#ifndef YTE_Actions_ActionManager_hpp
#define YTE_Actions_ActionManager_hpp

#include <unordered_map>

#include "YTE/Core/Actions/ActionSequence.hpp"
#include "YTE/Core/Component.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"

namespace YTE
{
  class ActionManager : public Component
  {
  public:
    YTEDeclareType(ActionManager);
    YTE_Shared ActionManager(Composition *aOwner, Space * aSpace);
    YTE_Shared void AddSequence(Composition *aComposition, const ActionSequence &sequence);
    YTE_Shared void Initialize();
    YTE_Shared void Update(LogicUpdate *aUpdate);
    YTE_Shared void OnCompositionRemoved(CompositionRemoved *aDeletion);
  private:
    std::unordered_map<Composition*, ActionSequence> mSequences;
  };

}

#endif
