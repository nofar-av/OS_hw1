
#include "small_shell.h"


using namespace std;

SmallShell::SmallShell() {
  this->pid = getpid();
  this->current_prompt = "smash";
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
  if(firstWord.compare("showpid") == 0)
    return shared_ptr<Command>(new ShowPidCommand(cmd_line));
  if(firstWord.compare("pwd") == 0)
    return shared_ptr<Command>(new GetCurrDirCommand(cmd_line));
  return nullptr;
}

void SmallShell::executeCommand(const string cmd_line) {
  // TODO: Add your implementation here
  // for example:
  // Command* cmd = CreateCommand(cmd_line);
  // cmd->execute();
  // Please note that you must fork smash process for some commands (e.g., external commands....)
  shared_ptr<Command> cmd = SmallShell::createCommand(cmd_line);
  if(cmd == nullptr)
	  return;
  cmd->execute();
}

