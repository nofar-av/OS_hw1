#ifndef COMMANDS_H_
#define COMMANDS_H_

#include <vector>
#include <memory>
#include <string.h>
#include "small_shell.h"
#include "exceptions.h"
#include "jobs.h"

#define COMMAND_ARGS_MAX_LENGTH (200)
#define COMMAND_MAX_ARGS (20)

using namespace std;

class Command {
// TODO: Add your data members 
 protected:
  string line;
  vector<string> argv;
 public:
  Command(const string cmd_line);
  virtual ~Command() = default;
  virtual void execute() = 0;
  string getName();
  string getCommandLine();
  //virtual void prepare();
  //virtual void cleanup();
  // TODO: Add your extra methods if needed
};

class BuiltInCommand : public Command {
 
 public:
  BuiltInCommand(const string cmd_line);
  virtual ~BuiltInCommand() {}
};

class ExternalCommand : public Command {
  bool is_background;
  string line_no_background;
 public:
  ExternalCommand(const string cmd_line);
  virtual ~ExternalCommand() {}
  void execute() override;
};

class PipeCommand : public Command {
  // TODO: Add your data members
 public:
  PipeCommand(const string cmd_line);
  virtual ~PipeCommand() {}
  void execute() override;
};

class RedirectionCommand : public Command {
 // TODO: Add your data members
 public:
  explicit RedirectionCommand(const string cmd_line);
  virtual ~RedirectionCommand() {}
  void execute() override;
  //void prepare() override;
  //void cleanup() override;
};

class ChangePromptCommand : public BuiltInCommand {
// TODO: Add your data members public:
public:
  ChangePromptCommand(const string cmd_line);
  virtual ~ChangePromptCommand() {};
  void execute() override;
};

class ChangeDirCommand : public BuiltInCommand {
// TODO: Add your data members 
public:  
  ChangeDirCommand(const string cmd_line);
  virtual ~ChangeDirCommand() {}
  void execute() override;
};

class GetCurrDirCommand : public BuiltInCommand {
 public:
  GetCurrDirCommand(const string cmd_line);
  virtual ~GetCurrDirCommand() {}
  void execute() override;

};

class ShowPidCommand : public BuiltInCommand {
 public:
  ShowPidCommand(const string cmd_line);
  virtual ~ShowPidCommand() {}
  void execute() override;
};


class QuitCommand : public BuiltInCommand {
// TODO: Add your data members public:
  QuitCommand(const string cmd_line, JobsList* jobs);
  virtual ~QuitCommand() {}
  void execute() override;
};



class JobsCommand : public BuiltInCommand {
 // TODO: Add your data members
 shared_ptr<JobsList> jobs_list;
 public:
  JobsCommand(const string cmd_line, shared_ptr<JobsList> jobs);
  virtual ~JobsCommand() {}
  void execute() override;
};

class KillCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  KillCommand(const string cmd_line, JobsList* jobs);
  virtual ~KillCommand() {}
  void execute() override;
};

class ForegroundCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  ForegroundCommand(const string cmd_line, JobsList* jobs);
  virtual ~ForegroundCommand() {}
  void execute() override;
};

class BackgroundCommand : public BuiltInCommand {
 // TODO: Add your data members
 public:
  BackgroundCommand(const string cmd_line, JobsList* jobs);
  virtual ~BackgroundCommand() {}
  void execute() override;
};

class TailCommand : public BuiltInCommand {
 public:
  TailCommand(const string cmd_line);
  virtual ~TailCommand() {}
  void execute() override;
};

class TouchCommand : public BuiltInCommand {
 public:
  TouchCommand(const string cmd_line);
  virtual ~TouchCommand() {}
  void execute() override;
};

#endif //COMMANDS_H_
