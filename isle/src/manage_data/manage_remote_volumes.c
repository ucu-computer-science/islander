#include "../../inc/manage_data/manage_remote_volumes.h"


void mount_s3_bucket(int isle_pid, char* src_bucket_name, char* dest_bucket_path, const char *exec_file_path) {
    char *islander_home_path = (char *)malloc(256);
    get_islander_home(islander_home_path, exec_file_path);
    printf("islander_home_path -- %s\n", islander_home_path);

    // make concatenations
    char *str_arr[] = {islander_home_path, S3_BUCKET_PATH, src_bucket_name};
    char abs_bucket_path[256];
    abs_bucket_path[0] = '\0';
    str_array_concat(abs_bucket_path, str_arr, 3);
    printf("abs_bucket_path -- %s\n", abs_bucket_path);

    create_dir(abs_bucket_path);

    char *aws_secrets_path = (char *)malloc(256);
    get_aws_secrets_path(aws_secrets_path, exec_file_path);

    // create s3 bucket
    exec_s3_cmd("create", src_bucket_name);
//    FILE *cmd = popen(s3fs_cmd, "r");
//    pclose(cmd);

    // create subvolume with src_vlm_name name
//    char *str_arr2[] = {"s3fs ", "os-project-test", " ../ubuntu-rootfs/s3_bucket/ -o passwd_file=../files/s3_secrets.txt ", abs_bucket_path, "> /dev/null 2>&1"};
    char s3fs_cmd[256];
//    btrfs_cmd[0] = '\0';
//    str_array_concat(btrfs_cmd, str_arr2, 3);
    sprintf(s3fs_cmd, "s3fs %s %s -o passwd_file=%s", src_bucket_name, dest_bucket_path, aws_secrets_path);
    printf("s3fs_cmd -- %s\n", s3fs_cmd);

//    FILE *cmd = popen(s3fs_cmd, "r");
//    pclose(cmd);
    exec_bash_cmd(s3fs_cmd);

    // mount subvolume to dest_vlm_path
//    fork_vlm_mount(isle_pid, src_bucket_name, dest_bucket_path);
    free(islander_home_path);
    free(aws_secrets_path);
}


void exec_s3_cmd(char *operation, char *bucket_name) {
    char remote_vlm_mng_bin[256];
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
