#include "../../inc/base_header.h"
#include "../../inc/helper_functions.h"
#include "../../inc/manage_data/manage_volumes.h"
#include "../../inc/manage_data/manage_mount.h"


void volume_feature(int isle_pid, struct process_params *params, const char *exec_file_path) {
//    char* src_dir_path = "/dev/";
//    char* dest_dir_path = "../ubuntu-rootfs/host_dev/";
    for (int i = 0; i < params->vlm_num; i++) {
//        printf("volume_feature: src -- %s, dest -- %s\n", params->vlm_src[i],  params->vlm_dst[i]);
        mount_ns_volume(isle_pid, params->vlm_src[i], params->vlm_dst[i], exec_file_path);
    }
}


void unmount_volumes(int isle_pid, struct process_params *params) {
//    char* dest_dir_path = "../ubuntu-rootfs/host_dev/";
    for (int i = 0; i < params->vlm_num; i++) {
//        printf("unvolume_dirs: dest -- %s\n", params->vlm_dst[i]);
        unmount_ns_dir(isle_pid, params->vlm_dst[i]);
    }
}


void mount_ns_volume(int isle_pid, char* src_vlm_name, char* dest_vlm_path, const char *exec_file_path) {
    char user_home_path[256];
    const char *exec_path;

    if (exec_file_path[0] == '/') {
        exec_path = exec_file_path;
    }
    else {
        char cwd[256];
        getcwd(cwd, 256);
//        printf("cwd -- %s\n", cwd);
        exec_path = cwd;
    }

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

//    printf("(uintptr_t) (ptr + 1) -- %d\n", substr_len);
    strncpy(user_home_path, exec_path, substr_len);
    user_home_path[substr_len] = '\0';
//    strncpy(user_home_path, exec_path, 16);
//    user_home_path[16] = '\0';
//    printf("mount_ns_volume(): user_home_path -- %s\n", user_home_path);

    char *str_arr[] = {user_home_path, SRC_VOLUMES_PATH, src_vlm_name};
    char abs_vlm_path[256];
    abs_vlm_path[0] = '\0';
    str_array_concat(abs_vlm_path, str_arr, 3);
//    printf("abs_vlm_path -- %s\n", abs_vlm_path);

    char *str_arr2[] = {"btrfs subvolume create ", abs_vlm_path, "> /dev/null 2>&1"};
    char btrfs_cmd[256];
    btrfs_cmd[0] = '\0';
    str_array_concat(btrfs_cmd, str_arr2, 3);
//    printf("btrfs_cmd -- %s\n", btrfs_cmd);

    FILE *cmd = popen(btrfs_cmd, "r");

//    char result[128];
//
//    while (fgets(result, sizeof(result), cmd) != NULL)
//           printf("%s", result);
    pclose(cmd);

//    if (chmod(abs_vlm_path, 777)) {
//        printf("chmod -- %s\n", strerror(errno));
//    }

    char *str_arr3[] = {"sudo chmod 770 ", abs_vlm_path, " > /dev/null 2>&1"};
    char chmod_cmd[256];
    chmod_cmd[0] = '\0';
    str_array_concat(chmod_cmd, str_arr3, 3);

    FILE *cmd2 = popen(chmod_cmd, "r");

    char result[128];
    while (fgets(result, sizeof(result), cmd2) != NULL)
           printf("%s", result);
    pclose(cmd);

    fork_vlm_mount(isle_pid, src_vlm_name, dest_vlm_path);
}


void fork_vlm_mount(int isle_pid, char* src_vlm_name, char* dest_dir_path) {
    pid_t parent = getpid();
    pid_t pid = fork();

    if (pid == -1) {
        perror("Failed to fork(). Errno: ");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0) {
        // We are parent process
//        printf("Parent: Parent PID: %d, child PID: %d\n", parent, pid);

        int status;
        waitpid(pid, &status, 0);
//        printf("Parent: child stopped, exit code: %d\n", status);
    }
    else {
        // We are the child.
        // We should not depend on process exit code, so
        // create a new process to avoid undesired shutdown of our application
        char victim_name[] = "nsenter";

        char *str_arr[] = {"subvol=", src_vlm_name};
        char subvolume[256];
        subvolume[0] = '\0';
        str_array_concat(subvolume, str_arr, 2);
//        printf("subvolume -- %s\n", subvolume);

        char args_arr[NSENTER_VLM_ARGS][256] = {
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
//            printf("%s\n", args[i]);
        }
        args[NSENTER_VLM_ARGS] = NULL;

        //! Environment is ready
        execvp(victim_name, args);

        printf("Parent: Failed to execute %s \n\tCode: %d\n", victim_name, errno);
        exit(MOUNT_FAIL);   // exec never returns
    }
}
