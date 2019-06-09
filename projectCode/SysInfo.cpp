#include <istream>
#include <string>
#include <vector>

#include "SysInfo.h"
#include "ProcessParser.h"
#include "util.h"

SysInfo::SysInfo() {
  /*
  Getting initial info about system
  Initial data for individual cores is set
  System data is set
   */
  this->GetOtherCores(ProcessParser::GetNumberOfCores());
  this->SetLastCpuMeasures();
  this->SetAttributes();
  this->os_name = ProcessParser::GetOSName();
  this->kernel_ver = ProcessParser::GetSysKernelVersion();
}

string SysInfo::GetCpuPercent() const
{
  return this->cpu_percent;
}

string SysInfo::GetMemPercent() const
{
  return to_string(this->mem_percent);
}

long SysInfo::GetUpTime() const
{
  return this->up_time;
}

string SysInfo::GetKernelVersion() const
{
  return this->kernel_ver;
}

string SysInfo::GetTotalProc() const
{
  return to_string(this->total_proc);
}

string SysInfo::GetRunningProc() const
{
  return to_string(this->running_proc);
}

string SysInfo::GetThreads() const
{
  return to_string(this->threads);
}

string SysInfo::GetOsName() const
{
  return this->os_name;
}

void SysInfo::SetLastCpuMeasures()
{
  this->last_cpu_stats = ProcessParser::GetSysCpuPercent();
}

void SysInfo::GetOtherCores(int _size)
{
  //when number of cores is detected, vectors are modified to fit incoming data
  this->cores_stats = vector<string>();
  this->cores_stats.resize(_size);
  this->last_cpu_cores_stats = vector<vector<string>>();
  this->last_cpu_cores_stats.resize(_size);
  this->current_cpu_cores_stats = vector<vector<string>>();
  this->current_cpu_cores_stats.resize(_size);
  for (int i = 0; i < _size; i++) {
    this->last_cpu_cores_stats[i] = ProcessParser::GetSysCpuPercent(to_string(i));
  }
}

void SysInfo::SetCpuCoresStats()
{
  // Getting data from files (previous data is required)
  for (int i = 0; i < this->current_cpu_cores_stats.size(); i++) {
    this->current_cpu_cores_stats[i] = ProcessParser::GetSysCpuPercent(to_string(i));
  }
  for (int i = 0; i < this->current_cpu_cores_stats.size(); i++) {
    //after acquirement of data we are calculating every core percentage of usage
    this->cores_stats[i] = ProcessParser::PrintCpuStats(this->last_cpu_cores_stats[i], this->current_cpu_cores_stats[i]);
  }
  this->last_cpu_cores_stats = this->current_cpu_cores_stats;
}

void SysInfo::SetAttributes() {
  // getting parsed data
  this->mem_percent = ProcessParser::GetSysRamPercent();
  this->up_time = ProcessParser::GetSysUpTime();
  this->total_proc = ProcessParser::GetTotalNumberOfProcesses();
  this->running_proc = ProcessParser::GetNumberOfRunningProcesses();
  this->threads = ProcessParser::GetTotalThreads();
  this->current_cpu_stats = ProcessParser::GetSysCpuPercent();
  this->cpu_percent = ProcessParser::PrintCpuStats(this->last_cpu_stats,
                                                   this->current_cpu_stats);
  this->last_cpu_stats = this->current_cpu_stats;
  this->SetCpuCoresStats();
}

// Constructing string for every core data display
vector<string> SysInfo::GetCoresStats() const {
  vector<string> result = vector<string>();
  for (int i = 0; i < this->cores_stats.size(); i++) {
    string temp = ("cpu" + to_string(i) + ":");
    float check = stof(this->cores_stats[i]);
    if (!check || this->cores_stats[i] == "nan") {
      return vector<string>();
    }
    temp += Util::GetProgressBar(this->cores_stats[i]);
    result.push_back(temp);
  }
  return result;
}