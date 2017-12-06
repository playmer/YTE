/******************************************************************************/
/*!
\author Evan T. Collier
All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/Core/Threading/Job.hpp"
#include "YTE/Core/Threading/JobHandle.hpp"

namespace YTE
{
  Job::Job(Delegate<Any(*)(JobHandle&)>&& aDelegate)
    : mParentJob(nullptr)
    , mDelegate(std::move(aDelegate))
    , mReturn()
    , mTotalJobs(0)
    , mUnfinishedJobs(0)
    , mAbandoned(std::make_shared<bool>(false))
  {
    IncrementJobs();
  }

  Job::Job(Delegate<Any(*)(JobHandle&)>&& aDelegate, JobHandle& aParentHandle)
    : mParentJob(aParentHandle.GetJob())
    , mDelegate(std::move(aDelegate))
    , mReturn()
    , mTotalJobs(0)
    , mUnfinishedJobs(0)
    , mAbandoned(std::make_shared<bool>(false))
  {
    IncrementJobs();
  }

  Job::~Job()
  {
    Abandon();
  }

  JobHandle Job::GetParentHandle()
  {
    return JobHandle(mParentJob);
  }

  bool Job::HasCompleted() const
  {
    return !mUnfinishedJobs;
  }

  float Job::Progress() const
  {
    return static_cast<float>(mTotalJobs - mUnfinishedJobs) / mTotalJobs;
  }

  bool Job::IsDeletable() const
  {
    return (1 == mAbandoned.use_count()) && (HasCompleted() || *mAbandoned);
  }

  Any Job::GetReturn()
  {
    return mReturn;
  }

  void Job::Invoke()
  {
    JobHandle handle(this);
    mReturn = mDelegate.Invoke(handle);
    DecrementJobs();
  }

  void Job::IncrementJobs()
  {
    ++mTotalJobs;
    ++mUnfinishedJobs;
    if (mParentJob)
    {
      mParentJob->IncrementJobs();
    }
  }

  void Job::DecrementJobs()
  {
    --mUnfinishedJobs;
    if (mParentJob)
    {
      mParentJob->DecrementJobs();
    }
  }

  void Job::Abandon()
  {
    *mAbandoned = true;
  }

  std::weak_ptr<bool> Job::GetAbandonedHandle()
  {
    return mAbandoned;
  }

}
