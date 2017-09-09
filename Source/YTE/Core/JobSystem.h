/******************************************************************************/
/*!
\author Joshua T. Fisher
All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#ifndef YTE_Core_JobSystem_h
#define YTE_Core_JobSystem_h

#include <atomic>
#include <chrono>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "YTE/Core/Component.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"

namespace YTE
{
  class BaseJob
  {
  public:
    virtual void Invoke() = 0;
  };


  template <typename ClassType>
  class Job : public BaseJob
  {
  public:
    using Function = void (ClassType::*)();

    Job(ClassType *aThis, Function aFunction)
      : mObject(aThis), mFunction(aFunction) {};

    void Invoke() override
    {
      (mObject->*mFunction)();
    }

  private:
    Function mFunction;
    ClassType *mObject;
  };

  class JobSystem : public Component
  {
  public:
    DeclareType(JobSystem);

    JobSystem(Composition *aOwner, RSValue *aProperties)
      : Component(aOwner, nullptr), mEngineClosing(false)
    {
      auto threadsToCreate = std::thread::hardware_concurrency();
      threadsToCreate = threadsToCreate <= 1 ? 1 : threadsToCreate - 1;

      for (size_t i = 0; i < threadsToCreate; ++i)
      {
        mThreads.emplace_back(std::make_unique<std::thread>(&JobSystem::Task, this));
      }
    }

    void EndExecution()
    {
      mEngineClosing = true;
    };

    void Join()
    {
      for (auto &thread : mThreads) thread->join();
    }

    template <typename ClassType>
    void AddJob(ClassType *aThis, void (ClassType::*aFunction)())
    {
      mJobsMutex.lock();
      mJobs.emplace_back(aThis, aFunction);
      mJobsMutex.unlock();
    }
      
    template <typename ClassType>
    void AddJobAtFront(ClassType *aThis, void (ClassType::*aFunction)())
    {
      mJobsMutex.lock();
      mJobs.emplace(mJobs.begin(), aThis, aFunction);
      mJobsMutex.unlock();
    }

  private:
    void Task()
    {
      while (mEngineClosing == false)
      {
        UniquePointer<BaseJob> job{nullptr};

        mJobsMutex.lock();
        if (mJobs.size() > 0)
        {
          auto job = std::move(mJobs.front());
          mJobs.erase(mJobs.begin());
        }
        mJobsMutex.unlock();

        if (job.get() != nullptr)
        {
          job->Invoke();
        }
          
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
    }

    std::vector<UniquePointer<std::thread>> mThreads;
    std::vector<UniquePointer<BaseJob>> mJobs;
    std::mutex mJobsMutex;
    std::atomic_bool mEngineClosing;
  };
}

#endif
