#ifndef LOGGER_SERVER_SERVER_H
#define LOGGER_SERVER_SERVER_H


#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>

#include "./server.h"
#include "./unix_sockets.h"
#include "./error_functions.h"

#define BACKLOG 5
#define BUF_SIZE 10
//#define SV_SOCK_PATH "~/islander/bin/logger_server"
#define SV_SOCK_PATH "/tmp/logger_server_socket1"

int run_server(int argc, char *argv[]);

#endif //LOGGER_SERVER_SERVER_H
