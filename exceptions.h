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
{
protected:
    string error_message;
public:
    SmashException(const string cmd_name);
    SmashException()  = default;
    ~SmashException() = default;
    virtual const char* what() const noexcept override;
};
class InvalidlArguments : public SmashException
{
public:
    InvalidlArguments(const string cmd_name);
    ~InvalidlArguments() = default;
};
class JobIdDoesntExist : public SmashException
{
public:
    JobIdDoesntExist(const string cmd_name, int job_id);
    ~JobIdDoesntExist() = default;
};
class JobAlreadyRunBG : public SmashException
{
public:
    JobAlreadyRunBG(const string cmd_name, int job_id);
    ~JobAlreadyRunBG() = default;
};

class OldpwdNotSet : public SmashException
{
public:
    OldpwdNotSet(const string cmd_name);
    ~OldpwdNotSet() = default;
};

class NoStoppedJobs : public SmashException
{
public:
    NoStoppedJobs(const string cmd_name);
    ~NoStoppedJobs() = default;
};
class TooManyArgs : public SmashException
{
public:
    TooManyArgs(const string cmd_name);
    ~TooManyArgs() = default;
};

#endif /*EXCEPTIONS_H*/