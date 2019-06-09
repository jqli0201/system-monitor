#ifndef SYSINFO_H
#define SYSINFO_H

#include <string>
#include <vector>

#include "ProcessParser.h"

using std::string;
using std::vector;

class SysInfo {
  private:
    vector<string> last_cpu_stats;
    vector<string> current_cpu_stats;
    vector<string> cores_stats;
    vector<vector<string>> last_cpu_cores_stats;
    vector<vector<string>> current_cpu_cores_stats;
    string cpu_percent;
    float mem_percent;
    string os_name;
    string kernel_ver;
    long up_time;
    int total_proc;
    int running_proc;
    int threads;

  public:
    SysInfo();
    void SetAttributes();
    void SetLastCpuMeasures();
    string GetMemPercent() const;
    long GetUpTime() const;
    string GetThreads() const;
    string GetTotalProc() const;
    string GetRunningProc() const;
    string GetKernelVersion() const;
    string GetOsName() const;
    string GetCpuPercent() const;
    void GetOtherCores(int _size);
    void SetCpuCoresStats();
    vector<string> GetCoresStats() const;
};

#endif // SYSINFO_H