// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "../../inc/base_header.h"
#include "../../inc/helper_functions.h"
#include "../../inc/manage_data/manage_volumes.h"
#include "../../inc/manage_data/manage_mount.h"


void volume_feature(int isle_pid, struct process_params *params, const char *exec_file_path) {
    for (int i = 0; i < params->vlm_num; i++) {
        mount_ns_volume(isle_pid, params->vlm_src[i], params->vlm_dst[i], exec_file_path);
    }
}


void unmount_volumes(int isle_pid, struct process_params *params) {
    for (int i = 0; i < params->vlm_num; i++) {
        unmount_ns_dir(isle_pid, params->vlm_dst[i]);
    }
}


/** Mount subvolume src_vlm_name into dest_vlm_path **/
void mount_ns_volume(int isle_pid, char* src_vlm_name, char* dest_vlm_path, const char *exec_file_path) {
    char user_home_path[MAX_PATH_LENGTH];
    const char *exec_path;

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
        char cwd[MAX_PATH_LENGTH];
        getcwd(cwd, MAX_PATH_LENGTH);
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
    char *str_arr[] = {user_home_path, SRC_VOLUMES_PATH, src_vlm_name};
    char abs_vlm_path[MAX_PATH_LENGTH];
    abs_vlm_path[0] = '\0';
    str_array_concat(abs_vlm_path, str_arr, 3);

    // create subvolume with src_vlm_name name
    char *str_arr2[] = {"btrfs subvolume create ", abs_vlm_path, "> /dev/null 2>&1"};
    char btrfs_cmd[MAX_PATH_LENGTH];
    btrfs_cmd[0] = '\0';
    str_array_concat(btrfs_cmd, str_arr2, 3);

    FILE *cmd = popen(btrfs_cmd, "r");
    pclose(cmd);

    // give permission for thw process to make read, write and execute modes for subvolume
    char *str_arr3[] = {"sudo chmod 770 ", abs_vlm_path, " > /dev/null 2>&1"};
    char chmod_cmd[MAX_PATH_LENGTH];
    chmod_cmd[0] = '\0';
    str_array_concat(chmod_cmd, str_arr3, 3);

    FILE *cmd2 = popen(chmod_cmd, "r");
    pclose(cmd2);

    // mount subvolume to dest_vlm_path
    fork_vlm_mount(isle_pid, src_vlm_name, dest_vlm_path);
}


/** Use fork and exec with nsenter + mount to mount subvolume src_vlm_name into dest_vlm_path **/
void fork_vlm_mount(int isle_pid, char* src_vlm_name, char* dest_dir_path) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("Failed to fork(). Errno: ");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0) {
        // We are parent process
        int status;
        waitpid(pid, &status, 0);
    }
    else {
        // We are the child.
        // We should not depend on process exit code, so
        // create a new process to avoid undesired shutdown of our application
        //
        // Here we use nsenter to enter namespace and make mount command inside it.
        char victim_name[] = "nsenter";

        char *str_arr[] = {"subvol=", src_vlm_name};
        char subvolume[MAX_PATH_LENGTH];
        subvolume[0] = '\0';
        str_array_concat(subvolume, str_arr, 2);

        char args_arr[NSENTER_VLM_ARGS][MAX_PATH_LENGTH] = {
                "-t", "isle_pid_str", "mount",
                "/dev/nvme0n1p5", "-o", "subvol", "dest_dir_path"
        };
        sprintf(args_arr[1], "%d", isle_pid);
        strcpy(args_arr[5], subvolume);
        strcpy(args_arr[6], dest_dir_path);

        char **args = malloc( (NSENTER_VLM_ARGS + 2) * sizeof(char*)); //Empty array of pointers

        args[0] = victim_name;
        for (int i = 1; i < NSENTER_VLM_ARGS; i++) {
            args[i] = args_arr[i - 1];
        }
        args[NSENTER_VLM_ARGS] = NULL;

        // Environment is ready
        execvp(victim_name, args);

        printf("Parent: Failed to execute %s \n\tCode: %d\n", victim_name, errno);
        exit(MOUNT_FAIL);   // exec never returns
    }
}
