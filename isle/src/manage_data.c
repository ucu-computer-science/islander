#include "../inc/base_header.h"
#include "../inc/manage_data.h"


void mount_dir(int isle_pid, char* src_dir_path, char* dest_dir_path) {
    pid_t parent = getpid();
    pid_t pid = fork();

    if (pid == -1) {
        perror("Failed to fork(). Errno: ");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0) {
        // We are parent process
        printf("Parent: Hello from parent\n");
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

//        int args_n = NSENTER_MNT_ARGS;
//        char arg1[] = "-t";
//        char isle_pid_str[8];
//        sprintf(isle_pid_str, "%d", isle_pid);
//        char arg3[] = "mount";
//        char arg4[] = "--bind";

        char args_arr[NSENTER_MNT_ARGS][16] = {
                "-t", "isle_pid_str", "mount",
                "--bind", "src_dir_path", "dest_dir_path"
        };
        sprintf(args_arr[1], "%d", isle_pid);
        strcpy(args_arr[4], src_dir_path);
        strcpy(args_arr[5], dest_dir_path);

        //! C вільно перетворює (несумісні) вказівники,
        //! C++ собі такого не дозволяє, тому cast.
//        char **args = static_cast<char**>(
//                malloc( (args_n + 2) * sizeof(char*)) ); //Empty array of pointers
        char **args = malloc( (NSENTER_MNT_ARGS + 2) * sizeof(char*)); //Empty array of pointers

//        args[0] = victim_name;  // Zero argument should be program name
//        args[1] = arg1;
//        args[2] = arg2;
//        args[3] = arg3;
//        args[4] = NULL;
        args[0] = victim_name;
        for (int i = 1; i < NSENTER_MNT_ARGS; i++) {
            args[i] = args_arr[i - 1];
            printf("%s\n", args[i]);
        }
        args[NSENTER_MNT_ARGS] = NULL;

//        char* path_ptr = getenv("PATH");
//        char str_to_add[] = ":.";
//        char* new_path_str = malloc( strlen(path_ptr) + strlen(str_to_add) + 1);
//        strcpy(new_path_str, path_ptr);
//        strcat(new_path_str, str_to_add);
//        setenv("PATH", new_path_str, 1);

        //! Environment is ready
        execvp(victim_name, args);

        printf("Parent: Failed to execute %s \n\tCode: %d\n", victim_name, errno);
        exit(MOUNT_FAIL);   // exec never returns
    }
}
