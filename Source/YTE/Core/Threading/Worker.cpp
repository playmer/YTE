/******************************************************************************/
/*!
\author Evan T. Collier
All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "YTE/Core/Threading/Worker.hpp"

namespace YTE
{
  Worker::Worker()
    : mState(WorkerState::Started), mStopped(false)
  {
  }

  Worker::~Worker()
  {
    for (auto& job : mCompletedJobs) {
      job->Abandon();
      delete job;
    }
  }

  void Worker::Pause()
  {
    SetState(WorkerState::Paused);
  }

  void Worker::Unpause()
  {
    SetState(WorkerState::Started);
  }

  void Worker::Queue(Job* aJob)
  {
    mQueue.Push(aJob);
  }

  void Worker::Wait(JobHandle& aJob)
  {
    while (!aJob.HasCompleted())
    {
      ExecuteNext();
    }
  }

  void Worker::Clean()
  {
    std::vector<Job*> mNotDeletedJobs;
    for (auto& job : mCompletedJobs)
    {
      if (job->IsDeletable())
      {
        delete job;
        job = nullptr;
      }
      else
      {
        mNotDeletedJobs.push_back(job);
      }
    }

    mCompletedJobs = mNotDeletedJobs;
  }

  void Worker::AddCoworker(Worker * aWorker)
  {
    mCoworkers.push_back(aWorker);
  }

  void Worker::Run()
  {
    while (mState != WorkerState::Stopped)
    {
      ExecuteNext();
    }

    mStopped = true;
  }

  void Worker::YieldThread()
  {
    // TODO (Evan): Time tasks to figure out sleep length
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }

  void Worker::ExecuteNext()
  {
    if (mState == WorkerState::Paused)
    {
      YieldThread();
      return;
    }

    auto job = GetJob();
    if (job)
    {
      SetState(WorkerState::Running);
      job->Invoke();
      mCompletedJobs.push_back(job);
    }
    else
    {
      YieldThread();
    }
    SetState(WorkerState::Idle);
  }

  void Worker::SetState(WorkerState aState)
  {
    if (mState != WorkerState::Stopped) {
      mState = aState;
    }
  }

  Job* Worker::StealFrom()
  {
    return mQueue.Steal();
  }

  Job* Worker::GetJob()
  {
    Job* job = mQueue.Pop();
    if (job)
    {
      return job;
    }
    else if (!mCoworkers.empty())
    {
      // TODO(Evan): make choosing the coworker to rob smarter
      int index = std::rand() % mCoworkers.size();
      return mCoworkers[index]->StealFrom();
    }

    // TODO(Evan): Is this what it should do?
    return nullptr;
  }


  BackgroundWorker::BackgroundWorker()
    : mThread()
  {
  }

  BackgroundWorker::~BackgroundWorker()
  {
    Join();
  }

  void BackgroundWorker::Init()
  {
    mThread = std::thread([this]() { Run(); });
  }

  void BackgroundWorker::Join()
  {
    while (!mStopped)
    {
      SetState(WorkerState::Stopped);
      YieldThread();
    }

    mThread.join();
  }

  Worker::WorkerID BackgroundWorker::GetID()
  {
    return mThread.get_id();
  }


  ForegroundWorker::ForegroundWorker(bool aAsync)
    : mAsync(aAsync), mID(std::this_thread::get_id())
  {
  }

  void ForegroundWorker::Init()
  {
  }

  void ForegroundWorker::Join()
  {
    SetState(WorkerState::Stopped);
  }

  Worker::WorkerID ForegroundWorker::GetID()
  {
    return mID;
  }

  void ForegroundWorker::RunForeground()
  {
    ExecuteNext();
  }
}
