#ifndef CLIENT_SERVER_COMMUNICATION_FUNCTIONS_H
#define CLIENT_SERVER_COMMUNICATION_FUNCTIONS_H

#include <openssl/ssl.h>

#include "./base_functions.h"

void recv_cmd_output(int client_socket_desc, std::string &command_output_buf, bool need_encryption);

void run_command_remotely(int sockfd, const std::string &buf, std::string &command_output_buf, bool need_encryption);


#endif //CLIENT_SERVER_COMMUNICATION_FUNCTIONS_H
