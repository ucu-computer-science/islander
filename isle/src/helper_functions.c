#include "../inc/helper_functions.h"


void parse_args(int argc, char** argv, struct process_params *params, resource_limits *res_limits) {
    if (argc < 2) exit(0);

    char** command_args = calloc(argc, sizeof(char*));

    // Split argv on limits for cgroup and arguments for command,
    // which will be executed via execvp()
    int arg_idx = 0;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--memory-in-bytes") == 0) {
            res_limits->memory_in_bytes = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "--cpu-quota") == 0) {
            res_limits->cpu_quota = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "--device-read-bps") == 0) {
            res_limits->device_read_bps = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "--device-write-bps") == 0) {
            res_limits->device_write_bps = argv[i + 1];
            i++;
        } else {
            command_args[arg_idx] = argv[i];
            arg_idx++;
        }
    }

//#ifdef DEBUG_MODE
    printf("res_limits->memory_in_bytes -- %s\n", res_limits->memory_in_bytes);
    printf("res_limits->cpu_quota -- %s\n", res_limits->cpu_quota);
    printf("res_limits->device_read_bps -- %s\n", res_limits->device_read_bps);
    printf("res_limits->device_write_bps -- %s\n", res_limits->device_write_bps);
//#endif

    // Add NULL at the end since exec syscall take such format of argv
    command_args[arg_idx++] = NULL;
    params->argc = arg_idx;
    params->argv = command_args;
}


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


void write_file(char path[100], char line[100]) {
    FILE *f = fopen(path, "w");

    if (f == NULL)
        kill_process("Failed to open file %s: %m\n", path);
    if (fwrite(line, 1, strlen(line), f) < 0)
        kill_process("Failed to write to file %s:\n", path);
    if (fclose(f) != 0)
        kill_process("Failed to close file %s: %m\n", path);
}
