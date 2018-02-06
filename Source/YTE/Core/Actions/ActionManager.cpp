#include "YTE/Core/Actions/ActionManager.hpp"

namespace YTE
{
  YTEDefineType(ActionManager)
  {
    YTERegisterType(ActionManager);
  }

  void ActionManager::AddSequence(Composition *aComposition, const ActionSequence &sequence)
  {

  }

  void ActionManager::Update(LogicUpdate *aUpdate)
  {
    std::vector<std::unordered_map::iterator> finishedSequences;
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

  void ActionManager::DeletionUpdate(DeletionUpdate *aDeletion)
  {

  }

}
