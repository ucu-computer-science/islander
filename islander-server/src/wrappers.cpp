// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "../include/wrappers.h"

extern bool need_encryption;
extern SSL *ssl;

/* Writ=e from buffer into the file. Resilient to external signals */
int write_buffer(int fd, const char* buffer, ssize_t size, int &status) {
    ssize_t written_bytes = 0;
    while( written_bytes < size ) {
        ssize_t written_now = write(fd, buffer + written_bytes,
                                    size - written_bytes );
        if (written_now == -1) {
            if (errno == EINTR)
                continue;
            else {
                status = errno;
                return FUNC_FAILED;
            }
        } else {
            written_bytes += written_now;
        }
    }
    return SUCCESS;
}


/** Use appropriate function for error prints for simple or encrypted communication modes **/
void encrypted_cerr_if_needed(std::string msg) {
    SSL_write(ssl, msg.c_str(), strlen(msg.c_str()));
}


/** Use appropriate function for cout prints for simple or encrypted communication modes **/
void encrypted_cout_if_needed(std::string msg) {
    SSL_write(ssl, msg.c_str(), strlen(msg.c_str()));
}


/** Use appropriate function for write_buffer prints for simple or encrypted communication modes **/
void encrypted_write_buffer_if_needed(int fd, std::string &msg) {
    int status;
    if (need_encryption) {
        int err = SSL_write(ssl, msg.c_str(), strlen(msg.c_str()));
        int decodedError = SSL_get_error(ssl, err);
        ERR_print_errors_fp(stderr);
    }
    else
        write_buffer(fd, msg.c_str(), (ssize_t) msg.length(), status);
}


void exit_if_needed(int code) {
    if (!need_encryption)
        exit(code);
}


/** Use appropriate function to exit for simple or encrypted communication modes **/
void exit_wrapper(int code) {
    if (need_encryption) {
        std::string delimiter = COMMUNICATION_DELIMiTER;
        SSL_write(ssl, delimiter.c_str(), strlen(delimiter.c_str()));
    }
    else
        exit(code);
}
