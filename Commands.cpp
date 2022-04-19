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
  return line; 
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
    shared_ptr<Command> cmnd (this);
    if (this->is_background)
    {
      SmallShell::getInstance().addJob(cmnd, pid, false);
    }
    else
    {
      SmallShell::getInstance().setForeground(cmnd, pid);
    }
  }
}

JobsCommand::JobsCommand(const string cmd_line, JobsList* jobs) : BuiltInCommand(cmd_line)
{
  this->jobs_list = jobs;
}

void JobsCommand::execute()
{
  this->jobs_list->printJobsList();
}

