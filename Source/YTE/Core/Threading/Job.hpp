/******************************************************************************/
/*!
\author Evan T. Collier
All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once
#include <atomic>
#include <memory>

#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/Utilities.hpp"

#include "YTE/StandardLibrary/Delegate.hpp"

#include <functional>

namespace YTE
{
  class Job
  {
  public:
    Job(std::function<Any(JobHandle&)>&& aDelegate);
    Job(std::function<Any(JobHandle&)>&& aDelegate, JobHandle& aParentHandle);
    ~Job();

    JobHandle GetParentHandle();
    bool HasCompleted() const;
    float Progress() const;
    bool IsDeletable() const;

    Any GetReturn();
    void Invoke();

    void IncrementJobs();
    void DecrementJobs();

    void Abandon();
    std::weak_ptr<bool> GetAbandonedHandle();

  protected:
    Job* mParentJob;
    std::function<Any(JobHandle&)> mDelegate;
    Any mReturn;
    std::atomic<int> mTotalJobs;
    std::atomic<int> mUnfinishedJobs;
    std::shared_ptr<bool> mAbandoned;
  };
}
