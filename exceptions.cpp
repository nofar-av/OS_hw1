#include "exceptions.h"


const char* Exception::what() const noexcept
{
    return error_message.c_str();
}
Exception::Exception(JobEntry& job)
{ 
    error_message = "smash error:" + job.command.GetName() + ":" + error_message + "\n"; 
}
InvalidlArguments::InvalidlArguments() : Exception(JobEntry& job)
{
    error_message = "invalid arguments";
    Exception::Exception(job);
}
JobIdDoesntExist::JobIdDoesntExist() : Exception(JobEntry& job)
{
    error_message = "job-id" + job.JobId() + "does not exist";
    Exception::Exception(job);
}
JobAlreadyRunBG::JobAlreadyRunBG() : Exception(JobEntry& job)
{
    error_message = "job-id" + job.JobId() + "is already running in the background";
    Exception::Exception(job);
}
OldpwdNotSet::OldpwdNotSet() : Exception(JobEntry& job)
{
    error_message = "OLDPWD not set";
    Exception::Exception(job);
}
NoStoppedJobs::NoStoppedJobs() : Exception(JobEntry& job)
{
    error_message = "there is no stopped jobs to resume";
    Exception::Exception(job);
}

TooManyArgs::TooManyArgs() : Exception(JobEntry& job)
{
    error_message = "too many arguments";
    Exception::Exception(job);
}