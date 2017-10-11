/******************************************************************************/
/*!
\author Evan T. Collier
All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#include "YTE/Core/Threading/Job.hpp"

namespace YTE
{
  class JobHandle
  {
    friend class Job;
  public:
    JobHandle(Job* aJob);
    bool HasParentHandle() const;
    JobHandle GetParentHandle();
    bool HasCompleted() const;
    float Progress() const;
    bool WasAbandoned() const;
    bool IsEmpty() const;
    Any GetReturn();

  private:
    Job* GetJob();

    Job* mJob;
    std::shared_ptr<bool> mAbandoned;
  };
}
