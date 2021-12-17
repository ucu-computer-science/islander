#include "../../inc/manage_data/manage_tmpfs.h"



/** Mount src_dir_path into dest_dir_path **/
void mount_ns_tmpfs(int isle_pid, struct process_params *params) {
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

        char args_arr[NSENTER_TMPFS_ARGS][256] = {
                "-t", "<ISLE_PID_STR>", "mount",
                "-t", "tmpfs", "-o",
//                "size=", "nr_inodes=", "mode=770",
                "<CONFIGS>",
//                "tmpfs",
                "/dev/shm",
                "<MOUNT_POINT>"
        };
        sprintf(args_arr[1], "%d", isle_pid);

        // make concatenations
        // tmpfs_size
        char *str_arr[] = {"size=", params->tmpfs_size, ",nr_inodes=", params->tmpfs_nr_inodes, ",mode=770"};
        char tmpfs_size[256];
        tmpfs_size[0] = '\0';
        str_array_concat(tmpfs_size, str_arr, 5);
        strcpy(args_arr[6], tmpfs_size);

//        // nr_inodes
//        char *str_arr2[] = {args_arr[7], params->tmpfs_nr_inodes};
//        char tmpfs_nr_inodes[256];
//        tmpfs_nr_inodes[0] = '\0';
//        str_array_concat(tmpfs_nr_inodes, str_arr2, 2);
//        strcpy(args_arr[7], tmpfs_nr_inodes);
//
        // tmpfs_dst
        strcpy(args_arr[8], params->tmpfs_dst);

        char **args = malloc( (NSENTER_TMPFS_ARGS + 2) * sizeof(char*)); // Empty array of pointers

        args[0] = victim_name;
        for (int i = 1; i < NSENTER_TMPFS_ARGS; i++) {
            args[i] = args_arr[i - 1];
        }
        args[NSENTER_TMPFS_ARGS] = NULL;

        for (int i = 0; i < NSENTER_TMPFS_ARGS; i++) {
            printf("%s\n", args[i]);
        }

        // Environment is ready
        execvp(victim_name, args);

        printf("Parent: Failed to execute %s \n\tCode: %d\n", victim_name, errno);
        exit(MOUNT_FAIL);   // exec never returns
    }
}
