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

#include "ProcessParser.h"
#include "constants.h"
#include "util.h"

string ProcessParser::GetCmd(string pid)
{
  string line;
  ifstream stream = Util::GetStream((Path::basePath() + pid + Path::cmdPath()));
  std::getline(stream, line);
  return line;
}

vector<string> ProcessParser::GetPidList()
{
  DIR* dir;
  // Basically, we are scanning / proc dir for all directories with numbers as their names
  // If we get valid check we store dir names in vector as list of machine pids
  vector<string> container;
  if(!(dir = opendir("/proc")))
    throw std::runtime_error(std::strerror(errno));

  while (dirent* dirp = readdir(dir)) {
    // is this a directory?
    if (dirp->d_type != DT_DIR)
      continue;
    // is every character of the name a digit?
    if (all_of(dirp->d_name, dirp->d_name + std::strlen(dirp->d_name), [](char c){ return std::isdigit(c); })) {
      container.push_back(dirp->d_name);
    }
  }

  // validating process of directory closing
  if(closedir(dir))
    throw std::runtime_error(std::strerror(errno));
  return container;
}

//Reading /proc/[PID]/status for memory status of specific process
string ProcessParser::GetVmSize(string pid)
{
  string line;
  //Declaring search attribute for file
  string name = "VmData";
  string value;
  float result;
  // Opening stream for specific file
  ifstream stream = Util::GetStream(Path::basePath() + pid + Path::statusPath());
  while(std::getline(stream, line)){
    // Searching line by line
    if (line.compare(0, name.size(), name) == 0) {
      // slicing string line on ws for values using sstream
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      //conversion KB -> GB
      result = (stof(values[1])/float(1024));
      break;
    }
  }
  return to_string(result);
}

string ProcessParser::GetCpuPercent(string pid) {
  string line;
  string value;
  float result;
  ifstream stream = Util::GetStream((Path::basePath() + pid + "/" + Path::statPath()));
  getline(stream, line);
  string str = line;
  istringstream buf(str);
  istream_iterator<string> beg(buf), end;
  vector<string> values(beg, end);
  // acquiring relevant times for calculation of active occupation of CPU for selected process
  float utime = stof(ProcessParser::GetProcUpTime(pid));
  float stime = stof(values[14]);
  float cutime = stof(values[15]);
  float cstime = stof(values[16]);
  float starttime = stof(values[21]);
  float uptime = ProcessParser::GetSysUpTime();
  // provides the number of ticks per second
  float freq = sysconf(_SC_CLK_TCK);
  float total_time = utime + stime + cutime + cstime;
  float seconds = uptime - (starttime/freq);
  result = 100.0*((total_time/freq)/seconds);
  return to_string(result);
}

// Read data from /proc. stoi() converts a string to an int.
long int ProcessParser::GetSysUpTime()
{
  string line;
  ifstream stream = Util::GetStream((Path::basePath() + Path::upTimePath()));
  getline(stream, line);
  istringstream buf(line);
  istream_iterator<string> beg(buf), end;
  vector<string> values(beg, end);
  return stoi(values[0]);
}

// Read data from the system files in /proc and retrieve the system frequency for calculating the process up time.
string ProcessParser::GetProcUpTime(string pid)
{
  string line;
  string value;
  float result;
  ifstream stream = Util::GetStream((Path::basePath() + pid + "/" + Path::statPath()));
  getline(stream, line);
  string str = line;
  istringstream buf(str);
  istream_iterator<string> beg(buf), end;
  vector<string> values(beg, end);
  // Using sysconf to get clock ticks of the host machine
  return to_string(float(stof(values[13])/sysconf(_SC_CLK_TCK)));
}

string ProcessParser::GetProcUser(string pid)
{
  string line;
  string name = "Uid:";
  string result = "";
  ifstream stream = Util::GetStream((Path::basePath() + pid + Path::statusPath()));
  // Getting UID for user
  while (std::getline(stream, line)) {
    if (line.compare(0, name.size(), name) == 0) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      result = values[1];
      break;
    }
  }
  stream = Util::GetStream("/etc/passwd");
  name = ("x:" + result);
  // Searching for name of the user with selected UID
  while (std::getline(stream, line)) {
    if (line.find(name) != std::string::npos) {
      result = line.substr(0, line.find(":"));
      return result;
    }
  }
  return "";
}

// This function reads raw data from the /proc/stat file. This file contains information on overall cpu usage, as well stats for individual cores.
vector<string> ProcessParser::GetSysCpuPercent(string coreNumber)
{
  // It is possible to use this method for selection of data for overall cpu or every core.
  // when nothing is passed "cpu" line is read
  // when, for example "0" is passed -> "cpu0" -> data for first core is read
  string line;
  string name = "cpu" + coreNumber;
  string value;
  int result;
  ifstream stream = Util::GetStream(Path::basePath() + Path::statPath());
  while (std::getline(stream, line)) {
    if (line.compare(0, name.size(), name) == 0) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      // set of cpu data active and idle times;
      return values;
    }
  }
  return (vector<string>());
}

