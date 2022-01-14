#ifndef CLIENT_WRAPPERS_H
#define CLIENT_WRAPPERS_H

#include <malloc.h>
#include <resolv.h>
#include <netdb.h>
#include <fcntl.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include "./base_functions.h"

void send_wrapper(bool need_encryption, int sockfd, std::string &msg);

ssize_t recv_wrapper(bool need_encryption, int sockfd, char *buf);

#endif //CLIENT_WRAPPERS_H
