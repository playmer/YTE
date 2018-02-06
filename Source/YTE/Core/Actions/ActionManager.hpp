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
  /*
  #define YTENewAction(name) \
  class name : public Action_CRTP<name> \
  { \ 
  public: \
    name(float& aValue, float aFinal, float aDur) \
      : Action(aDur), value(aValue), b(aValue), c(aFinal - b) { } \
    float& value; \
    float b; \
    float c; \
    void Init() override; \
    { \
      if (b != value) { \
        float f = b + c;  \
        b = value; \
        c = f - b;
      } \
    } \
    void operator()(); \
  }; \
  inline void name::operator()();
*/

  class Action
  {
  public:
    friend class Group;
    explicit Action(float aDur);
    float Increment(float dt);
    bool IsDone() const;
    virtual void Init();
    virtual Action * Clone();
    virtual void operator() ();
    virtual ~Action();
  protected:
    float d;
    float t;
    float Time;
  };

  template <typename T>
  class Action_CRTP : public Action
  {
  public:
    Action_CRTP(float aDur) : Action(aDur)
    {
      static_assert(std::is_base_of<Action_CRTP, T>::value,
        "Action_CRTP is for curiously recurring template pattern use only");
    }
    Action * Clone() { return new T(*reinterpret_cast<T*>(this)); }
  };

  class Action_Callback : public Action_CRTP<Action_Callback>
  {
  public:
    Action_Callback(std::function<void(void)> aCallback);
    void operator() () override;
  private:
    bool mHasCalled;
    std::function<void(void)> mCallback;
  };

  class Group
  {
  public:
    friend class ActionSequence;
    Group();
    Group(const Group& aGroup);
    ~Group();
    template <typename T>
    void Add(float& aValue, float aFinal, float aDuration)
    {
      static_assert(std::is_base_of<Action, T>::value, 
        "Actions must derive from the Action class");
      mActions.push_back(new T(aValue, aFinal, aDuration));
    }
    void Call(std::function<void(void)> aCallback);
    void Delay(float aDuration);
  private:
    void Init();
    float Increment(float dt);
    void operator() ();
    bool IsDone() const;

    std::vector<Action*> mActions;
  };

  class ActionSequence
  {
    friend class ActionManager;
    ActionSequence();
    void AddGroup(const Group &group);

    template <typename T>
    void Add(float& aValue, float aFinal, float aDuration)
    {
      static_assert(std::is_base_of<Action, T>::value,
        "Actions must derive from the Action class");
      Group group;
      group.Add<T>(aValue, aFinal, aDuration);
      mGroups.push(group);
    }

    void Call(std::function<void(void)> aCallback);
    void Delay(float aDuration);
  private:
    float Increment(float dt);
    void operator() ();
    bool IsDone() const;
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
    void OnCompositionRemoved(CompositionRemoved *aDeletion);
  private:
    std::unordered_map<Composition*, ActionSequence> mSequences;
  };

}

#endif
