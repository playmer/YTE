#include "YTE/Core/Threading/Job.hpp"
#include "YTE/Core/Threading/JobQueue.hpp"

namespace YTE
{
  JobQueue::~JobQueue()
  {
    Flush();
  }

  void JobQueue::Push(Job* aJob)
  {
    std::lock_guard<std::mutex> guard(mQueueLock);
    mQueue.push_back(aJob);
  }

  Job* JobQueue::Pop()
  {
    std::unique_lock<std::mutex> guard(mQueueLock);
    if (mQueue.empty())
    {
      guard.unlock();
      return nullptr;
    }
    Job* ret = mQueue.back();
    mQueue.pop_back();
    guard.unlock();
    return ret;
  }

  Job* JobQueue::Steal()
  {
    std::unique_lock<std::mutex> guard(mQueueLock);
    if (mQueue.empty())
    {
      guard.unlock();
      return nullptr;
    }
    Job* ret = mQueue.front();
    mQueue.pop_front();
    guard.unlock();
    return ret;
  }

  void JobQueue::Flush()
  {
    std::lock_guard<std::mutex> guard(mQueueLock);
    for (auto& job : mQueue)
    {
      job->Abandon();
      delete job;
    }
    mQueue.clear();
  }
}
