// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "../inc/wrappers.h"

extern SSL* ssl;

void send_wrapper(bool need_encryption, int sockfd, std::string &msg) {
    if (need_encryption) {
        SSL_write(ssl, msg.c_str(), strlen(msg.c_str()));
    } else {
        send(sockfd, msg.c_str(), msg.size(), 0);
    }
}


ssize_t recv_wrapper(bool need_encryption, int sockfd, char *buf) {
    ssize_t n_bytes;
    if (need_encryption) {
        n_bytes = SSL_read(ssl, buf, SOCKET_BUF_SIZE);
        int decodedError = SSL_get_error(ssl, (int) n_bytes);
        ERR_print_errors_fp(stderr);
    } else {
        n_bytes = recv(sockfd, buf, SOCKET_BUF_SIZE, 0);
    }
    return n_bytes;
}


void close_sock_wrapper(bool need_encryption, int fd) {
    if (need_encryption) {
        SSL_free(ssl);         /* release connection state */
    }
    close(fd);
}
