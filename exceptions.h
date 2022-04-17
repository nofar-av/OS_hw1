#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

#include <exception>
#include <typeinfo>
#include <string>
#include <memory>

#include "jobs.h"
#include "Commands.h"
using namespace std;


class SmashException : public std::exception
{};
class BuiltInException : public SmashException {
protected:
    string error_message;
public:
    BuiltInException(const string cmd_name);
    BuiltInException()  = default;
    ~BuiltInException() = default;
    virtual const char* what() const noexcept override;
};
class SyscallException : public SmashException {
  private:
    string error_message;

  public:
    SyscallException(string syscall_name);
    const char *what() const noexcept override { return error_message.c_str(); }
};
class InvalidlArguments : public BuiltInException
{
public:
    InvalidlArguments(const string cmd_name);
    ~InvalidlArguments() = default;
};
class JobIdDoesntExist : public BuiltInException
{
public:
    JobIdDoesntExist(const string cmd_name, int job_id);
    ~JobIdDoesntExist() = default;
};
class JobAlreadyRunBG : public BuiltInException
{
public:
    JobAlreadyRunBG(const string cmd_name, int job_id);
    ~JobAlreadyRunBG() = default;
};

class OldpwdNotSet : public BuiltInException
{
public:
    OldpwdNotSet(const string cmd_name);
    ~OldpwdNotSet() = default;
};

class NoStoppedJobs : public BuiltInException
{
public:
    NoStoppedJobs(const string cmd_name);
    ~NoStoppedJobs() = default;
};
class TooManyArgs : public BuiltInException
{
public:
    TooManyArgs(const string cmd_name);
    ~TooManyArgs() = default;
};

#endif /*EXCEPTIONS_H*/