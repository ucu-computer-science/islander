//
// Created by yaroslav_morozevych on 16.10.21.
//

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

void await_setup(int pipe) {
    // We're done once we read something from the pipe.
    char buff[PIPE_MSG_SIZE];
    if (read(pipe, buff, PIPE_MSG_SIZE) != PIPE_MSG_SIZE) kill_process("Failed to read from pipe: %m\n");
}

static void write_file(char path[100], char line[100]) {
    FILE *f = fopen(path, "w");

    if (f == NULL) kill_process("Failed to open file %s: %m\n", path);
    if (fwrite(line, 1, strlen(line), f) < 0) kill_process("Failed to write to file %s:\n", path);
    if (fclose(f) != 0) kill_process("Failed to close file %s: %m\n", path);
}

#endif //NAMESPACES_HELPER_FUNCTIONS_H
