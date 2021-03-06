// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "../../inc/manage_data/manage_mount.h"


void mount_feature(int isle_pid, struct process_params *params) {
    for (int i = 0; i < params->mnt_num; i++) {
        mount_ns_dir(isle_pid, params->mnt_src[i], params->mnt_dst[i]);
    }
}


void unmount_dirs(int isle_pid, struct process_params *params) {
    for (int i = 0; i < params->mnt_num; i++) {
        unmount_ns_dir(isle_pid, params->mnt_dst[i]);
    }
}

/** Mount src_dir_path into dest_dir_path **/
void mount_ns_dir(int isle_pid, char* src_dir_path, char* dest_dir_path) {
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

        char args_arr[NSENTER_MNT_ARGS][MAX_PATH_LENGTH] = {
                "-t", "isle_pid_str", "mount",
                "--bind", "src_dir_path", "dest_dir_path"
        };
        sprintf(args_arr[1], "%d", isle_pid);
        strcpy(args_arr[4], src_dir_path);
        strcpy(args_arr[5], dest_dir_path);

        char **args = malloc( (NSENTER_MNT_ARGS + 2) * sizeof(char*)); // Empty array of pointers

        args[0] = victim_name;
        for (int i = 1; i < NSENTER_MNT_ARGS; i++) {
            args[i] = args_arr[i - 1];
        }
        args[NSENTER_MNT_ARGS] = NULL;

        // Environment is ready
        execvp(victim_name, args);

        printf("Parent: Failed to execute %s \n\tCode: %d\n", victim_name, errno);
        exit(MOUNT_FAIL);   // exec never returns
    }
}


/** Unmount mounted directories from dest_dir_path **/
void unmount_ns_dir(int isle_pid, char* dest_dir_path) {
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
        // Here we use nsenter to enter namespace and make umount command inside it.
        char victim_name[] = "nsenter";

        char args_arr[NSENTER_UNMNT_ARGS][MAX_PATH_LENGTH] = {
                "-t", "isle_pid_str", "umount",
                "-R", "dest_dir_path"
        };
        sprintf(args_arr[1], "%d", isle_pid);
        strcpy(args_arr[4], dest_dir_path);

        char **args = malloc( (NSENTER_UNMNT_ARGS + 2) * sizeof(char*)); //Empty array of pointers

        args[0] = victim_name;
        for (int i = 1; i < NSENTER_UNMNT_ARGS; i++) {
            args[i] = args_arr[i - 1];
        }
        args[NSENTER_UNMNT_ARGS] = NULL;

        // Environment is ready
        execvp(victim_name, args);

        printf("Parent: Failed to execute %s \n\tCode: %d\n", victim_name, errno);
        exit(UMOUNT_FAIL);   // exec never returns
    }
}
