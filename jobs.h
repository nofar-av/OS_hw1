#ifndef JOBS_H_
#define JOBS_H_

#include <memory>
#include <string>
#include <vector>

#include "Commands.h"
using namespace std;

#define EMPTY_JOB_ID -1
enum JobStatus { STOPPED, FINISHED, BG_ACTIVE };

class JobsList {
    vector<shared_ptr<JobEntry>> job_entries;
    int max_job_id;
 public:
  class JobEntry {
  public:
    int job_id;
    int pid;
    shared_ptr<Command> command;
    time_t insertion_time;
    JobStatus job_status;
    JobEntry(int job_id, int pid, shared_ptr<Command>command, time_t insertion_time, JobStatus job_status);
    ~JobEntry() = default;
    bool isFinished() const;
    bool isStopped() const;
    void print(bool full_print = true) const;
  };
 // TODO: Add your data members
 public:
  JobsList();
  ~JobsList() = default;
  void addJob(shared_ptr<Command> command, int pid,  bool isStopped = false);
  void printJobsList();
  void killAllJobs();
  void removeFinishedJobs();
  JobEntry * getJobById(int jobId);
  void removeJobById(int jobId);
  JobEntry * getLastJob(int* lastJobId);
  JobEntry *getLastStoppedJob(int *jobId);
  // TODO: Add extra methods or modify exisitng ones as needed
};

#endif //JOBS_H_