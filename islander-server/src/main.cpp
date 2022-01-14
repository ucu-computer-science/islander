// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "../include/defined_vars.hpp"
#include "../include/options_parser.hpp"
#include "../include/encrypted_server.h"


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
