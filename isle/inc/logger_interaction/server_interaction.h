#ifndef ISLANDER_ENGINE_SERVER_INTERACTION_H
#define ISLANDER_ENGINE_SERVER_INTERACTION_H

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>

#include "../base_header.h"
#include "../helper_functions.h"
#include "../syscall_wrappers.h"
#include "../error_functions.h"

#define SV_SOCK_PATH "/tmp/logger_server_socket1"

int connect_to_process_logger(char* islename);

#endif //ISLANDER_ENGINE_SERVER_INTERACTION_H
