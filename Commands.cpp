#include <unistd.h>
#include <iostream>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include "Commands.h"
#include <time.h>
#include <utime.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>



using namespace std;
#define ERROR -1
#define NO_FG -1
#define NO_ID -1
#define N 10

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


bool _isBackgroundComamnd(const string cmd_line) {
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

bool is_num (const string& str)
{
  return !str.empty() && str.find_first_not_of("0123456789") == string::npos;
}

// TODO: Add your implementation for classes in Commands.h 
void runInFg(pid_t pid, const string& line, int job_id)
{
  SmallShell& smash = SmallShell::getInstance(); 
  smash.setFgJob(pid, line, job_id);
  int status;
  if (waitpid(pid, &status, WUNTRACED) == ERROR) {
    throw SyscallException("waitpid");
  }
  if (WIFSTOPPED(status)) {
    smash.addJob(line, pid, true, job_id);
  }
  smash.setFgJob();//TODO: function for this
  smash.getJobs()->removeFinishedJobs();
}

void Command::removeRedirectionPart()
{
  size_t found = this->line.find(">");
  if(found == string::npos)
  {
    found = this->line.find(">>"); 
  }
  this->line = this->line.substr(0,found);
  vector<string>::iterator it;
  for (it = argv.begin(); it != argv.end(); it++)
  {
    if(*it == ">" || *it == ">>")
    {
      break;
    }
  }
  vector<string> subargv = {argv.begin() , it};
  this->argv = subargv;
}

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

ChangeDirCommand::ChangeDirCommand(const string cmd_line) : BuiltInCommand(cmd_line)
{
  if(argv.size() > 2)
  {
    throw TooManyArgs(this->getName());
  }
}

void ChangeDirCommand::execute()
{
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
    SmallShell& smash = SmallShell::getInstance();
    if (this->is_background)
    {
      smash.addJob(this->line, pid, false);
    }
    else
    {
      runInFg(pid, this->line, smash.getJobs()->getFGJobID());
    }
  }
}
PipeCommand::PipeCommand(const string cmd_line) : Command(cmd_line)
{
  this->pipe_err = (this->line.find("|&") != string::npos);
  size_t index_pipe = this->line.find("|");
  this->left_cmd = this->line.substr(0,index_pipe);
  this->right_cmd = this->line.substr(index_pipe + 1 + this->pipe_err);
}

void PipeCommand::execute()
{
  SmallShell& smash = SmallShell::getInstance();
  shared_ptr<Command> left = smash.createCommand(this->left_cmd);
  shared_ptr<Command> right = smash.createCommand(this->right_cmd);
  int fd[2];
  if(pipe(fd) == ERROR)
  {
    throw SyscallException("pipe");
  }
  pid_t left_pid, right_pid;
  left_pid = fork();
  if(left_pid < 0)
  {
    throw SyscallException("fork");
  }
  else if(left_pid == 0)//left cmd
  {
    if (setpgrp() == ERROR)
    {
      throw SyscallException("setgrp");
    }
    int fd_to_close = this->pipe_err ? 2 : 1;
    if(close(fd_to_close) == ERROR)
    {
      throw SyscallException("close");
    }
    if(dup2(fd[1],fd_to_close) == ERROR)
    {
      throw SyscallException("dup2");
    }
    if(close(fd[1]) == ERROR)
    {
      throw SyscallException("close");
    }
    left->execute();
  
    exit(0);
  }
  else
  {
    right_pid = fork();
    if(right_pid < 0)
    {
      throw SyscallException("fork");
    }
    if(right_pid == 0)//right cmd
    {
      if (setpgrp() == ERROR)
      {
        throw SyscallException("setgrp");
      }
      int fd_back_up = dup(0);
      if(close(0) == ERROR)
      {
        throw SyscallException("close");
      }
      if(dup2(fd[0],0) == ERROR)
      {
        throw SyscallException("dup2");
      }
      if(close(fd[0]) == ERROR)
      {
        throw SyscallException("close");
      }
      right->execute();
      if(close(0) == ERROR)
      {
        throw SyscallException("close");
      }

      exit(0);
    }
  }
  close(fd[0]);
  close(fd[1]);
  
}
RedirectionCommand::RedirectionCommand(const string cmd_line) : Command(cmd_line)
{
  this->cat = (this->line.find(">>") != string::npos);
  size_t index_end_cmd = this->line.find(">");
  this->cmd = this->line.substr(0,index_end_cmd);
  string str = this->cat ? ">>" : ">";
  vector<string>::iterator it = find(this->argv.begin(),this->argv.end(), str);
  this->filename = *(++it);
}

