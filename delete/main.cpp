//
// Created by yaroslav_morozevych on 09.01.22.
//
#include <iostream>
#include <fstream>
#include <vector>
#include <csignal>
#include <boost/algorithm/string.hpp>

#define ISLENODE_PATH "/home/yaroslav_morozevych/islander/islenodes/"
#define ISLENODE_FORMAT ".txt"


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


int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Too little arguments. Usage: islander delete <isle-name>\n";
        return 0;
    }
    // Get the PID of the isle.
    std::string isle_name = ISLENODE_PATH + std::string(argv[1]) + ISLENODE_FORMAT;
    int isle_pid = get_pid(isle_name);
    if (isle_pid == -1) {
        std::cout << "islander delete: Such isle does not exist.\n";
        return 0;
    }
    // kill the running isle.
    if (kill(isle_pid, SIGKILL) != 0) {
        std::cout << "islander delete: The isle with PID = " << isle_pid << " can't be deleted." << std::endl;
    } else {
        std::cout << "islander delete: The isle with PID = " << isle_pid << " is successfuly deleted." << std::endl;
    }
    return 0;
}
