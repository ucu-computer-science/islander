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


void mount_az_storage_container(int isle_pid, char* src_bucket_name, char* dest_bucket_path, const char *exec_file_path) {
    char azfs_cmd[MAX_PATH_LENGTH];
    char islander_home_path[MAX_PATH_LENGTH];
    get_islander_home(islander_home_path, exec_file_path);

    char blobfuse_tmp_path[MAX_PATH_LENGTH + 32];
    sprintf(blobfuse_tmp_path, "%s%s", islander_home_path, AZ_BLOBFUSE_TMP_PATH);

    char *az_secrets_path = (char *)malloc(MAX_PATH_LENGTH);
    get_az_secrets_path(az_secrets_path, exec_file_path);

    char blobfuse_tmp_secrets_path[MAX_PATH_LENGTH + 32];
    sprintf(blobfuse_tmp_secrets_path, "%s/%s", islander_home_path, ISLANDER_TEMP_PATH);

    char file_content[1000000];
    FILE *fp = fopen(az_secrets_path, "r");
    if (fp != NULL) {
        size_t newLen = fread(file_content, sizeof(char), 1000000, fp);
        if ( ferror( fp ) != 0 ) {
            fputs("Error reading file", stderr);
        } else {
            file_content[newLen++] = '\0'; /* Just to be safe. */
        }

        fclose(fp);
    }

    // split by \n
    char delim[] = "\n";
    char file_lines[2][MAX_PATH_LENGTH];
    char file_tokens[4][MAX_PATH_LENGTH];

    uint idx_lines = 0;
    char *ptr = strtok(file_content, delim);
    while (ptr != NULL) {
        printf("'%s'\n", ptr);
        strcpy(file_lines[idx_lines++], ptr);
        ptr = strtok(NULL, delim);
    }

    // split by spaces
    char delim2[] = " ";
    uint idx_tokens = 0;
    for (uint i = 0; i < idx_lines; i++) {
        ptr = strtok(file_lines[i], delim2);
        while (ptr != NULL) {
            printf("'%s'\n", ptr);
            strcpy(file_tokens[idx_tokens++], ptr);
            ptr = strtok(NULL, delim);
        }
    }

    // setenv
    setenv("AZURE_STORAGE_ACCOUNT", file_tokens[1], 1);
    setenv("AZURE_STORAGE_ACCESS_KEY", file_tokens[3], 1);

//     in this command we use s3fs to mount s3 bucket to destination dir,
//     nonempty option is needed as our bucket, which we want to mount to our fs, can be nonempty;
//     also use su USERNAME -c to run command as non-root user, such that is can be accessed from our isle
    sprintf(azfs_cmd,
            "blobfuse %s --container-name=%s --tmp-path=%s -o allow_other",
            dest_bucket_path, src_bucket_name, blobfuse_tmp_path);
    system(azfs_cmd);

#ifdef DEBUG_MODE
    printf("azfs_cmd -- %s\n", azfs_cmd);
    printf("blobfuse_tmp_path -- %s\n", blobfuse_tmp_path);
    printf("az_secrets_path -- %s\n", az_secrets_path);
    printf("file_tokens[0] -- %s\n", file_tokens[0]);
    printf("file_tokens[1] -- %s\n", file_tokens[1]);
    printf("file_tokens[2] -- %s\n", file_tokens[2]);
    printf("file_tokens[3] -- %s\n", file_tokens[3]);
#endif

    free(az_secrets_path);
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
        char args_arr[NSENTER_UNMNT_ARGS][MAX_PATH_LENGTH] = {
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