float ProcessParser::GetSysActiveCpuTime(vector<string> values)
{
    return (stof(values[S_USER]) +
            stof(values[S_NICE]) +
            stof(values[S_SYSTEM]) +
            stof(values[S_IRQ]) +
            stof(values[S_SOFITRQ]) +
            stof(values[S_STEAL]) +
            stof(values[S_GUEST]) +
            stof(values[S_GUEST_NICE]));
} 

float ProcessParser::GetSysIdleCpuTime(vector<string> values)
{
  return (stof(values[S_IDLE]) + stof(values[S_IOWAIT]));
}

float ProcessParser::GetSysRamPercent()
{
  string line;
  string name1 = "MemAvailable:";
  string name2 = "MemFree.";
  string name3 = "Buffers:";

  string value;
  int result;
  ifstream stream = Util::GetStream((Path::basePath() + Path::memInfoPath()));
  float total_mem = 0;
  float free_mem = 0;
  float buffers = 0;
  while (std::getline(stream, line)) {
    if (total_mem != 0 && free_mem != 0)
      break;
    if (line.compare(0, name1.size(), name1) == 0) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      total_mem = stof(values[1]);
    }
    if (line.compare(0, name2.size(), name2) == 0) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      free_mem = stof(values[1]);
    }
    if (line.compare(0, name3.size(), name3) == 0) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      buffers = stof(values[1]);
    }
  }
  // calculating usage:
  return float(100.0 * (1 - (free_mem / (total_mem-buffers))));
}

string ProcessParser::GetSysKernelVersion() 
{
  string line;
  string name = "Linux version";
  ifstream stream = Util::GetStream(Path::basePath() + Path::versionPath());
  while (std::getline(stream, line)) {
    if (line.compare(0, name.size(), name) == 0) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      return values[2];
    }
  }
  return "";
}

int ProcessParser::GetTotalThreads()
{
  string line;
  int result = 0;
  string name = "Threads:";
  vector<string> _list = ProcessParser::GetPidList();
  for (int i = 0; i < _list.size(); i++) {
    string pid = _list[i];
    // getting every process and reading their number of their threads
    ifstream stream = Util::GetStream(Path::basePath() + pid + Path::statusPath());
    while (std::getline(stream, line)) {
      if (line.compare(0, name.size(), name) == 0) {
        istringstream buf(line);
        istream_iterator<string> beg(buf), end;
        vector<string> values(beg, end);
        result += stoi(values[1]);
        break;
      }
    }
  }
  return result;
}

int ProcessParser::GetNumberOfCores()
{
  // Get the number of host cpu cores
  string line;
  string name = "cpu cores";
  ifstream stream = Util::GetStream((Path::basePath() + "cpuinfo"));
  while (std::getline(stream, line)) {
    if (line.compare(0, name.size(), name) == 0) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      return stoi(values[3]);
    }
  }
  return 0;
}

int ProcessParser::GetTotalNumberOfProcesses()
{
  string line;
  int result = 0;
  string name = "processes";
  ifstream stream = Util::GetStream((Path::basePath() + Path::statPath()));
  while (std::getline(stream, line)) {
    if (line.compare(0, name.size(), name) == 0) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      result += stoi(values[1]);
      break;
    }
  }
  return result;
}

int ProcessParser::GetNumberOfRunningProcesses()
{
  string line;
  int result = 0;
  string name = "procs_running";
  ifstream stream = Util::GetStream((Path::basePath() + Path::statPath()));
  while (std::getline(stream, line)) {
    if (line.compare(0, name.size(), name) == 0) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      result += stoi(values[1]);
      break;
    }
  }
  return result;
}

string ProcessParser::GetOSName()
{
  string line;
  string name = "PRETTY_NAME=";

  ifstream stream = Util::GetStream(("/etc/os-release"));

  while (std::getline(stream, line)) {
    if (line.compare(0, name.size(), name) == 0) {
      std::size_t found = line.find("=");
      found++;
      string result = line.substr(found);
      result.erase(std::remove(result.begin(), result.end(), '"'), result.end());
      return result;
    }
  }
  return "";
}

string ProcessParser::PrintCpuStats(vector<string> values1, vector<string> values2)
{
/*
Because CPU stats can be calculated only if you take measures in two different time,
this function has two paramaters: two vectors of relevant values.
We use a formula to calculate overall activity of processor.
*/
float active_time = GetSysActiveCpuTime(values2) - GetSysActiveCpuTime(values1);
float idle_time = GetSysIdleCpuTime(values2) - GetSysIdleCpuTime(values1);
float total_time = active_time + idle_time;
float result = 100.0 * (active_time / total_time);
return to_string(result);
}