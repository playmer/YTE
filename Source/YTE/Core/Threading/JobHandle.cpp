#include "JobHandle.hpp"
/******************************************************************************/
/*!
\author Evan T. Collier
All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

namespace YTE
{
  JobHandle::JobHandle(Job * aJob)
    : mJob(aJob)
    , mAbandoned(aJob ? aJob->GetAbandonedHandle().lock() : std::make_shared<bool>(false))
  {
  }

  bool JobHandle::HasParentHandle() const
  {
    return !mJob->GetParentHandle().IsEmpty();
  }

  JobHandle JobHandle::GetParentHandle()
  {
    return *mAbandoned ? nullptr : mJob->GetParentHandle();
  }

  bool JobHandle::HasCompleted() const
  {
    return *mAbandoned ? false : mJob->HasCompleted();
  }

  float JobHandle::Progress() const
  {
    return *mAbandoned ? false : mJob->Progress();
  }

  bool JobHandle::WasAbandoned() const
  {
    return *mAbandoned;
  }

  bool JobHandle::IsEmpty() const
  {
    return !mJob;
  }

  Any JobHandle::GetReturn()
  {
    return *mAbandoned ? Any() : mJob->GetReturn();
  }

  Job * JobHandle::GetJob()
  {
    return mJob;
  }

}

