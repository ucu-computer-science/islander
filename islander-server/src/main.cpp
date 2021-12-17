// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <sys/stat.h> // Structures (types) for fstat(), lstat (), stat ().
#include <sys/wait.h>
#include <fcntl.h> // Types and constants for fcntl() and open().
#include <unistd.h> // POSIX header

#include <fstream>
#include <string>

#include "../include/defined_vars.hpp"
#include "../include/options_parser.hpp"
#include "../include/base_header.hpp"
#include "../include/utils.hpp"
#include "../include/execution_functs.hpp"
#include "../include/server.h"
#include "../include/encrypted_server.h"

;
bool need_encryption;

std::string get_working_path() {
    std::string cur_path, working_dir_path;
    const int BUF_SIZE = 512;
    std::vector<char> buffer(BUF_SIZE);

    cur_path = getcwd(buffer.data(), BUF_SIZE);
    working_dir_path = "\033[1;36m" + cur_path + "\033[0m" + " $ "; // add bold and color style

    return working_dir_path;
}

int main(int argc, char *argv[]) {
    // parse arguments
    command_line_options config;
    config.parse(argc, argv);

    int port = config.get_port();

    if (port == -1) {
        cout << "No port specified. Picking the default one" << endl;
        port = DEFAULT_PORT;  // Setting a default port
    }

    cout << "Launching server on 127.0.0.1:" << port << endl;

    // Launch server
    run_encrypted_server(port);

    return SUCCESS;
}
