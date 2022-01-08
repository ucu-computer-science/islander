#ifndef LOGGER_SERVER_SERVER_H
#define LOGGER_SERVER_SERVER_H


#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/un.h>
#include <sys/socket.h>

#include "./server.h"
#include "./unix_sockets.h"
#include "./error_functions.h"

#define BACKLOG 5
#define BUF_SIZE 100
#define MAX_USERS 100
//#define SV_SOCK_PATH "~/islander/bin/logger_server"
#define SV_SOCK_PATH "/tmp/logger_server_socket1"

int run_server(int argc, char *argv[]);

void exit_gracefully(int *children_pids, int socket_desc, int exit_code, int n_pids);

#endif //LOGGER_SERVER_SERVER_H
