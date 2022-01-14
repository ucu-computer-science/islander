// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <dirent.h>
#include "../include/utils.hpp"

#define RELATIVE_BIN_FOLDER_PATH "../external_commands/binary"
#define BUFFER_SIZE 512
#define DETACHED_MODE_CHAR "&"


bool is_number(const std::string &str) {
    for (char const &c : str) {
        if (std::isdigit(c) == 0) return false;
    }
    return true;
}


/* Logs the provided message and prepends it with time and client information  */
void log_action(sockaddr_in* client_info, const std::string& message) {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    // do not use encryption here as we use it just for debug
    std::cout << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << "        " <<
              inet_ntoa(client_info->sin_addr) << ":" << (int) ntohs(client_info->sin_port)
              << " -- " << message << std::endl;
}


/* Function to exit the main server process.
 * It kills all children processes that were created by the server and it also closes the parent socket fd */
void exit_gracefully(const std::vector<pid_t>& children_pids, int socket_desc, int exit_code) {
    cout << "\nServer shutting down...\n" << endl;
    for (auto child_pid : children_pids) kill(child_pid, SIGTERM);  // Kill all children :=(
    close(socket_desc);  // Close parent socket fd
    exit(exit_code);  // Goodbye!
}
