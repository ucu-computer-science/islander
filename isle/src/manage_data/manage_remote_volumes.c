#include "../../inc/manage_data/manage_remote_volumes.h"


void mount_s3_bucket(int isle_pid, char* src_bucket_name, char* dest_bucket_path, const char *exec_file_path) {
    char *islander_home_path = (char *)malloc(MAX_PATH_LENGTH);
    get_islander_home(islander_home_path, exec_file_path);

    char *aws_secrets_path = (char *)malloc(MAX_PATH_LENGTH);
    get_aws_secrets_path(aws_secrets_path, exec_file_path);

    char *username = get_username();
    char s3fs_cmd[MAX_PATH_LENGTH];

    // in this command we use s3fs to mount s3 bucket to destination dir,
    // nonempty option is needed as our bucket, which we want to mount to our fs, can be nonempty;
    // also use su USERNAME -c to run command as non-root user, such that is can be accessed from our isle
    sprintf(s3fs_cmd, "su %s -c \"s3fs %s %s -o nonempty -o passwd_file=%s\"", username, src_bucket_name, dest_bucket_path, aws_secrets_path);
    system(s3fs_cmd);

    free(islander_home_path);
    free(aws_secrets_path);
}


/** Unmount mounted from clouds directories located in dest_dir_path **/
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

        // here we need to apply -l option, since we use s3fs for mounting
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
