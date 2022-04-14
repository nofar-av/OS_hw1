#include <iostream>
#include "jobs.h"
#include "small_shell.h"

using namespace std;

#define ERROR -1

time_t _getTime() {
    time_t curr_time;
    if (time(&curr_time) == ERROR) {
        throw SyscallException("time");
    }
    return curr_time;
}

JobsList::JobEntry::JobEntry(int job_id, int pid, shared_ptr<Command> command,
                   time_t insertion_time, JobStatus job_status)
    : job_id(job_id), pid(pid), command(command),
      insertion_time(insertion_time), job_status(job_status) {}

bool JobsList::JobEntry::isFinished() const
{
    return (this->job_status == FINISHED);
}

bool JobsList::JobEntry::isStopped() const
{
    return (this->job_status == STOPPED);
}

void JobsList::JobEntry::print(bool full_print = true) const
{
    cout<<"["<<this->job_id<<"]"<<endl;
}

JobsList::JobsList() : job_entries(), max_job_id(EMPTY_JOB_ID) {}

void JobsList::addJob(shared_ptr<Command> command, int pid,  bool isStopped = false)
{
    int job_id = this->max_job_id + 1;
    this->max_job_id ++;
    //JobEntry new_job = new JobEntry(job_id, )
    this->job_entries.push_back(new_job);
}