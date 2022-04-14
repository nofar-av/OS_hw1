#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

#include <exception>
#include <typeinfo>
#include <string>


class Exception : public std::exception
{
private:
    std::string error_message;
public:
    Exception(JobEntry& job);
    Exception() {};
    ~Exception() {};
    virtual const char* what() const noexcept override;
};
class InvalidlArguments : public Exception
{
public:
    InvalidlArguments() : Exception(JobEntry& job);
    ~InvalidlArguments() = default;
};
class JobIdDoesntExist : public Exception   
{
public:
    JobIdDoesntExist() : Exception(JobEntry& job);
    ~JobIdDoesntExist() = default;
};
class JobAlreadyRunBG : public Exception
{
public:
    JobAlreadyRunBG() : Exception(JobEntry& job);
    ~JobAlreadyRunBG() = default;
};

class OldpwdNotSet : public Exception
{
public:
    OldpwdNotSet() : Exception(JobEntry& job);
    ~OldpwdNotSet() = default;
};

class NoStoppedJobs : public Exception
{
public:
    NoStoppedJobs() : Exception(JobEntry& job);
    ~NoStoppedJobs() = default;
};
class TooManyArgs : public Exception
{
public:
    TooManyArgs() : Exception(JobEntry& job);
    ~TooManyArgs() = default;
};

#endif /*EXCEPTIONS_H*/