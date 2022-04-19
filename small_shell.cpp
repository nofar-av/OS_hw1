
#include "small_shell.h"

#define PWD_NOT_SET "-1"

using namespace std;
#define NO_FG -1
#define ERROR -1
SmallShell::SmallShell() {
  this->pid = getpid();
  this->current_prompt = "smash";
  this->current_pwd = getcwd(nullptr,0); // memory leak?
  this->old_pwd = PWD_NOT_SET;
  this->fg_job_id = NO_FG;
  this->fg_pid = NO_FG;
  this->jobs_list = new JobsList();

// TODO: add your implementation
}

SmallShell::~SmallShell() {
// TODO: add your implementation
}

string SmallShell::getPrompt()
{
  return this->current_prompt;
}

void SmallShell::setPrompt(string new_prompt)
{
  this->current_prompt = new_prompt;
}
int SmallShell::getPid()
{
  return this->pid;
}
void SmallShell::changeCurrentDirectory(string new_pwd)
{
  this->old_pwd = this->current_pwd;
  if(chdir(new_pwd.c_str()) == -1)
  {
    throw SyscallException("chdir");
  }
  this->current_pwd = getcwd(nullptr,0); //memory leak?
}
bool SmallShell::isOldPwdSet()
{
  return this->old_pwd != PWD_NOT_SET;
}
string SmallShell::getOldPwd()
{
  return this->old_pwd;
}
/**
* Creates and returns a pointer to Command class which matches the given command line (cmd_line)
*/
shared_ptr<Command> SmallShell::createCommand(const string cmd_line) {
	// For example:
/*
  string cmd_s = _trim(string(cmd_line));
  string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));

  if (firstWord.compare("pwd") == 0) {
    return new GetCurrDirCommand(cmd_line);
  }
  else if (firstWord.compare("showpid") == 0) {
    return new ShowPidCommand(cmd_line);
  }
  else if ...
  .....
  else {
    return new ExternalCommand(cmd_line);
  }
  */
  string cmd_s = _trim(cmd_line);
  string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));
  if(firstWord.compare("chprompt") == 0)
    return shared_ptr<Command>(new ChangePromptCommand(cmd_line));
  else if(firstWord.compare("showpid") == 0)
    return shared_ptr<Command>(new ShowPidCommand(cmd_line));
  else if(firstWord.compare("pwd") == 0)
    return shared_ptr<Command>(new GetCurrDirCommand(cmd_line));
  else if(firstWord.compare("cd") == 0)
    return shared_ptr<Command>(new ChangeDirCommand(cmd_line));
  else if(firstWord.compare("jobs") == 0)
    return shared_ptr<Command>(new JobsCommand(cmd_line, SmallShell::getInstance().getJobs()));
  else 
    return shared_ptr<Command>(new ExternalCommand(cmd_line));
}

void SmallShell::executeCommand(const string cmd_line) {
  // TODO: Add your implementation here
  // for example:
  // Command* cmd = CreateCommand(cmd_line);
  // cmd->execute();
  // Please note that you must fork smash process for some commands (e.g., external commands....)


  //   jobs.updateJobList();
  try {
      shared_ptr<Command> cmd = SmallShell::createCommand(cmd_line);
      if (cmd == nullptr) {
          return;
      }
      cmd->execute();
  } catch (SyscallException &err) {
      perror(err.what()); // TODO: check prints to stderr
  } catch (SmashException &err) {
      cerr << err.what(); // TODO: check prints to stderr
  }
  cout<<"finished execute"<<endl;
}

void SmallShell::addJob (shared_ptr<Command> command, pid_t pid, bool is_stopped)
{
  this->jobs_list->addJob(command, pid, is_stopped);
}

void SmallShell::setForeground (shared_ptr<Command> command, pid_t pid)
{

  this->fg_pid = pid;
  this->fg_job_id = this->jobs_list->getFGJobID();
  int status;
  if (waitpid(pid, &status, WUNTRACED) == ERROR) {
      throw SyscallException("waitpid");
  }
  if (WIFSTOPPED(status)) {
      this->jobs_list->addJob(command, pid, true);
  }
  this->fg_pid = NO_FG;
  this->fg_job_id = NO_FG;
  this->jobs_list->removeFgJob();
}

JobsList* SmallShell::getJobs()
{
  return this->jobs_list;
}