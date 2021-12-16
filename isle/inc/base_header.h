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
#include <time.h>

#include "./helper_functions.h"
#include "./defined_vars.h"

struct process_params {
    int pipe_fd[PIPE_FD_NUM];
    char **argv;
};


static void parse_args(int argc, char **argv, char **new_argv, struct process_params *params) {
    // Skip binary path
//    argc--; argv++;
//    if (argc < 1) exit(0);
//    params->argv = argv;
    int start = 1, end = 2;
    arr_slice(argv, argc, new_argv, start, end);

    int null_ptr_idx = end - start;
    new_argv[null_ptr_idx] = NULL;
    params->argv = new_argv;
}

#endif //NAMESPACES_BASE_HEADER_H
