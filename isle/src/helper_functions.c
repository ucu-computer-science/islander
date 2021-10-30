#include "../inc/base_header.h"


void await_setup(int pipe) {
    // We're done once we read something from the pipe.
    char buff[PIPE_MSG_SIZE];
    if (read(pipe, buff, PIPE_MSG_SIZE) != PIPE_MSG_SIZE)
        kill_process("Failed to read from pipe: %m\n");
}


char* get_cgroup_name(int pid) {
    char group_prefix[] = "group_";
    char str_num[16];

    // convert int to string
    sprintf(str_num, "%d", pid);
    char *group_name = str_concat(group_prefix, str_num);

    printf("group_name -- %s\n", group_name);
    return group_name;
}


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


void create_dir(char* subsystem_path) {
    // 0700 meaning -- http://www.filepermissions.com/file-permission/0700
    // permission codes -- https://man7.org/linux/man-pages/man7/inode.7.html
//    mode_t target_mode = 0777;
    mode_t target_mode = 0700;
    if (mkdir(subsystem_path, target_mode) == 0) {
        printf("Created a new directory -- %s\n", subsystem_path);
    } else if (errno == 17) {
        printf("Directory already exists -- %s\n", subsystem_path);
    } else {
        printf("errno -- %d\n", errno);
        printf("Unable to create directory-- %s. Reason -- %s\n", subsystem_path, strerror(errno));
        exit(1);
    }
}
