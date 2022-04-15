#include "exceptions.h"

using namespace std;


const char* SmashException::what() const noexcept
{
    return error_message.c_str();
}
SmashException::SmashException(const string cmd_name)
{ 
    this->error_message = "smash error:" + cmd_name + ":" + this->error_message + "\n"; 
}
InvalidlArguments::InvalidlArguments(const string cmd_name) 
{
    this->error_message = "invalid arguments";
    SmashException(cmd_name);
}
JobIdDoesntExist::JobIdDoesntExist(const string cmd_name, int job_id)
{
    this->error_message = "job-id" + job_id + "does not exist";
    SmashException::SmashException(cmd_name);
}
JobAlreadyRunBG::JobAlreadyRunBG(const string cmd_name, int job_id)
{
    this->error_message = "job-id" + job_id + "is already running in the background";
    SmashException::SmashException(cmd_name);
}
OldpwdNotSet::OldpwdNotSet(const string cmd_name)
{
    this->error_message = "OLDPWD not set";
    SmashException::SmashException(cmd_name);
}
NoStoppedJobs::NoStoppedJobs(const string cmd_name)
{
    this->error_message = "there is no stopped jobs to resume";
    SmashException::SmashException(cmd_name);
}

TooManyArgs::TooManyArgs(const string cmd_name)
{
    this->error_message = "too many arguments";
    SmashException::SmashException(cmd_name);
}