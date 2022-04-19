#ifndef SMALL_SHELL_H_
#define SMALL_SHELL_H_

#include <memory>
#include <iostream>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "Commands.h"
#include "exceptions.h"
#include "jobs.h"
using namespace std;

string _ltrim(const std::string& s);
string _rtrim(const std::string& s);
string _trim(const std::string& s);
class Command;
class JobsList;


class SmallShell {
 private:
  // TODO: Add your data members
  string current_prompt;
  string old_pwd;
  string current_pwd;
  pid_t pid;
  SmallShell();
  JobsList* jobs_list;
  pid_t fg_pid;
  int fg_job_id;
 public:
  shared_ptr<Command> createCommand(const string cmd_line);
  SmallShell(SmallShell const&)      = delete; // disable copy ctor
  void operator=(SmallShell const&)  = delete; // disable = operator
  static SmallShell& getInstance() // make SmallShell singleton
  {
    static SmallShell instance; // Guaranteed to be destroyed.
    // Instantiated on first use.
    return instance;
  }
  ~SmallShell();
  void executeCommand(const string cmd_line);
  // TODO: add extra methods as needed
  void setPrompt(const string new_prompt = "smash");
  string getPrompt();
  int getPid();
  void changeCurrentDirectory(const string new_pwd);
  bool isOldPwdSet();
  string getOldPwd();
  void addJob (shared_ptr<Command> command, pid_t pid, bool is_stopped); 
  void updateJobsList();
  void setForeground (shared_ptr<Command> command, pid_t pid);
  void sendSignal(const string signal);
  void bg ();
  void fg ();
  JobsList* getJobs();
};

#endif //SMALL_SHELL_H_
