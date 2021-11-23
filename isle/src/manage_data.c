#include "../inc/base_header.h"
#include "../inc/manage_data.h"


void mount_dir() {
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
        // We are the child
        // We should not depend on process exit code, so
        // create a new process to avoid undesired shutdown of our application
        char victim_name[] = "nsenter";

        int args_n = 1;
        char arg1[] = "--help";
//        char arg2[] = "Dva_arg";
//        char arg3[] = "arg_three";

        //! C вільно перетворює (несумісні) вказівники,
        //! C++ собі такого не дозволяє, тому cast.
//        char **args = static_cast<char**>(
//                malloc( (args_n + 2) * sizeof(char*)) ); //Empty array of pointers
        char **args = malloc( (args_n + 2) * sizeof(char*)); //Empty array of pointers

        args[0] = victim_name;  // Zero argument should be program name
        args[1] = arg1;
//        args[2] = arg2;
//        args[3] = arg3;
//        args[4] = NULL;
        args[2] = NULL;

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