void RedirectionCommand::execute()
{
  SmallShell& smash = SmallShell::getInstance();
  shared_ptr<Command> cmd = smash.createCommand(this->cmd);
  pid_t pid = fork();
  if(pid < 0)
  {
    throw SyscallException("fork");
  }
  if(pid == 0)
  {
    if (setpgrp() == ERROR)
    {
      throw SyscallException("setgrp");
    }
    dup(1);
    close(1);
    int fd;
    if (this->cat)
    {
      fd = open(this->filename.c_str(), O_APPEND, 00777);//check if success
      if (fd == ERROR)
      {
        this->cat = false;
      }
    }
    if(!this->cat)
    {
      fd = open(this->filename.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 00777);
      if (fd == ERROR)
      {
        throw SyscallException("open");
      }
    }
    cmd->execute();
    close(fd);
    exit(0);
  }
}

JobsCommand::JobsCommand(const string cmd_line, shared_ptr<JobsList> jobs) : BuiltInCommand(cmd_line), jobs_list(jobs)
{}

void JobsCommand::execute()
{
  this->jobs_list->printJobsList();
}
KillCommand::KillCommand(const string cmd_line, shared_ptr<JobsList> jobs) : 
                                    BuiltInCommand(cmd_line), jobs_list(jobs)
{
  if (argv.size() != 3 || (argv[1])[0] != '-' || !is_num(this->argv[2] ))
  {
    throw InvalidlArguments(this->line);
  }
  if(!is_num(argv[1].substr(1)))
  {
    throw InvalidlArguments(this->line);
  }
}

void KillCommand::execute()
{
  int job_id = stoi(argv[2]);
  int signum = stoi(argv[1].substr(1));
  if(signum <= 0 || signum > 31)
  {
    throw InvalidlArguments(this->line);
  }
  shared_ptr<JobEntry> job = SmallShell::getInstance().getJobs()->getJobById(job_id);
  if(job == nullptr)
  {
    throw JobIdDoesntExist(this->argv[0], job_id);
  }
  if (kill(job->getPid(), signum) == ERROR)
  {
    throw SyscallException("kill");
  }
  cout << "signal number " << signum << " was sent to pid " << job->getPid() << endl;
}

ForegroundCommand::ForegroundCommand(const string cmd_line, shared_ptr<JobsList> jobs) : 
                                    BuiltInCommand(cmd_line), jobs_list(jobs)
{
  if(this->argv.size() > 2 || (this->argv.size() > 1 && !is_num(this->argv[1])))
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
  shared_ptr<JobEntry> job;
  if(this->job_id != NO_ID)
  {
    job = this->jobs_list->getJobById(this->job_id);
    if(job == nullptr)
    {
      throw JobIdDoesntExist(this->argv[0], this->job_id);
    }
  }
  else
  {
    if(this->jobs_list->isEmpty())
    {
      throw JobsListEmpty(this->line);
    }
    job = this->jobs_list->getMaxJob();
  }
  cout << job->getLine() << " : " << job->getPid() << endl;
  job->activate();
  this->jobs_list->removeJobById(this->job_id);
  runInFg(job->getPid(), job->getLine(), job->getJobId());
}

BackgroundCommand::BackgroundCommand(const string cmd_line, shared_ptr<JobsList> jobs) : 
                                    BuiltInCommand(cmd_line), jobs_list(jobs)
{
  if(this->argv.size() > 2)
  {
    throw InvalidlArguments(this->line);
  }
  else if(this->argv.size() == 1)
  {
    this->job_id = NO_ID;
  }
  else if (!is_num(this->argv[1]))
  {
    throw InvalidlArguments(this->line);
  }
  else
  {
    this->job_id = stoi(this->argv[1]);
  }
}

void BackgroundCommand::execute() 
{
  shared_ptr<JobEntry> job;
  if (this->job_id == NO_ID)
  {
    job = this->jobs_list->getLastStoppedJob();
    if (job == nullptr)
    {
      throw NoStoppedJobs(this->line);
    }
  }
  else
  {
    job = this->jobs_list->getJobById(this->job_id);
    if (job == nullptr)
    {
      throw JobIdDoesntExist(this->argv[0], this->job_id);
    }
    bool is_running = this->jobs_list->isJobRunning(this->job_id);
    if (!is_running)
    {
      throw JobAlreadyRunBG(this->argv[0], this->job_id);
    }
  }
  cout<< job->getLine() << " : " << job->getPid() << endl;
  job->activate();
  //now continue job and change status in the list
}
QuitCommand::QuitCommand(const string cmd_line, shared_ptr<JobsList> jobs) : 
                                    BuiltInCommand(cmd_line), jobs_list(jobs)
{}

