#ifndef JOBS_H_
#define JOBS_H_

#include <memory>
#include <string>
#include <unordered_map>

//#include "Commands.h"
using namespace std;

#define EMPTY_JOB_ID 0
enum JobStatus { STOPPED, FINISHED, BG_ACTIVE };

class Command;

class JobEntry {
    int job_id;
    pid_t pid;
    shared_ptr<Command> command;
    time_t insertion_time;
    JobStatus job_status;
  public:
    JobEntry(int job_id, pid_t pid, shared_ptr<Command>command, time_t insertion_time, JobStatus job_status);
    ~JobEntry() = default;
    bool isFinished();
    bool isStopped() const;
    void print(bool full_print = true) const;
    pid_t getPid ();
    int getJobId();
};

class JobsList {
    unordered_map<int, shared_ptr<JobEntry>> job_entries;
    int max_job_id;
 // TODO: Add your data members
 public:
  JobsList();
  ~JobsList() = default;
  void addJob(shared_ptr<Command> command, pid_t pid,  bool is_stopped = false);
  void printJobsList();
  void killAllJobs();
  void removeFinishedJobs();
  shared_ptr<JobEntry> getJobById(int jobId);
  void removeJobById(int jobId);
  shared_ptr<JobEntry> getLastJob(int& lastJobId);
  shared_ptr<JobEntry> getLastStoppedJob(int& jobId);
  void removeFgJob();
  int getFGJobID();
  // TODO: Add extra methods or modify exisitng ones as needed
};

#endif //JOBS_H_