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

#include <functional>
#include <queue>
#include <unordered_map>

#include "YTE/Core/Component.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"

namespace YTE
{
#define YTENewAction(name)

  struct Action
  {
    friend struct Group;
    explicit Action(float aDur);
    float Increment(float dt);
    bool IsDone() const;
    virtual void Init();
    virtual void operator() ();
    virtual ~Action();
  protected:
    float d;
    float t;
    float Time;
  };

  template <typename T>
  struct Action_RTP : public Action
  {
    Action * Clone() { return new T(*reinterpret_cast<T*>(this)); }
  };

  struct Action_Callback : Action_RTP<Action_Callback>
  {
    Action_Callback(std::function<void(void)> aCallback);
    ~Action_Callback();

  private:
    bool mHasCalled;
    std::function<void(void)> mCallback;
  };

  struct Group
  {
    friend class ActionSequence;
    Group();
    Group(const Group& group);
    template <typename T>
    void Add(float& aValue, float aFinal, float aDuration)
    {

    }
    void Call(std::function<void(void)> aCallback);
    void Delay(float aDuration);
    bool IsDone() const;
  private:
    std::vector<Action*> mActions;
  };

  struct ActionSequence
  {
    friend class ActionManager;
    ActionSequence();
    void AddGroup(const Group &group);

    template <typename T>
    void Add(float& aValue, float aFinal, float aDuration)
    {
      Group group;
      group.Add<T>(aValue, aFinal, aDuration);
      mGroups.push(group);
    }

    void Call(std::function<void(void)> aCallback);
    void Delay(float aDuration);
    bool IsDone() const;
  private:
    float Increment(float dt);
    void operator() ();
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
    //void DeletionUpdate(DeletionUpdate *aDeletion);
  private:
    std::unordered_map<Composition*, ActionSequence> mSequences;
  };

}

#endif
