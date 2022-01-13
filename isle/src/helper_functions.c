// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "../inc/helper_functions.h"
#include "../inc/cgroup_functions.h"

#define MNT_FREQUENCY 5.0

void parse_args(int argc, char** argv, struct process_params *params, resource_limits *res_limits) {
    if (argc < 2) exit(0);

    char** command_args = calloc(argc, sizeof(char*));
    params->mnt_src = calloc((int)(argc / MNT_FREQUENCY), sizeof(char*));
    params->mnt_dst = calloc((int)(argc / MNT_FREQUENCY), sizeof(char*));
    params->vlm_src = calloc((int)(argc / MNT_FREQUENCY), sizeof(char*));
    params->vlm_dst = calloc((int)(argc / MNT_FREQUENCY), sizeof(char*));
    params->has_netns = false;

    // Split argv on limits for cgroup and arguments for command,
    // which will be executed via execvp()
    int arg_idx = 0;
    int mnt_src_idx = 0;
    int mnt_dst_idx = 0;
    int vlm_src_idx = 0;
    int vlm_dst_idx = 0;
    for (int i = 1; i < argc; i++) {
        // cgroup limits
        if (strcmp(argv[i], "--memory-in-bytes") == 0) {
            res_limits->memory_in_bytes = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "--cpu-shares") == 0) {
            res_limits->cpu_shares = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "--cpu-period") == 0) {
            res_limits->cpu_period = argv[i + 1];
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

        // remote volumes feature
        } else if (strcmp(argv[i], "--mount-aws") == 0) {
            params->remote_vlm.is_mount_aws = true;
            params->remote_vlm.mnt_aws_src = argv[i + 2];
            params->remote_vlm.mnt_aws_dst = argv[i + 4];
            i += 4;

        // detached mode feature
        } else if (strcmp(argv[i], "--detach") == 0 || strcmp(argv[i], "-d") == 0) {
            params->is_detached = true;

        // mount feature
        } else if (strcmp(argv[i], "--mount") == 0) {
            params->is_mount = true;
            params->mnt_src[mnt_src_idx++] = argv[i + 2];
            params->mnt_dst[mnt_dst_idx++] = argv[i + 4];
            i += 4;

        // volume feature
        } else if (strcmp(argv[i], "--volume") == 0) {
            params->is_volume = true;
            params->vlm_src[vlm_src_idx++] = argv[i + 2];
            params->vlm_dst[vlm_dst_idx++] = argv[i + 4];
            i += 4;

        // tmpfs feature
        } else if (strcmp(argv[i], "--tmpfs") == 0) {
            params->is_tmpfs = true;
            params->tmpfs_dst = argv[i + 2];
            params->tmpfs_size = argv[i + 4];
            params->tmpfs_nr_inodes = argv[i + 6];
            i += 6;
          
        // islenode feature
        } else if (strcmp(argv[i], "--name") == 0) {
            params->name = argv[i + 1];
            i++;

        // check if container has net namespace
        } else if (strcmp(argv[i], "--netns") == 0) {
            if (strcmp(argv[i + 1], "True") == 0)
                params->has_netns = true;
            else
                params->has_netns = false;
            i++;

        } else {
            command_args[arg_idx] = argv[i];
            arg_idx++;
        }
    }

    params->mnt_num = mnt_src_idx;
    params->vlm_num = vlm_src_idx;

#ifdef DEBUG_MODE
    printf("res_limits->memory_in_bytes -- %s\n", res_limits->memory_in_bytes);
    printf("res_limits->cpu_shares -- %s\n", res_limits->cpu_shares);
    printf("res_limits->cpu_period -- %s\n", res_limits->cpu_period);
    printf("res_limits->cpu_quota -- %s\n", res_limits->cpu_quota);
    printf("res_limits->device_read_bps -- %s\n", res_limits->device_read_bps);
    printf("res_limits->device_write_bps -- %s\n", res_limits->device_write_bps);
#endif

    // Add NULL at the end since exec syscall take such format of argv
    command_args[arg_idx++] = NULL;
    params->argc = arg_idx;
    params->argv = command_args;
}


void set_up_default_params(struct process_params* params) {
    params->is_detached = false;
    params->is_mount = false;
    params->is_volume = false;
    params->is_tmpfs = false;
}


void enable_features(int isle_pid, struct process_params *params, const char *exec_file_path) {
    if (params->is_mount) mount_feature(isle_pid, params);
    if (params->is_volume) volume_feature(isle_pid, params, exec_file_path);
    if (params->is_tmpfs) mount_ns_tmpfs(isle_pid, params);
    if (params->remote_vlm.is_mount_aws) mount_s3_bucket(isle_pid, params->remote_vlm.mnt_aws_src,
                                                         params->remote_vlm.mnt_aws_dst, exec_file_path);
}


