#include "exceptions.h"
#include <errno.h>
using namespace std;


const char* BuiltInException::what() const noexcept
{
    return error_message.c_str();
}
BuiltInException::BuiltInException(const string cmd_name)
{ 
    this->error_message = "smash error:" + cmd_name + ":" + this->error_message + "\n"; 
}
SyscallException::SyscallException(string syscall_name) {
        this->error_message = "smash error: " + syscall_name + " failed";
}
InvalidlArguments::InvalidlArguments(const string cmd_name) : error_message("invalid arguments"),
                                                              BuiltInException(cmd_name){}

OldpwdNotSet::OldpwdNotSet(const string cmd_name) : error_message("OLDPWD not set"), 
                                                  BuiltInException(cmd_name){}

TooManyArgs::TooManyArgs(const string cmd_name) : error_message("too many arguments"), 
                                                  BuiltInException(cmd_name){}

JobIdDoesntExist::JobIdDoesntExist(const string cmd_name, int job_id)
{
    this->error_message = "job-id" + job_id + "does not exist";
    BuiltInException::BuiltInException(cmd_name);
}

JobAlreadyRunBG::JobAlreadyRunBG(const string cmd_name, int job_id)
{
    this->error_message = "job-id" + job_id + "is already running in the background";
    BuiltInException::BuiltInException(cmd_name);
}

NoStoppedJobs::NoStoppedJobs(const string cmd_name)
{
    this->error_message = "there is no stopped jobs to resume";
    BuiltInException::BuiltInException(cmd_name);
}
