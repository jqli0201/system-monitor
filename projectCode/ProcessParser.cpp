#include <iostream>
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

vector<string> ProcessParser::get_pid_list()
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
string ProcessParser::get_vm_size(string pid)
{
  string line;
  //Declaring search attribute for file
  string name = "VmData";
  string value;
  float result;
  // Opening stream for specific file
  ifstream stream = Util::getStream(Path::basePath() + pid + Path::statusPath());
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

string ProcessParser::get_cpu_percent(string pid) {
  string line;
  string value;
  float result;
  ifstream stream = Util::getStream((Path::basePath() + pid + "/" + Path::statPath()));
  getline(stream, line);
  string str = line;
  istringstream buf(str);
  istream_iterator<string> beg(buf), end;
  vector<string> values(beg, end);
  // acquiring relevant times for calculation of active occupation of CPU for selected process
  float utime = stof(ProcessParser::get_proc_up_time(pid));
  float stime = stof(values[14]);
  float cutime = stof(values[15]);
  float cstime = stof(values[16]);
  float starttime = stof(values[21]);
  float uptime = ProcessParser::get_sync_up_time();
  // provides the number of ticks per second
  float freq = sysconf(_SC_CLK_TCK);
  float total_time = utime + stime + cutime + cstime;
  float seconds = uptime - (starttime/freq);
  result = 100.0*((total_time/freq)/seconds);
  return to_string(result);
}

// Read data from /proc. stoi() converts a string to an int.
long int ProcessParser::get_sys_up_time()
{
  string line;
  ifstream stream = Util::getStream((Path::basePath() + Path::upTimePath()));
  getline(stream, line);
  istringstream buf(line);
  istream_iterator<string> beg(buf), end;
  vector<string> values(beg, end);
  return stoi(values[0]);
}

// Read data from the system files in /proc and retrieve the system frequency for calculating the process up time.
string ProcessParser::get_proc_up_time(string pid)
{
  string line;
  string value;
  float result;
  ifstream stream = Util::getStream((Path::basePath() + pid + "/" + Path::statPath()));
  getline(stream, line);
  string str = line;
  istringstream buf(str);
  istream_iterator<string> beg(buf), end;
  vector<string> values(beg, end);
  // Using sysconf to get clock ticks of the host machine
  return to_string(float(stof(values[13])/sysconf(_SC_CLK_TCK)));
}

string ProcessParser::get_proc_user(string pid)
{
  string line:
  string name = "Uid:";
  string result = "";
  ifstream stream = Util::getStream((Path::basePath() + pid + Path::statusPath()));
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
  stream = Util::getStream("/etc/passwd");
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