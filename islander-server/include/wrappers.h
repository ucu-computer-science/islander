#ifndef MYSHELL_WRAPPERS_H
#define MYSHELL_WRAPPERS_H

#include "../include/base_header.hpp"
#include "../include/defined_vars.hpp"

#include "openssl/ssl.h"
#include "openssl/err.h"

int write_buffer(int fd, const char* buffer, ssize_t size, int &status);

void encrypted_cerr_if_needed(std::string msg);

void encrypted_cout_if_needed(std::string msg);

void encrypted_write_buffer_if_needed(int fd, std::string &msg);

void exit_if_needed(int code);

void exit_wrapper(int code);

#endif //MYSHELL_WRAPPERS_H
