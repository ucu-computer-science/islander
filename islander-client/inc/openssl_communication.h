#ifndef CLIENT_OPENSSL_COMMUNICATION_H
#define CLIENT_OPENSSL_COMMUNICATION_H

#include <malloc.h>
#include <resolv.h>
#include <netdb.h>
#include <fcntl.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include "./base_functions.h"
#include "./defined_vars.h"

void run_encrypted_client(int port, std::string& message, std::string& command_output_buf);

int is_authorized_user(int server_fd);

int OpenConnection(const char *hostname, int port);

SSL_CTX* InitCTX(void);

void ShowCerts(SSL* ssl);

#endif //CLIENT_OPENSSL_COMMUNICATION_H
