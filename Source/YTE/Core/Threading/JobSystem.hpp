/******************************************************************************/
/*!
\author Evan T. Collier
All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
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

    JobSystem(Composition *aOwner = nullptr);
    ~JobSystem();
    void Initialize();
    void WaitThisThread(JobHandle& aJobHandle);
    void Update(LogicUpdate *aUpdate);

    // TODO(Evan): Finish FunctionDelegate to allow lambda support

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
