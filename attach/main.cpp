#include <iostream>
#include <fstream>
#include <vector>
#include <csignal>
#include <boost/algorithm/string.hpp>

#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <signal.h>
#include <ctype.h>

//#define ISLENODE_PATH "/home/yaroslav_morozevych/islander/islenodes/"
#define ISLANDER_HOME_PREFIX "islander/"
#define ISLENODE_DIR_PATH "islenodes/"
#define LOG_FDS_DIR_PATH "log_fds/"
#define ISLENODE_FORMAT ".txt"
#define MAX_PATH_LENGTH 256

using std::cout;
using std::cerr;
using std::endl;

typedef struct {
    int log_process_pid;
    int log_file_fd;
} log_process_info;

void get_islander_home(std::string &islander_home_path) {
    const char *exec_path;
    char user_home_path[MAX_PATH_LENGTH];


    // Set exec_path to current working dir to use exec_path for getting substring with user host path,
    // in case we run islander_engine binary with relative path to it
    char cwd[MAX_PATH_LENGTH];
    getcwd(cwd, MAX_PATH_LENGTH);
    exec_path = cwd;

    // get substring with user host path
    int count = 0;
    int substr_len = 0;
    for (int i = 0; i < strlen(exec_path); i++) {
        if (exec_path[i] == '/') {
            if (++count == 3) {
                substr_len = i + 1;
                break;
            }
        }
    }
    strncpy(user_home_path, exec_path, substr_len);
    user_home_path[substr_len] = '\0';

    std::string islander_home = ISLANDER_HOME_PREFIX;
    islander_home_path = user_home_path + islander_home;
}


void get_full_islenodes_path(std::string &full_islenodes_path) {
    std::string islander_home_path;
    get_islander_home(islander_home_path);
    full_islenodes_path = islander_home_path + ISLENODE_DIR_PATH;
}


void get_full_log_fds_path(std::string &full_log_fds_path) {
    std::string islander_home_path;
    get_islander_home(islander_home_path);
    full_log_fds_path = islander_home_path + LOG_FDS_DIR_PATH;
}


int get_pid(std::string &isle_name) {
    std::ifstream t(isle_name);
    std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    // Split the file by \n
    std::vector<std::string> params;
    boost::split(params, str,boost::is_any_of("\n"));
    // Get the PID.
    if (!params[0].empty()) {
        int pid = std::stoi(params[0]);
        return pid;
    }
    return -1;
}


void get_log_process_info(std::string &filepath, log_process_info &log_info) {
    // Read the whole file at once.
    std::ifstream t(filepath);
    std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    // Split the file by \n
    std::vector<std::string> params;
    boost::split(params, str,boost::is_any_of("\n"));
    // Save the params.
    cout << "params[0] -- " << params[0] << endl;
    cout << "params[1] -- " << params[1] << endl;
    log_info.log_process_pid = std::stoi(params[0]);
    log_info.log_file_fd  = std::stoi(params[1]);
}


void process_attach(int isle_log_process_pid, int log_file_fd, int tty_fd) {
    std::ofstream outfile;
    outfile.open("./gdb_process_attach", std::ios::out | std::ios::trunc );
    outfile << "p (int) dup2(" << tty_fd << ", " << log_file_fd << ")\nq" << endl;
    outfile.close();

    char cmd[256];
//    sprintf(cmd, "gdb -p %d -x gdb_process_attach > /dev/null 2>&1", isle_log_process_pid);
    sprintf(cmd, "gdb -p %d -x gdb_process_attach", isle_log_process_pid);
    cout << "cmd -- " << cmd << endl;
    system(cmd);
}


volatile sig_atomic_t stop;

void stop_process(int signum) {
    stop = 1;
}


void process_detach(int isle_log_process_pid, int log_file_fd, int tty_fd) {
    signal(SIGINT, stop_process);

    while (!stop);
    printf("exiting safely\n");

    std::ofstream outfile;
    outfile.open("./gdb_process_attach", std::ios::out | std::ios::trunc );
    outfile << "p (int) dup2(" << log_file_fd << ", " << tty_fd << ")\nq" << endl;
    outfile.close();

    char cmd[256];
//    sprintf(cmd, "gdb -p %d -x gdb_process_attach > /dev/null 2>&1", isle_log_process_pid);
    sprintf(cmd, "gdb -p %d -x gdb_process_attach", isle_log_process_pid);
    cout << "cmd -- " << cmd << endl;
    system(cmd);
//    system("pause");
}



std::string exec_bash_command(const std::string &cmd, int& out_exitStatus) {
    out_exitStatus = 0;
    auto pPipe = ::popen(cmd.c_str(), "r");
    if (pPipe == nullptr) {
        throw std::runtime_error("Cannot open pipe");
    }

    std::array<char, 256> buffer{};
    std::string result;

    while (not std::feof(pPipe))
    {
        auto bytes = std::fread(buffer.data(), 1, buffer.size(), pPipe);
        result.append(buffer.data(), bytes);
    }

    auto rc = ::pclose(pPipe);

    if (WIFEXITED(rc)) {
        out_exitStatus = WEXITSTATUS(rc);
    }

    return result;
}


int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Too little arguments. Usage: islander delete <isle-name>\n";
        return 0;
    }
    std::string full_islenodes_path;
    get_full_islenodes_path(full_islenodes_path);

    std::string cmd = "tty";
    int out_exitStatus;
    std::string tty_name = exec_bash_command(cmd, out_exitStatus);
    tty_name.pop_back();
    cout << "tty_name -- " << tty_name << endl;
    cout << "tty_name.size() -- " << tty_name.size() << endl;

    tty_name = "/dev/pts/6";
    int tty_fd = open(tty_name.c_str(), 1089, 0777);
    cout << "tty_fd -- " << tty_fd << endl;

//    // Get the PID of the isle.
//    std::string isle_name = full_islenodes_path + std::string(argv[1]) + ISLENODE_FORMAT;
//    int isle_pid = get_pid(isle_name);
//    if (isle_pid == -1) {
//        std::cout << "islander attach: Such isle does not exist.\n";
//        return 0;
//    }
//    cout << "isle pid: " << isle_pid << endl;

    // get log_process info
    std::string full_log_fds_path;
    get_full_log_fds_path(full_log_fds_path);
    std::string log_file_path = full_log_fds_path + std::string(argv[1]) + ISLENODE_FORMAT;
    cout << "log_file_path -- " << log_file_path << endl;

    log_process_info log_info;
    get_log_process_info(log_file_path, log_info);

    // attach to process with gdb
    process_attach(log_info.log_process_pid, log_info.log_file_fd, tty_fd);
//    process_detach(log_info.log_process_pid, log_info.log_file_fd, tty_fd);

//    close(tty_fd);
    return 0;
}