void QuitCommand::execute()
{
  if(this->argv.size() > 1 && this->argv[1] == "kill")
  {
    this->jobs_list->killAllJobs();
  }
  SmallShell::getInstance().stopRun();
}

TailCommand::TailCommand(const string cmd_line) : BuiltInCommand(cmd_line)
{
  if(this->argv.size() > 3 || this->argv.size() < 2)
  {
    throw InvalidlArguments(cmd_line);
  }
  if(this->argv.size() == 3)
  {
    if(!is_num(this->argv[1].substr(1)))
    {
      throw InvalidlArguments(cmd_line);
    }
    this->num_lines = stoi(this->argv[1].substr(1));
    if(this->num_lines < 0)
    {
      throw InvalidlArguments(cmd_line);
    }
    this->file_name = this->argv[2];
  }
  else
  {
    this->num_lines = N;
    this->file_name = this->argv[1];
  }
}

void TailCommand::execute()
{
  int fd = open(this->file_name.c_str(), O_RDONLY);
  if(fd == ERROR)
  {
    throw SyscallException("open");
  }
  int fd_faster = open(this->file_name.c_str(), O_RDONLY);
  if(this->ReadNLines(this->num_lines, fd_faster) == ERROR) //file shorter than N lines
  {
    this->ReadNLines(this->num_lines, fd, 1);
  }
  else 
  {
    while(this->ReadNLines(1, fd_faster) != ERROR)
    {
      this->ReadNLines(1, fd);
    }
    this->ReadNLines(this->num_lines, fd, 1);
  }
  if(close(fd) == ERROR)
  {
    close(fd_faster);
    throw SyscallException("close");
  }
  if(close(fd_faster) == ERROR)
  {
    throw SyscallException("close");
  }
}
int TailCommand::ReadNLines(int lines, int fd_read, int fd_write)
{
  int i = 0;
  char ch;
  ssize_t rv = read(fd_read, &ch, 1);
  while(rv != ERROR && i < lines)
  {
    if(rv == 0) //EOF
    {
        return -1;
    }
    else if(ch == '\n')
    {
        i++;
    }
    if(fd_write != -1 && write(fd_write, &ch, 1) == ERROR)
    {
      throw SyscallException("write");
    }
    if(i == lines)
    {
      return rv;
    }
    rv = read(fd_read, &ch, 1);
    if(rv == ERROR)
    {
      throw SyscallException("read");
    }
  }
  return rv;
}

TouchCommand::TouchCommand(const string cmd_line) :  BuiltInCommand(cmd_line), timestamp()
{
  if(this->argv.size() != 3)
  {
    throw InvalidlArguments(this->line);
  }
  this->file_name = this->argv[1];
  strptime(this->argv[2].c_str(), "%S:%M:%H:%d:%m:%Y", &this->timestamp);
}

void TouchCommand::execute()
{
  time_t set_time = mktime(&this->timestamp);
  if(set_time == ERROR)
  {
    throw SyscallException("mktime");
  }
  utimbuf times;
  times.actime = set_time;
  times.modtime = set_time;
  if(utime(this->file_name.c_str(), &times) == ERROR)
  {
    throw SyscallException("utime");
  }
}
TimeoutCommand::TimeoutCommand(const string cmd_line) : BuiltInCommand(cmd_line)
{
  if(this->argv.size() < 2 || !is_num(this->argv[1]))
  {
    throw InvalidlArguments(this->line);
  }
  this->duration = stoi(this->argv[1]);
  size_t found = this->line.find(this->argv[1]);
  this->cmd = this->line.substr(found + 1);
}

void TimeoutCommand::execute()
{
  if(this->duration == 0)
  {
    cout << "smash: got an alarm" << endl << "smash:" << this->line << " timed out!" << endl;
    return;
  }
  SmallShell& smash = SmallShell::getInstance();
  shared_ptr<Command> command = smash.createCommand(this->cmd);
  //smash.addTimedJob(command,this->duration);
  if (command->is_background)
  {
    smash.addJob(this->line, pid, false);
  }
  else
  {
    runInFg(pid, this->line, smash.getJobs()->getFGJobID());
  }
  
}
pid_t Command::childExecute()  
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
  return pid;
}