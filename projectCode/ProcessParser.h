#ifndef PROCESSPARSER_H
#define PROCESSPARSER_H

#include <algorithm>
#include <iostream>
#include <math.h>
#include <thread>
#include <chrono>
#include <iterator>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <time.h>
#include <unistd.h>

#include "constants.h"


using namespace std;

class ProcessParser{
  private:
    ifstream stream; 
  public:
    static string GetCmd(string pid);
    static vector<string> GetPidList();
    static string GetVmSize(string pid);
    static string GetCpuPercent(string pid);
    static long int GetSysUpTime();
    static string GetProcUpTime(string pid);
    static string GetProcUser(string pid);
    static vector<string> GetSysCpuPercent(string coreNumber = "");
    static float GetSysActiveCpuTime(vector<string> values);
    static float GetSysIdleCpuTime(vector<string> values);
    static float GetSysRamPercent();
    static string GetSysKernelVersion();
    static int GetTotalThreads();
    static int GetTotalNumberOfProcesses();
    static int GetNumberOfCores();
    static int GetNumberOfRunningProcesses();
    static string GetOSName();
    static string PrintCpuStats(vector<string> values1, vector<string>values2);
    static bool IsPidExisting(string pid);
};

#endif // PROCESSPARSER_H