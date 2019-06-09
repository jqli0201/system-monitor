#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <fstream>

// Classic helper functions
class Util {
  public:
    static std::string ConvertToTime (long int input_seconds);
    static std::string GetProgressBar (std::string percent);
    static std::ifstream GetStream(std::string path);
};

#endif // UTIL_H