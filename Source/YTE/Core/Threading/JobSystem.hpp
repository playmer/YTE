#pragma once

#ifndef YTE_Core_JobSystem_hpp
#define YTE_Core_JobSystem_hpp

#include <functional>
#include <mutex>
#include <thread>

#include "YTE/Core/Component.hpp"
#include "YTE/Core/Threading/Worker.hpp"

namespace YTE
{
  class JobSystem : public Component
  {
  public:
    YTEDeclareType(JobSystem);

    YTE_Shared JobSystem(Composition *aOwner, Space*);
    YTE_Shared ~JobSystem();
    YTE_Shared void Initialize();
    YTE_Shared void WaitThisThread(JobHandle& aJobHandle);
    YTE_Shared void Update(LogicUpdate *aUpdate);
    YTE_Shared void Join();

    // TODO(Evelyn): Finish FunctionDelegate to allow lambda support

    JobHandle QueueJobThisThread(std::function<Any(JobHandle&)>&& aJob)
    {
      Job* newJob = new Job(std::move(aJob));
      QueueJobInternal(newJob);
      return JobHandle(newJob);
    }

    JobHandle QueueJobThisThread(std::function<Any(JobHandle&)>& aJob, JobHandle& aParentHandle)
    {
      Job* newJob = new Job(std::move(aJob), aParentHandle);
      QueueJobInternal(newJob);
      return JobHandle(newJob);
    }
  private:
    void QueueJobInternal(Job* aJob);

    Worker::WorkerID mForegroundWorker;
    std::unordered_map<Worker::WorkerID, Worker*> mPool;
    bool mAsync;
  };

}

#endif
