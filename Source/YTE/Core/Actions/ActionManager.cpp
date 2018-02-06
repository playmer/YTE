#include "ActionManager.hpp"
#include "ActionManager.hpp"
#include "YTE/Core/Actions/ActionManager.hpp"
#include "YTE/Core/Engine.hpp"

namespace YTE
{
  YTEDefineType(ActionManager)
  {
    YTERegisterType(ActionManager);
  }

  ActionManager::ActionManager(Composition * aOwner, RSValue * aProperties)
    : Component(aOwner, nullptr)
  {
    YTEUnusedArgument(aProperties);
  }

  void ActionManager::AddSequence(Composition *aComposition, const ActionSequence &sequence)
  {
    //TODO: Register for delete callback
    mSequences.emplace(aComposition, sequence);
  }

  void ActionManager::Initialize()
  {
  }

  void ActionManager::Update(LogicUpdate *aUpdate)
  {
    std::vector<std::unordered_map<Composition*, ActionSequence>::iterator> finishedSequences;
    for (auto it = mSequences.begin(); it != mSequences.end(); ++it)
    {
      auto& sequence = it->second;
      sequence.Increment(aUpdate->Dt);
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

  //void ActionManager::DeletionUpdate(DeletionUpdate *aDeletion)
  //{
  //
  //}

}
