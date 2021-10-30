#include "../inc/base_header.h"


char* str_concat(char *str1, char *str2) {
    unsigned long len_buffer = strlen(str1) + strlen(str2);
    char *str = malloc(len_buffer + 1);

    strcat(str, str1);
    strcat(str, str2);

    return str;
}


char* str_array_concat(char *strings[], int strings_size) {
    unsigned long len_buffer = 0;

    for (int i = 0; i < strings_size; i++) {
        len_buffer += strlen(strings[i]);
    }

    char *str = malloc(len_buffer + 1);
    for (int i = 0; i < strings_size; i++) {
        strcat(str, strings[i]);
    }

    return str;
}


void await_setup(int pipe) {
    // We're done once we read something from the pipe.
    char buff[PIPE_MSG_SIZE];
    if (read(pipe, buff, PIPE_MSG_SIZE) != PIPE_MSG_SIZE)
        kill_process("Failed to read from pipe: %m\n");
}
