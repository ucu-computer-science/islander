#ifndef CLIENT_SERVER_COMMUNICATION_FUNCTIONS_H
#define CLIENT_SERVER_COMMUNICATION_FUNCTIONS_H

#include <openssl/ssl.h>

#include "./base_functions.h"

std::string recv_cmd_output(int client_socket_desc, bool need_encryption, bool one_time);

void run_command_remotely(int sockfd, const std::string &buf, bool need_encryption);


#endif //CLIENT_SERVER_COMMUNICATION_FUNCTIONS_H
