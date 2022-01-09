#ifndef ISLANDER_ENGINE_SYSCALL_WRAPPERS_H
#define ISLANDER_ENGINE_SYSCALL_WRAPPERS_H

#include "./base_header.h"

ssize_t read_in_buffer(int fd, char* buffer, const ssize_t size, int* status);

int write_from_buffer(int fd, const char* buffer, const ssize_t size, int* status);

#endif //ISLANDER_ENGINE_SYSCALL_WRAPPERS_H
