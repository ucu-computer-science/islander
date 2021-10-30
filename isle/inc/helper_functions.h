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


void await_setup(int pipe);


static void write_file(char path[100], char line[100]) {
    FILE *f = fopen(path, "w");

    if (f == NULL)
        kill_process("Failed to open file %s: %m\n", path);
    if (fwrite(line, 1, strlen(line), f) < 0)
        kill_process("Failed to write to file %s:\n", path);
    if (fclose(f) != 0)
        kill_process("Failed to close file %s: %m\n", path);
}

char* get_cgroup_name(int pid);

char* str_concat(char *str1, char *str2);

char* str_array_concat(char *strings[], int strings_siz);

void create_dir(char* subsystem_path);

#endif //NAMESPACES_HELPER_FUNCTIONS_H
