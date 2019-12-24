#include <chrono>
#include <vector>

#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Threading/JobSystem.hpp"

namespace YTE
{
  YTEDefineType(JobSystem)
  {
    RegisterType<JobSystem>();
    TypeBuilder<JobSystem> builder;
  }


  JobSystem::JobSystem(Composition * aOwner /*= nullptr*/, Space*)
    : Component(aOwner, nullptr)
    , mForegroundWorker()
    , mPool()
    , mAsync(false)
  {
    
  }

  JobSystem::~JobSystem()
  {
    for (auto& worker : mPool)
    {
      worker.second->Join();
    }

    for (auto& worker : mPool)
    {
      delete worker.second;
    }
  }

  void JobSystem::Initialize()
  {
    mOwner->RegisterEvent<&JobSystem::Update>(Events::FrameUpdate, this);
    size_t workerCount = std::thread::hardware_concurrency();
    std::vector<Worker*> workers;

    for (auto i = 0; i < workerCount - 1; ++i)
    {
      workers.push_back(new BackgroundWorker());
    }

    mAsync = !workers.empty();
    workers.push_back(new ForegroundWorker(mAsync));

    for (auto& worker : workers)
    {
      for (auto& coworker : workers)
      {
        if (worker != coworker)
        {
          worker->AddCoworker(coworker);
        }
      }
    }

    for (auto& worker : workers)
    {
      worker->Init();
      Worker::WorkerID id = worker->GetID();
      mPool.insert(std::make_pair(id, worker));
    }
  }

  void JobSystem::WaitThisThread(JobHandle & aJobHandle)
  {
    auto it = mPool.find(std::this_thread::get_id());
    if (it != mPool.end())
    {
      it->second->Wait(aJobHandle);
    }
  }

  void JobSystem::Update(LogicUpdate *aUpdate)
  {
    UnusedArguments(aUpdate);

    if (std::this_thread::get_id() == mForegroundWorker)
    {
      auto it = mPool.find(mForegroundWorker);
      if (it != mPool.end())
      {
        static_cast<ForegroundWorker*>(it->second)->RunForeground();
      }
    }

    // Clean up any jobs that can be freed
    for (auto& worker : mPool)
    {
      worker.second->Clean();
    }
  }

  void JobSystem::QueueJobInternal(Job * aJob)
  {
    auto it = mPool.find(std::this_thread::get_id());
    if (it != mPool.end())
    {
      it->second->Queue(aJob);
    }

    if (!mAsync)
    {
      Update(nullptr);
    }
  }
}
