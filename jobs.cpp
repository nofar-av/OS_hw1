//#include <iostream>
#include "jobs.h"
//#include "small_shell.h"
#include "Commands.h"
using namespace std;
#include <vector>

#define ERROR -1
#define DOSENT_EXIST 0

time_t _getTime() {
    time_t curr_time;
    if (time(&curr_time) == ERROR) {
        throw SyscallException("time");
    }
    return curr_time;
}

JobEntry::JobEntry(int job_id, pid_t pid, string cmd_line,
                   time_t insertion_time, JobStatus job_status)
    : job_id(job_id), pid(pid), cmd_line(cmd_line),
      insertion_time(insertion_time), job_status(job_status) {}

bool JobEntry::isFinished() 
{
    if (waitpid(this->pid, nullptr, WNOHANG) != 0)
    {
        this->job_status = FINISHED;
    }
    return (this->job_status == FINISHED);
}

bool JobEntry::isStopped() const
{
    return (this->job_status == STOPPED);
}


void JobEntry::print(bool full_print) const
{
    double diff = difftime(_getTime(), this->insertion_time);
    cout<<"["<<this->job_id<<"] "<< this->cmd_line << " : "<< this->pid << " " << diff << " secs";
    if (this->isStopped())
    {
        cout<<" (stopped)";
    }
    cout << endl;
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

void JobsList::addJob(string cmd_line, pid_t pid,  bool is_stopped)
{
    this->max_job_id ++;
    int job_id = this->max_job_id;
    time_t time = _getTime();
    JobStatus status = (is_stopped) ? STOPPED : BG_ACTIVE;
    shared_ptr<JobEntry> new_job (new JobEntry(job_id, pid, cmd_line, time, status));
    this->job_entries[job_id] = new_job;
}

void JobsList::printJobsList() 
{
    this->removeFinishedJobs();
    for (auto it = this->job_entries.begin(); it != this->job_entries.end(); it++)
    {
        (*it->second).print();
    }
}

void JobsList::removeFinishedJobs()
{
    int max = EMPTY_JOB_ID;
    vector<int> jobs_to_erase;
    for (auto it = this->job_entries.begin(); it != this->job_entries.end(); it++)
    {
        if ((*(it->second)).isFinished())
        {
            jobs_to_erase.push_back(it->first);
        }
        else
        {
            max = (max < (it->first)) ? (it->first) : max;
        }
    }
    this->max_job_id = max;
    for (auto it = jobs_to_erase.begin(); it != jobs_to_erase.end(); it++)
    {
        this->job_entries.erase(*it);
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
pid_t JobsList::getPid(int job_id)
{
    if (this->job_entries.find(job_id) == this->job_entries.end())
    {
        return DOSENT_EXIST;
    }
    return this->job_entries[job_id]->getPid();//TODO: Eficciency
}
bool JobsList::isEmpty() const
{
    return this->job_entries.empty();
}
pid_t JobsList::getMaxJobPid() 
{
    return this->job_entries[this->max_job_id]->getPid();
}