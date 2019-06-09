#ifndef PROCESS_H
#define PROCESS_H

#include <string>
using std::string;

#include "ProcessParser.h"

/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
  private:
    string pid_;
    string user_;
    string cmd_;
    string cpu_;
    string mem_;
    string up_time_;
  
  public:
    Process(string pid) {
      this->pid_ = pid;
      this->user_ = ProcessParser::GetProcUser(pid);
      this->mem_ = ProcessParser::GetVmSize(pid);
      this->cmd_ = ProcessParser::GetCmd(pid);
      this->up_time_ = ProcessParser::GetProcUpTime(pid);
      this->cpu_ = ProcessParser::GetCpuPercent(pid);
    }
    void SetPid(int pid);
    string GetPid() const;
    string GetUser() const;
    string GetCmd() const;
    int GetCpu() const;
    int GetMem() const;
    string GetUpTime() const;
    string GetProcess();
};

#endif //PROCESS_H