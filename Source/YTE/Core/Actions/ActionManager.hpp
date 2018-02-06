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

#include "YTE/Core/Component.hpp"

namespace YTE
{
#define YTENewAction(name)

  struct Action
  {

  };

  struct Action_Callback : Action
  {

  };

  struct Group
  {
    Group();
    Group(const Group& group);
    template <typename T>
    void Add(float& aValue, float aFinal, float aDuration)
    {

    }
    void Call(std::function<void(void)> aCallback);
    void Delay(float aDuration);
  private:
    std::vector<Action*> mActions;
  };

  struct ActionSequence
  {
    ActionSequence();
    void AddGroup(const Group &group);

    template <typename T>
    void Add(float& aValue, float aFinal, float aDuration)
    {

    }

    void Call(std::function<void(void)> aCallback);
    void Delay(float aDuration);
  private:
    std::queue<Group> mGroups;
  };

  class ActionManager : public Component
  {
  public:
    YTEDeclareType(ActionManager);
    ActionManager(Composition *aOwner, RSValue *aProperties = nullptr);
    void AddSequence(Composition *aComposition, const ActionSequence &sequence);
    void Initialize();
    void Update(LogicUpdate *aUpdate);
  private:
    std::unordered_map<Composition* aOwner, ActionSequence> mSequences;
  };

}

#endif
