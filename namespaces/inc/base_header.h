//
// Created by yaroslav_morozevych on 16.10.21.
//

#ifndef NAMESPACES_BASE_HEADER_H
#define NAMESPACES_BASE_HEADER_H

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

#include "helper_functions.h"
#include "usernamespace.h"
#include "mntnamespace.h"

#define STACKSIZE (1024 * 1024)
#define PIPE_READ 0
#define PIPE_WRITE 1
#define PIPE_FD_NUM 2
#define ROOT_UID 0
#define ROOT_GID 0
#define UID 1000
#define PIPE_OK_MSG "OK"
#define PIPE_MSG_SIZE 2

struct process_params {
    int pipe_fd[PIPE_FD_NUM];
    char **argv;
};


static void parse_args(int argc, char **argv, struct process_params *params) {
    // Skip binary path
    argc--; argv++;
    if (argc < 1) exit(0);

    params->argv = argv;
}

#endif //NAMESPACES_BASE_HEADER_H
