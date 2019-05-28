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

//Reading /proc/[PID]/status for memory status of specific process
string ProcessParser::getVmSize(string pid)
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

string ProcessParser::getCpuPercent(string pid) {
  // acquiring relevant times for calculation of active occupation of CPU for selected process

  //Path::basePath() + pid + "/" + Path::statPath()

  //float utime = stof(ProcessParser::getProcUpTime(pid));

  //float stime = //14
  //float cutime = //15
  //float cstime = //16
  //float starttime = //21;

  //float uptime = ProcesspARSER::getSysUpTime();

  //float freq = sysconf(_SC_CLK_TCK);

  //float total_time = utime + stime + cutime + cstime;
  //float seconds = uptime - (starttime/freq);
  //result = 100.0*((total_time/freq)/seconds);

  //return to_string(result);
}