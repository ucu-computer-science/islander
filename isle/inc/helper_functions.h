#ifndef NAMESPACES_HELPER_FUNCTIONS_H
#define NAMESPACES_HELPER_FUNCTIONS_H


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <memory.h>

#define PIPE_FD_NUM 2
#define PIPE_OK_MSG "OK"
#define PIPE_MSG_SIZE 2

static void kill_process(const char *msg, ...) {
    va_list params;

    va_start(params, msg);
    vfprintf(stderr, msg, params);
    va_end(params);
    exit(1);
}

void write_file(char path[100], char line[100]);

void await_setup(int pipe);

void get_cgroup_name(char *dest_str, int pid);

void str_array_concat(char *dest_str, char *strings[], int strings_size);

void create_dir(char* subsystem_path);

void arr_slice(char** arr, size_t size, char** new_arr, size_t start, size_t end);

void create_islenode(char* isle_name, int isle_pid);

#endif //NAMESPACES_HELPER_FUNCTIONS_H