void release_resources(int isle_pid, struct process_params *params) {
    if (params->is_mount) unmount_dirs(isle_pid, params);
    if (params->is_volume) unmount_volumes(isle_pid, params);
    if (params->is_tmpfs) unmount_ns_dir(isle_pid, params->tmpfs_dst);
    if (params->remote_vlm.is_mount_aws) umount_cloud_dir(isle_pid, params->remote_vlm.mnt_aws_dst);

//    rm_cgroup_dirs(isle_pid);

    free(params->argv);
    free(params->mnt_src);
    free(params->mnt_dst);
    free(params->vlm_src);
    free(params->vlm_dst);
}


// Function source -- https://www.techiedelight.com/implement-substr-function-c/
// Following function extracts characters present in `src`
// between `m` and `n` (excluding `n`)
char* substr(const char *src, int m, int n) {
    // get the length of the destination string
    int len = n - m;

    // allocate (len + 1) chars for destination (+1 for extra null character)
    char *dest = (char*)malloc(sizeof(char) * (len + 1));

    // start with m'th char and copy `len` chars into the destination
    strncpy(dest, (src + m), len);

    // return the destination string
    return dest;
}


char* get_username() {
    const char *exec_path;
    char cwd[256];
    getcwd(cwd, 256);
    exec_path = cwd;

    // get substring with user host path
    uint count = 0;
    uint substr_start = 0;
    uint substr_end = strlen(exec_path);
    for (uint i = 0; i < strlen(exec_path); i++) {
        if (exec_path[i] == '/') {
            count++;
            if (count == 2) {
                substr_start = i + 1;
            } else if (count == 3) {
                substr_end = i;
                break;
            }
        }
    }
    return substr(exec_path, substr_start, substr_end);
}


void get_islander_home(char *islander_home_path, const char *exec_file_path) {
    const char *exec_path;
    char user_home_path[256];

    // here we find use home path as where islander is located by default.
    // For example, /home/username/
    if (exec_file_path[0] == '/') {
        // set exec_path to exec_file_path to use exec_path for getting substring with user host path,
        // in case we run islander_engine binary with full path to it, for ex., from /var/lib directory
        exec_path = exec_file_path;
    }
    else {
        // set exec_path to current working dir to use exec_path for getting substring with user host path,
        // in case we run islander_engine binary with relative path to it
        char cwd[256];
        getcwd(cwd, 256);
        exec_path = cwd;
    }

    // get substring with user host path
    uint count = 0;
    uint substr_len = 0;
    for (uint i = 0; i < strlen(exec_path); i++) {
        if (exec_path[i] == '/') {
            if (++count == 3) {
                substr_len = i + 1;
                break;
            }
        }
    }
    strncpy(user_home_path, exec_path, substr_len);
    user_home_path[substr_len] = '\0';

    // make concatenations
    char *str_arr[] = {user_home_path, ISLANDER_HOME_PREFIX};
    char islander_home_path2[256];
    islander_home_path2[0] = '\0';
    str_array_concat(islander_home_path2, str_arr, 2);
    strcpy(islander_home_path, islander_home_path2);
}


void get_aws_secrets_path(char *aws_secrets_path, const char *exec_file_path) {
    char islander_home_path[256];
    get_islander_home(islander_home_path, exec_file_path);

    char *secrets_prefix = SECRETS_PREFIX;
    char *aws_secrets_name = AWS_SECRETS_NAME;
    sprintf(aws_secrets_path, "%s%s%s", islander_home_path, secrets_prefix, aws_secrets_name);
}


void exec_bash_cmd(char *cmd) {
    FILE *p;
    int ch;

    p = popen(cmd,"r");
    if (p == NULL) {
        puts("Unable to open process");
        return;
    }
    while( (ch=fgetc(p)) != EOF)
        putchar(ch);
    pclose(p);
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
#ifdef DEBUG_MODE
        printf("Created a new directory -- %s\n", subsystem_path);
#endif
    } else if (errno == 17) {
#ifdef DEBUG_MODE
        printf("Directory already exists -- %s\n", subsystem_path);
#endif
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


/** Create file that contains information about the isle itself
 * like PID, Name, Time created. */
void create_islenode(char* isle_name, int isle_pid) {
    // Provide a path for the file that needs to be created
    char file_name[strlen(ISLENODE_DIR_PATH) + strlen(isle_name) + strlen(ISLENODE_FORMAT) + 1];
//    file_name[0] = '\0';
    sprintf(file_name, "%s%s%s", ISLENODE_DIR_PATH, isle_name, ISLENODE_FORMAT);
    printf("file_name -- %s", file_name);

    // Create file.
    FILE* file = fopen(file_name, "w");
    // Get the current timestamp.
    time_t t;
    time(&t);
    char* time = ctime(&t);
    printf("file_name2 -- %s", file_name);
    // Write isle parameters to the associated file separated with \n
    fprintf(file, "%d\n%s\n%s", isle_pid, isle_name, time);
    fclose(file);
}
