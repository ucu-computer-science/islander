#include "../../inc/base_header.h"
#include "../../inc/manage_data/manage_mount.h"


void mount_feature(int isle_pid, struct process_params *params) {
//    char* src_dir_path = "/dev/";
//    char* dest_dir_path = "../ubuntu-rootfs/host_dev/";
    for (int i = 0; i < params->mnt_num; i++) {
        printf("mount_feature: src -- %s, dest -- %s\n", params->mnt_src[i],  params->mnt_dst[i]);
        mount_ns_dir(isle_pid, params->mnt_src[i], params->mnt_dst[i]);
    }
}


void unmount_dirs(int isle_pid, struct process_params *params) {
//    char* dest_dir_path = "../ubuntu-rootfs/host_dev/";
    for (int i = 0; i < params->mnt_num; i++) {
        printf("unmount_dirs: dest -- %s\n", params->mnt_dst[i]);
        unmount_ns_dir(isle_pid, params->mnt_dst[i]);
    }
}


void mount_ns_dir(int isle_pid, char* src_dir_path, char* dest_dir_path) {
    pid_t parent = getpid();
    pid_t pid = fork();

    if (pid == -1) {
        perror("Failed to fork(). Errno: ");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0) {
        // We are parent process
        printf("Parent: Parent PID: %d, child PID: %d\n", parent, pid);

        int status;
        waitpid(pid, &status, 0);
        printf("Parent: child stopped, exit code: %d\n", status);
    }
    else {
        // We are the child.
        // We should not depend on process exit code, so
        // create a new process to avoid undesired shutdown of our application
        char victim_name[] = "nsenter";

        char args_arr[NSENTER_MNT_ARGS][256] = {
                "-t", "isle_pid_str", "mount",
                "--bind", "src_dir_path", "dest_dir_path"
        };
        sprintf(args_arr[1], "%d", isle_pid);
        strcpy(args_arr[4], src_dir_path);
        strcpy(args_arr[5], dest_dir_path);
//        strcpy(args_arr[4], "/home/denys_herasymuk/UCU/3course_1term/OS_course/Practice/UCU_OS_Course_Project/isle/tests/test_mount/");
//        strcpy(args_arr[5], "../ubuntu-rootfs/host_dev/");

        char **args = malloc( (NSENTER_MNT_ARGS + 2) * sizeof(char*)); //Empty array of pointers

        args[0] = victim_name;
        for (int i = 1; i < NSENTER_MNT_ARGS; i++) {
            args[i] = args_arr[i - 1];
            printf("%s\n", args[i]);
        }
        args[NSENTER_MNT_ARGS] = NULL;

        //! Environment is ready
        execvp(victim_name, args);

        printf("Parent: Failed to execute %s \n\tCode: %d\n", victim_name, errno);
        exit(MOUNT_FAIL);   // exec never returns
    }
}


void unmount_ns_dir(int isle_pid, char* dest_dir_path) {
    pid_t parent = getpid();
    pid_t pid = fork();

    if (pid == -1) {
        perror("Failed to fork(). Errno: ");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0) {
        // We are parent process
        printf("Parent: Parent PID: %d, child PID: %d\n", parent, pid);

        int status;
        waitpid(pid, &status, 0);
        printf("Parent: child stopped, exit code: %d\n", status);
    }
    else {
        // We are the child.
        // We should not depend on process exit code, so
        // create a new process to avoid undesired shutdown of our application
        char victim_name[] = "nsenter";

        char args_arr[NSENTER_UNMNT_ARGS][16] = {
                "-t", "isle_pid_str", "umount",
                "-R", "dest_dir_path"
        };
        sprintf(args_arr[1], "%d", isle_pid);
        strcpy(args_arr[4], dest_dir_path);

        char **args = malloc( (NSENTER_UNMNT_ARGS + 2) * sizeof(char*)); //Empty array of pointers

        args[0] = victim_name;
        for (int i = 1; i < NSENTER_UNMNT_ARGS; i++) {
            args[i] = args_arr[i - 1];
            printf("%s\n", args[i]);
        }
        args[NSENTER_UNMNT_ARGS] = NULL;

        //! Environment is ready
        execvp(victim_name, args);

        printf("Parent: Failed to execute %s \n\tCode: %d\n", victim_name, errno);
        exit(UMOUNT_FAIL);   // exec never returns
    }
}
