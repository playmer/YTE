/******************************************************************************/
/*!
\author Evan T. Collier
All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#include <deque>
#include <mutex>

#include "YTE/Core/Threading/Job.hpp"

//TODO (Evan): Replace JobQueue with circular array
// less locks and more performant
namespace YTE
{
  class JobQueue
  {
  public:
    ~JobQueue();
    void Push(Job* aJob);
    Job* Pop();
    Job* Steal();
    void Flush();
  private:
    std::mutex mQueueLock;
    std::deque<Job*> mQueue;
  };

}
