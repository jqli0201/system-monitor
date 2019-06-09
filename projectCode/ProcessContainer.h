#ifndef PROCESSCONTAINER_H
#define PROCESSCONTAINER_H

#include <string>
#include <vector>

#include "Process.h"

using std::string;
using std::vector;

class ProcessContainer {
  public:
    ProcessContainer();
    void RefreshList();
    string PrintList();
    vector<string> GetList();

  private:
    vector<Process> list_;
};

#endif // PROCESSCONTAINER_H

