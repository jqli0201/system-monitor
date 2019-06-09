#include "ProcessContainer.h"

#include <string>
#include <vector>

#include "ProcessParser.h"

using std::string;
using std::vector;

ProcessContainer::ProcessContainer() {
  this->RefreshList();
}

void ProcessContainer::RefreshList() {
  vector<string> pids = ProcessParser::GetPidList();
  this->list_.clear();
  for (auto pid : pids) {
    Process proc(pid);
    this->list_.push_back(proc);
  }
}

string ProcessContainer::PrintList() {
  string result = "";
  for (auto i : list_) {
    result += i.GetProcess();
  }
  return result;
}

vector<string> ProcessContainer::GetList() {
  vector<string> values;
  for (int i = (this->list_.size() - 10); i < this->list_.size(); i++) {
    values.push_back(this->list_[i].GetProcess());
  }
  return values;
}