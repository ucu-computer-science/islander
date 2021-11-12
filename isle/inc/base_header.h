#ifndef NAMESPACES_BASE_HEADER_H
#define NAMESPACES_BASE_HEADER_H

// do not delete this 'define' as it is used for clone() syscall
#define _GNU_SOURCE

#include <stdio.h>
#include <sched.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/prctl.h>
#include <sys/stat.h>
#include <wait.h>
#include <memory.h>
#include <syscall.h>
#include <errno.h>

#include "./defined_vars.h"


struct process_params
{
    int pipe_fd[PIPE_FD_NUM];
    char **argv;
    int argc;
};

typedef struct
{
    char* memory_in_bytes;
    char* cpu_shares;
    char* cpu_period;
    char* cpu_quota;
    char* device_read_bps;
    char* device_write_bps;
} resource_limits;


#endif //NAMESPACES_BASE_HEADER_H
