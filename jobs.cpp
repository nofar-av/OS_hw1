//#include <iostream>
#include "jobs.h"
//#include "small_shell.h"
#include "Commands.h"
using namespace std;

#define ERROR -1

time_t _getTime() {
    time_t curr_time;
    if (time(&curr_time) == ERROR) {
        throw SyscallException("time");
    }
    return curr_time;
}

JobEntry::JobEntry(int job_id, pid_t pid, shared_ptr<Command> command,
                   time_t insertion_time, JobStatus job_status)
    : job_id(job_id), pid(pid), command(command),
      insertion_time(insertion_time), job_status(job_status) {}

bool JobEntry::isFinished() const
{
    return (this->job_status == FINISHED);
}

bool JobEntry::isStopped() const
{
    return (this->job_status == STOPPED);
}

void JobEntry::print(bool full_print) const
{
    cout<<"["<<this->job_id<<"]"<< this->command->getCommandLine() << " : "<<this->pid << this->insertion_time<< " secs";
    if (this->isStopped())
    {
        cout<<" (stopped)";
    }
    cout<<endl;
}

pid_t JobEntry::getPid ()
{
    return this->pid;
}

int JobEntry::getJobId()
{
    return this->job_id;
}

JobsList::JobsList() : job_entries(), max_job_id(EMPTY_JOB_ID) {}

void JobsList::addJob(shared_ptr<Command> command, pid_t pid,  bool is_stopped)
{
    int job_id = this->max_job_id + 1;
    this->max_job_id ++;
    time_t time = _getTime();
    JobStatus status = (is_stopped) ? STOPPED : BG_ACTIVE;
    shared_ptr<JobEntry> new_job (new JobEntry(job_id, pid, command, time, status));
    this->job_entries.push_back(shared_ptr<JobEntry>(new_job));
}
//TODO: do we need pointers in the vector?
void JobsList::printJobsList()
{
    for (size_t i = 0; i < this->job_entries.size(); i++)
    {
        this->job_entries[i]->print();
    }
}

//TODO:: how to check if a job is finished
void JobsList::removeFinishedJobs()
{
    for (auto it = this->job_entries.begin(); it != this->job_entries.end(); it++)
    {
        if ((*it)->isFinished())
        {
            int job_id = (*it)->getJobId();
            if (job_id == this->max_job_id - 1)
            {
                this->max_job_id -- ;
            }
            this->job_entries.erase(it);
        }
    }
}

int JobsList::getFGJobID()
{
    this->max_job_id++;
    return this->max_job_id;
}

void JobsList::removeFgJob()
{
    this->max_job_id--;
}