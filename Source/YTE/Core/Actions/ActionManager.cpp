#include "YTE/Core/Actions/ActionManager.hpp"
#include "YTE/Core/Composition.hpp"
#include "YTE/Core/Engine.hpp"

namespace YTE
{
  YTEDefineType(ActionManager)
  {
    RegisterType<ActionManager>();
    TypeBuilder<ActionManager> builder;

    GetStaticType()->AddAttribute<RunInEditor>();
  }

  ActionManager::ActionManager(Composition * aOwner, Space * aSpace, RSValue * aProperties)
    : Component(aOwner, aSpace)
  {
    UnusedArguments(aProperties);
  }

  void ActionManager::AddSequence(Composition *aComposition, const ActionSequence &sequence)
  {
    mSequences.emplace(aComposition, sequence);
  }

  void ActionManager::Initialize()
  {
    mOwner->RegisterEvent<&ActionManager::Update>(Events::LogicUpdate, this);
    GetSpace()->RegisterEvent<&ActionManager::OnCompositionRemoved>(Events::CompositionRemoved, this);
  }

  void ActionManager::Update(LogicUpdate *aUpdate)
  {
    std::vector<std::unordered_map<Composition*, ActionSequence>::iterator> finishedSequences;
    for (auto it = mSequences.begin(); it != mSequences.end(); ++it)
    {
      auto& sequence = it->second;
      sequence.Increment(static_cast<float>(aUpdate->Dt));
      sequence();
      if (sequence.IsDone())
      {
        finishedSequences.push_back(it);
      }
    }

    for (auto& finished : finishedSequences)
    {
      mSequences.erase(finished);
    }
  }

  void ActionManager::OnCompositionRemoved(CompositionRemoved * aDeletion)
  {
    auto it = mSequences.find(aDeletion->mComposition);
    if (it != mSequences.end())
    {
      mSequences.erase(it);
    }
  }
}
