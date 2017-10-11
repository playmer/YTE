/******************************************************************************/
/*!
\author Evan T. Collier
All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once
#include <thread>

#include "YTE/Core/Threading/Job.hpp"
#include "YTE/Core/Threading/JobHandle.hpp"
#include "YTE/Core/Threading/JobQueue.hpp"

namespace YTE
{
  class Worker
  {
  public:
    enum class WorkerState
    {
      Started,
      Idle,
      Running,
      Paused,
      Stopped
    };
    typedef std::thread::id WorkerID;
    Worker();
    ~Worker();
    virtual void Init() = 0;
    virtual void Join() = 0;
    void Pause();
    void Unpause();
    void Queue(Job* aJob);
    void Wait(JobHandle& aJob);
    void Clean();
    void AddCoworker(Worker* aWorker);
    virtual WorkerID GetID() = 0;
  protected:
    void Run();
    void YieldThread();
    void ExecuteNext();
    void SetState(WorkerState aState);

    bool mStopped;
  private:
    Job* StealFrom();
    Job* GetJob();

    std::atomic<WorkerState> mState;
    JobQueue mQueue;
    std::vector<Job*> mCompletedJobs;
    std::vector<Worker*> mCoworkers;
  };

  class BackgroundWorker : public Worker
  {
  public:
    BackgroundWorker();
    ~BackgroundWorker();
    virtual void Init() override;
    virtual void Join() override;
    virtual WorkerID GetID() override;
  private:
    std::thread mThread;
  };

  class ForegroundWorker : public Worker
  {
  public:
    ForegroundWorker(bool aAsync);
    virtual void Init() override;
    virtual void Join() override;
    virtual WorkerID GetID() override;
    void RunForeground();
  private:
    WorkerID mID;
    bool mAsync;
  };
}
