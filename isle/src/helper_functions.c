#include "../inc/base_header.h"


void await_setup(int pipe) {
    // We're done once we read something from the pipe.
    char buff[PIPE_MSG_SIZE];
    if (read(pipe, buff, PIPE_MSG_SIZE) != PIPE_MSG_SIZE)
        kill_process("Failed to read from pipe: %m\n");
}


void get_cgroup_name(char *dest_str, int pid) {
    char group_prefix[] = "group_";
    char str_num[16];

    // convert int to string
    sprintf(str_num, "%d", pid);
    char *group_name[] = {group_prefix, str_num};

    str_array_concat(dest_str, group_name, 2);
#ifdef DEBUG_MODE
    printf("get_cgroup_name -- %s\n", dest_str);
#endif
}


void str_array_concat(char *dest_str, char *strings[], int strings_size) {
    for (int i = 0; i < strings_size; i++) {
        strcat(dest_str, strings[i]);
    }
}


void create_dir(char* subsystem_path) {
    // 0700 meaning -- http://www.filepermissions.com/file-permission/0700
    // permission codes -- https://man7.org/linux/man-pages/man7/inode.7.html
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


void arr_slice(char** arr, size_t size, char** new_arr, size_t start, size_t end) {
    if (end >= size)
        perror("Index out of bounds");

    size_t j = 0;
    for (size_t i = start; i < end; i++, j++) {
        new_arr[j] = arr[i];
    }
}


void write_file(char path[100], char line[100]) {
    FILE *f = fopen(path, "w");

    if (f == NULL)
        kill_process("Failed to open file %s: %m\n", path);
    if (fwrite(line, 1, strlen(line), f) < 0)
        kill_process("Failed to write to file %s:\n", path);
    if (fclose(f) != 0)
        kill_process("Failed to close file %s: %m\n", path);
}


/* Create file that contains information about the isle itself
 * like PID, Name, Time created. */
void create_islenode(char* isle_name, int isle_pid) {
    // Provide a path for the file that needs to be created
    char file_name[strlen("../isle/isles/") + strlen(isle_name) + strlen(".txt")];
    sprintf(file_name, "../isle/isles/%s.txt", isle_name);
    // Create file.
    FILE* file = fopen(file_name, "w");
    // Get the current timestamp.
    time_t t;
    time(&t);
    char* time = ctime(&t);
    // Write isle parameters to the associated file separeted with \n
    fprintf(file, "%d\n%s\n%s", isle_pid, isle_name, time);
    fclose(file);
}
