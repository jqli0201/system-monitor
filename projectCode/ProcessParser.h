#include <string>

class ProcessParser{

  public: 
    static std::string getCmd(std::string pid);
    static std::vector<std::string> get_pid_list();
    static std::string get_vm_size(std::string pid);
    static std::string get_cpu_percent(std::string pid);
    static long int get_sync_up_time();
    static std::string get_proc_up_time(std::string pid);
    static std::string get_proc_user(std::string pid);
    static std::vector<std::string> getSysCpuPercent(std::string coreNumber = "");
    static float getSysRamPercent();
    static std::string getSysKernelVersion();
    static int getTotalThreads();
    static int getTotalNumberOfProcesses();
    static int getNumberOfRunningProcesses();
    static std::string getOSName();
    static std::string PrintCpuStats(std::vector<std::string> values1, std::vector<std::string>values2);
};

