#ifndef NAMESPACES_HELPER_FUNCTIONS_H
#define NAMESPACES_HELPER_FUNCTIONS_H


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <memory.h>

#include "./base_header.h"
#include "./manage_data/manage_mount.h"
#include "./manage_data/manage_volumes.h"
#include "./manage_data/manage_tmpfs.h"

#define PIPE_FD_NUM 2
#define PIPE_OK_MSG "OK"
#define PIPE_MSG_SIZE 2
#define COMMAND_SIZE 256

static void kill_process(const char *msg, ...) {
    va_list params;

    va_start(params, msg);
    vfprintf(stderr, msg, params);
    va_end(params);
    exit(1);
}

void parse_args(int argc, char** argv, struct process_params *params, resource_limits *res_limits);

void enable_features(int isle_pid, struct process_params *params, const char *exec_file_path);

void release_resources(int isle_pid, struct process_params *params);

void write_file(char path[100], char line[100]);

void await_setup(int pipe);

void get_cgroup_name(char *dest_str, int pid);

void str_array_concat(char *dest_str, char *strings[], int strings_size);

void create_dir(char* subsystem_path);

#endif //NAMESPACES_HELPER_FUNCTIONS_H
