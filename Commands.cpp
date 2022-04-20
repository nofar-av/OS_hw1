#include <unistd.h>
#include <iostream>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include "Commands.h"
#include <time.h>
#include <utime.h>



using namespace std;
#define ERROR -1
#define NO_FG -1
#define NO_ID -1

#if 0
#define FUNC_ENTRY()  \
  cout << __PRETTY_FUNCTION__ << " --> " << endl;

#define FUNC_EXIT()  \
  cout << __PRETTY_FUNCTION__ << " <-- " << endl;
#else
#define FUNC_ENTRY()
#define FUNC_EXIT()
#endif

const string WHITESPACE = " \n\r\t\f\v";

string _ltrim(const string& s)
{
  size_t start = s.find_first_not_of(WHITESPACE);
  return (start == string::npos) ? "" : s.substr(start);
}

string _rtrim(const string& s)
{
  size_t end = s.find_last_not_of(WHITESPACE);
  return (end == string::npos) ? "" : s.substr(0, end + 1);
}

string _trim(const string& s)
{
  return _rtrim(_ltrim(s));
}

int _parseCommandLine(const string &cmd_line, vector<string> &argv)
{
  FUNC_ENTRY()
  int i = 0;
  istringstream iss(_trim(cmd_line));
  for (string s; iss >> s; i++)
  {
    argv.push_back(s);
  }
  return i;

  FUNC_EXIT()
}


bool _isBackgroundComamnd(const string  cmd_line) {
  const string str(cmd_line);
  return str[str.find_last_not_of(WHITESPACE)] == '&';
}

void _removeBackgroundSign(string&  cmd_line) {
  const string str(cmd_line);
  // find last character other than spaces
  unsigned int idx = str.find_last_not_of(WHITESPACE);
  // if all characters are spaces then return
  if (idx == string::npos) {
    return;
  }
  // if the command line does not end with & then return
  if (cmd_line[idx] != '&') {
    return;
  }
  // replace the & (background sign) with space and then remove all tailing spaces.
  cmd_line[idx] = ' ';
  // truncate the command line string up to the last non-space character
  cmd_line[str.find_last_not_of(WHITESPACE, idx) + 1] = 0;
}

// TODO: Add your implementation for classes in Commands.h 
string Command::getName()
{
  return this->argv[0];
}

Command::Command(const string cmd_line)
    : line(cmd_line) {
    _parseCommandLine(cmd_line, argv);
}
BuiltInCommand::BuiltInCommand(const string cmd_line) : Command(cmd_line) {}

string Command::getCommandLine() 
{ 
  return this->line; 
}
ChangePromptCommand::ChangePromptCommand (const string cmd_line) : BuiltInCommand(cmd_line){}

void ChangePromptCommand::execute()
{
  if(argv.size() > 1)
    SmallShell::getInstance().setPrompt(argv[1]);
  else
  {
    SmallShell::getInstance().setPrompt();
  }
}

ShowPidCommand::ShowPidCommand(const string cmd_line) : BuiltInCommand(cmd_line){}

void ShowPidCommand::execute()
{
  cout << "smash pid is " << SmallShell::getInstance().getPid() << endl;
}

GetCurrDirCommand::GetCurrDirCommand(const string cmd_line) : BuiltInCommand(cmd_line){}

void GetCurrDirCommand::execute()
{
  string dir = getcwd(nullptr,0);		// memory leak?
  cout << dir << endl;
}

ChangeDirCommand::ChangeDirCommand(const string cmd_line) : BuiltInCommand(cmd_line){}

void ChangeDirCommand::execute()
{
  if(argv.size() > 2)
  {
    throw TooManyArgs(this->getName());
  }
  if(argv[1] == "-")
  {
    if(!(SmallShell::getInstance().isOldPwdSet()))
    {
      throw OldpwdNotSet(this->getName());
    }
    string oldpwd = SmallShell::getInstance().getOldPwd();
    SmallShell::getInstance().changeCurrentDirectory(oldpwd);
    return;  
  }
  SmallShell::getInstance().changeCurrentDirectory(this->argv[1]);
}

ExternalCommand::ExternalCommand(const string cmd_line) : Command(cmd_line)
{
  this->is_background = _isBackgroundComamnd(cmd_line);
  this->line_no_background = this->line;
  _removeBackgroundSign(this->line_no_background);
}

void ExternalCommand::execute() 
{
  pid_t pid = fork();
  if (pid == ERROR) //failed
  {
    throw SyscallException("fork");
  }
  else if (pid == 0) //child
  {
    if (setpgrp() == ERROR)
    {
      throw SyscallException("setgrp");
    }
    if (execlp("/bin/bash", "/bin/bash", "-c", line_no_background.c_str(), nullptr) == ERROR) {
        throw SyscallException("execv");
    }
  }
  else //father
  {
    
    if (this->is_background)
    {
      SmallShell::getInstance().addJob(this->line, pid, false);
    }
    else
    {
      SmallShell::getInstance().setFgJob(pid);
      int status;
      if (waitpid(pid, &status, WUNTRACED) == ERROR) {
        throw SyscallException("waitpid");
      }
      if (WIFSTOPPED(status)) {
        SmallShell::getInstance().addJob(this->line, pid, true);
      }
      SmallShell::getInstance().setFgJob(NO_FG);
    }
  }
}

JobsCommand::JobsCommand(const string cmd_line, shared_ptr<JobsList> jobs) : BuiltInCommand(cmd_line), jobs_list(jobs)
{}

void JobsCommand::execute()
{
  this->jobs_list->printJobsList();
}

ForegroundCommand::ForegroundCommand(const string cmd_line, shared_ptr<JobsList> jobs) : 
                                    BuiltInCommand(cmd_line), jobs_list(jobs)
{
  if(this->argv.size() > 2)
  {
    throw InvalidlArguments(this->line);
  }
  else if(this->argv.size() > 1)
  {
    this->job_id = stoi(this->argv[1]);
  }
  else
  {
    this->job_id = NO_ID;
  }
}

void ForegroundCommand::execute()
{
  if(this->job_id != NO_ID)
  {
    pid_t pid = this->jobs_list->getPid(this->job_id);
    if(pid == 0)
    {
      throw JobIdDoesntExist(this->line, this->job_id);
    }
    SmallShell::getInstance().setFgJob(pid);
    int status;
      if (waitpid(pid, &status, WUNTRACED) == ERROR) {
        throw SyscallException("waitpid");
      }
      if (WIFSTOPPED(status)) {
        SmallShell::getInstance().addJob(this->line, pid, true);
      }
      SmallShell::getInstance().setFgJob(NO_FG);//TODO: function for this
      this->jobs_list->removeFinishedJobs();
  }
  else
  {
    if(this->jobs_list->isEmpty())
    {
      throw JobsListEmpty(this->line);
    }
    pid_t pid = this->jobs_list->getMaxJobPid();
    SmallShell::getInstance().setFgJob(pid);
    int status;
      if (waitpid(pid, &status, WUNTRACED) == ERROR) {
        throw SyscallException("waitpid");
      }
      if (WIFSTOPPED(status)) {
        SmallShell::getInstance().addJob(this->line, pid, true);
      }
      SmallShell::getInstance().setFgJob(NO_FG);//TODO: function for this
      this->jobs_list->removeFinishedJobs();
  }
}