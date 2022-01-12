#include "../../inc/manage_data/manage_remote_volumes.h"


void mount_s3_bucket(int isle_pid, char* src_bucket_name, char* dest_bucket_path, const char *exec_file_path) {
    char *islander_home_path = (char *)malloc(MAX_PATH_LENGTH);
    get_islander_home(islander_home_path, exec_file_path);
    printf("islander_home_path -- %s\n", islander_home_path);

    // make concatenations
    char *str_arr[] = {islander_home_path, S3_BUCKET_PATH, src_bucket_name};
    char abs_bucket_path[MAX_PATH_LENGTH];
    abs_bucket_path[0] = '\0';
    str_array_concat(abs_bucket_path, str_arr, 3);
    printf("abs_bucket_path -- %s\n", abs_bucket_path);

    create_dir(abs_bucket_path);

    char *aws_secrets_path = (char *)malloc(MAX_PATH_LENGTH);
    get_aws_secrets_path(aws_secrets_path, exec_file_path);

    // create s3 bucket
//    exec_s3_cmd("create", src_bucket_name);

//    FILE *cmd = popen(s3fs_cmd, "r");
//    pclose(cmd);
    char *username = get_username();
    printf("username -- %s", username);

    // create subvolume with src_vlm_name name
//    char *str_arr2[] = {"s3fs ", "os-project-test", " ../ubuntu-rootfs/s3_bucket/ -o passwd_file=../files/s3_secrets.txt ", abs_bucket_path, "> /dev/null 2>&1"};
    char s3fs_cmd[MAX_PATH_LENGTH];
//    btrfs_cmd[0] = '\0';
//    str_array_concat(btrfs_cmd, str_arr2, 3);
    sprintf(s3fs_cmd, "su %s -c \"s3fs %s %s -o nonempty -o passwd_file=%s\"", username, src_bucket_name, dest_bucket_path, aws_secrets_path);
//    sprintf(s3fs_cmd, "s3fs %s %s -o nonempty -o passwd_file=%s && chmod 777 %s", src_bucket_name, dest_bucket_path, aws_secrets_path,
//                                                                                        dest_bucket_path);
    printf("s3fs_cmd -- %s\n", s3fs_cmd);

//    FILE *cmd = popen(s3fs_cmd, "r");
//    pclose(cmd);

//    exec_bash_cmd(s3fs_cmd);

    system(s3fs_cmd);


//    char cmd[MAX_PATH_LENGTH];
//    sprintf(cmd, "sudo chmod 777 %s", dest_bucket_path);
//    printf("cmd -- %s\n", cmd);
//    exec_bash_cmd(s3fs_cmd);

//    // give permission for thw process to make read, write and execute modes for subvolume
//    char *str_arr3[] = {"sudo chmod 770 ", dest_bucket_path, " > /dev/null 2>&1"};
//    char chmod_cmd[MAX_PATH_LENGTH];
//    chmod_cmd[0] = '\0';
//    str_array_concat(chmod_cmd, str_arr3, 3);
//
//    FILE *cmd2 = popen(chmod_cmd, "r");
//    pclose(cmd2);

    // mount subvolume to dest_vlm_path
//    fork_vlm_mount(isle_pid, src_bucket_name, dest_bucket_path);
    free(islander_home_path);
    free(aws_secrets_path);
}


void exec_s3_cmd(char *operation, char *bucket_name) {
    char remote_vlm_mng_bin[MAX_PATH_LENGTH];
//    btrfs_cmd[0] = '\0';
//    str_array_concat(btrfs_cmd, str_arr2, 3);
    char *islander_bin = ISLANDER_BIN_PATH;
    char *remote_vlm_mng_name = REMOTE_VLM_MANAGER_NAME;
    sprintf(remote_vlm_mng_bin, "%s%s", islander_bin, remote_vlm_mng_name);
    printf("remote_vlm_mng_bin -- %s\n", remote_vlm_mng_bin);

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
        char **args = malloc( 5 * sizeof(char*)); // Empty array of pointers

        args[0] = remote_vlm_mng_bin;
        args[1] = operation;
        args[2] = "aws";
        args[3] = bucket_name;
//        for (int i = 1; i < NSENTER_MNT_ARGS; i++) {
//            args[i] = args_arr[i - 1];
//        }
        args[4] = NULL;

        // Environment is ready
        execvp(remote_vlm_mng_bin, args);

        printf("Parent: Failed to execute %s \n\tCode: %d\n", remote_vlm_mng_bin, errno);
        exit(MOUNT_FAIL);   // exec never returns
    }
}


//void umount_cloud_dir(int isle_pid, struct process_params *params) {
//    char cwd[MAX_PATH_LENGTH];
//    getcwd(cwd, MAX_PATH_LENGTH);
//
//    // get last level dir name
//    uint last_slash = strlen(params->remote_vlm.mnt_aws_dst) - 1;
////    uint substract_num = 1;
//    char *dest_path;
//    dest_path = params->remote_vlm.mnt_aws_dst;
//    if (params->remote_vlm.mnt_aws_dst[last_slash] != '/') {
//        char new_dest_path[MAX_PATH_LENGTH];
//        sprintf(new_dest_path, "%s/", params->remote_vlm.mnt_aws_dst);
//        printf("new_dest_path -- %s\n", new_dest_path);
//        dest_path = new_dest_path;
//    }
////    if (chdir(dest_path) != 0)
////        perror("chdir() failed");
//    if (chdir(SRC_ROOTFS_PATH) != 0)
//        perror("chdir() failed");
//
//    char cwd2[MAX_PATH_LENGTH];
//    getcwd(cwd2, 256);
//    printf("getcwd1 -- %s\n", cwd2);
//
////    if (chdir("..") != 0)
////        perror("chdir() failed");
//
//    for (uint i = strlen(dest_path) - 2; i >= 0; i--) {
//        if (dest_path[i] == '/') {
//            last_slash = i;
//            break;
//        }
//    }
//
//    char last_dir_name[MAX_PATH_LENGTH], chdir_last_dir_name[MAX_PATH_LENGTH + 1];
//    last_dir_name[0] = '.';
//    uint substr_len = strlen(dest_path) - last_slash;
//    strncpy(last_dir_name, (dest_path + last_slash), substr_len);
//    last_dir_name[substr_len] = '\0';
//    sprintf(chdir_last_dir_name, ".%s", last_dir_name);
//    printf("chdir_last_dir_name -- %s\n", chdir_last_dir_name);
//
//    getcwd(cwd, 256);
//    char cwd3[MAX_PATH_LENGTH];
//    getcwd(cwd3, 256);
//    printf("getcwd2 -- %s\n", cwd3);
////    char *a1 = "./s3_bucket";
////    unmount_ns_dir(isle_pid, chdir_last_dir_name);
//
//    char cmd[MAX_PATH_LENGTH + 32];
//    sprintf(cmd, "umount -l %s", chdir_last_dir_name);
////    sprintf(cmd, "su denys_herasymuk -c \"sudo umount -R %s\"", chdir_last_dir_name);
//    printf("cmd -- %s\n", cmd);
//    system(cmd);
//
//    chdir(cwd);
//}



/** Unmount mounted directories from dest_dir_path **/
void umount_cloud_dir(int isle_pid, char* dest_dir_path) {
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

        char args_arr[NSENTER_UNMNT_ARGS][256] = {
                "-t", "isle_pid_str", "umount",
                "-l", "dest_dir_path"
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
