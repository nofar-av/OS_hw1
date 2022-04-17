#include <unistd.h>
#include <iostream>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include "Commands.h"
#include <time.h>
#include <utime.h>



using namespace std;

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

int _parseCommandLine(const char* cmd_line, char** args) {
  FUNC_ENTRY()
  int i = 0;
  istringstream iss(_trim(string(cmd_line)).c_str());
  for(string s; iss >> s; ) {
    args[i] = (char*)malloc(s.length()+1);
    memset(args[i], 0, s.length()+1);
    strcpy(args[i], s.c_str());
    args[++i] = NULL;
  }
  return i;

  FUNC_EXIT()
}

bool _isBackgroundComamnd(const char* cmd_line) {
  const string str(cmd_line);
  return str[str.find_last_not_of(WHITESPACE)] == '&';
}

void _removeBackgroundSign(char* cmd_line) {
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


BuiltInCommand::BuiltInCommand(const char* cmd_line) : Command(cmd_line), line(cmd_line)
{
  string word = "";
  int j = 0;
  for (int i = 0; i < line.size(); i++)
  {
    if (line[i] == ' ' && word.size() == 0)
      continue;
    if (line[i] == ' ' && word.size() > 0)
    {
      argv.push_back(word);
      word = "";
      j++;
    }
    if (line[i] != ' ')
	    word += line[i];
  }
  if(line[line.size()-1] != ' ')
    argv.push_back(word);
}
ChangePromptCommand::ChangePromptCommand (const char* cmd_line) : BuiltInCommand(cmd_line){}

void ChangePromptCommand::execute()
{
  string prompt = "smash";
  if(argv.size() > 1)
    prompt = argv[1];
  SmallShell::getInstance().SetPrompt(prompt);
  return;
}

ShowPidCommand::ShowPidCommand(const char* cmd_line) : BuiltInCommand(cmd_line){}

void ShowPidCommand::execute()
{
  cout << "smash pid is " + SmallShell::getInstance().getPid() << endl;
  return;
}

GetCurrDirCommand::GetCurrDirCommand(const char* cmd_line) : BuiltInCommand(cmd_line){}

void GetCurrDirCommand::execute()
{
  char* dir = getcwd(nullptr,0); 		
  cout << dir << endl;
  free(dir);
}