#ifndef SMALL_SHELL_H_
#define SMALL_SHELL_H_

#include <memory>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "Commands.h"

using namespace std;

string _ltrim(const std::string& s);
string _rtrim(const std::string& s);
string _trim(const std::string& s);
class Command;

class SmallShell {
 private:
  // TODO: Add your data members
  string current_prompt;
  int pid;
  SmallShell();
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
  void setPrompt(string new_prompt = "smash");
  string getPrompt();
  int getPid();
};

#endif //SMALL_SHELL_H_
