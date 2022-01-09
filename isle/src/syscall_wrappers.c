// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "../inc/syscall_wrappers.h"


/* Read from the file into buffer. Resilient to external signals */
ssize_t read_in_buffer(int fd, char* buffer, const ssize_t size, int* status) {
    ssize_t read_bytes = 0;
    while( read_bytes < size ) {
        ssize_t read_now = read(fd, buffer + read_bytes,
                                size - read_bytes );
        if (read_now == -1) {
            if (errno == EINTR)
                continue;
            else {
                *status = errno;
                return -1;
            }
        } else {
            read_bytes += read_now;
            if (read_now == 0)
                return read_bytes;
        }

    }
    return read_bytes;
}


/* Write from buffer into the file. Resilient to external signals */
int write_from_buffer(int fd, const char* buffer, const ssize_t size, int* status) {
    ssize_t written_bytes = 0;
    while( written_bytes < size ) {
        ssize_t written_now = write(fd, buffer + written_bytes,
                                    size - written_bytes );
        if (written_now == -1) {
            if (errno == EINTR)
                continue;
            else {
                *status = errno;
                return EXIT_FAILURE;
            }
        } else {
            written_bytes += written_now;
        }
    }
    return SUCCESS;
}
