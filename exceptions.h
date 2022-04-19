#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

#include <exception>
#include <typeinfo>
#include <string>
#include <memory>

#include "Commands.h"
using namespace std;


class SmashException : public exception //std::exception
{};

<<<<<<< HEAD
class TooManyArgs : public SmashException{
=======
class TooManyArgs : public SmashException
{
>>>>>>> main
    string error_message;
public:
    TooManyArgs(const string cmd_name) 
    {
        this->error_message = "smash error:" + cmd_name + ":" + "too many arguments" + "\n"; 
    }
    ~TooManyArgs() = default;
    const char* what() const noexcept override
    {
        return error_message.c_str();
    }
};

<<<<<<< HEAD
class OldpwdNotSet : public SmashException {
=======
class OldpwdNotSet : public SmashnException
{
>>>>>>> main
    string error_message;
public:
    OldpwdNotSet(const string cmd_name)
    {
        this->error_message = "smash error:" + cmd_name + ":" + " too many arguments" + "\n"; 
    }    
    const char* what() const noexcept override { return error_message.c_str(); }
};

<<<<<<< HEAD
class SyscallException : public exception {
=======
class SyscallException : public SmashException {
>>>>>>> main
    string error_message;
  public:
    SyscallException(string syscall_name) 
    {
        this->error_message = "smash error: " + syscall_name + " failed";
    }
    const char* what() const noexcept override { return error_message.c_str(); }
};

<<<<<<< HEAD
class InvalidlArguments : public SmashException {
=======
class InvalidlArguments : public SmashException
{
>>>>>>> main
    string error_message;
public:
    InvalidlArguments(const string cmd_name)
    {
        this->error_message = "smash error:" + cmd_name + ":" + " invalid arguments" + "\n"; 
    }    
    const char* what() const noexcept override { return error_message.c_str(); }
};

<<<<<<< HEAD
class JobIdDoesntExist : public SmashException {
=======
class JobIdDoesntExist : public SmashException
{
>>>>>>> main
    string error_message;
public:
    JobIdDoesntExist(const string cmd_name, int job_id)
    {
<<<<<<< HEAD
        this->error_message = "smash error:" + cmd_name + ":" + "job-id" + to_string(job_id) + "does not exist" + "\n"; 
    }    
    const char* what() const noexcept override { return error_message.c_str(); }
};

class JobAlreadyRunBG : public SmashException {
=======
        this->error_message = "smash error:" + cmd_name + ":" + "job-id" + job_id + "does not exist" + "\n"; 
    }    
    const char* what() const noexcept override { return error_message.c_str(); }
};
class JobAlreadyRunBG : public SmashException
{
>>>>>>> main
    string error_message;
public:
    JobAlreadyRunBG(const string cmd_name, int job_id)
    {
<<<<<<< HEAD
        this->error_message = "smash error:" + cmd_name + ":" + "job-id" + to_string(job_id) + "is already running in the background" + "\n"; 
=======
        this->error_message = "smash error:" + cmd_name + ":" + "job-id" + job_id + "is already running in the background" + "\n"; 
>>>>>>> main
    }  
    const char* what() const noexcept override { return error_message.c_str(); }
};


<<<<<<< HEAD
class NoStoppedJobs : public SmashException {
=======
class NoStoppedJobs : public SmashException
{
>>>>>>> main
    string error_message;
public:
    NoStoppedJobs(const string cmd_name)
    {
        this->error_message = "smash error:" + cmd_name + ":" + "there is no stopped jobs to resume" + "\n"; 
    }  
    const char* what() const noexcept override { return error_message.c_str(); }
};


#endif /*EXCEPTIONS_H*/