// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "../inc/base_header.h"
#include "../inc/cgroup_functions.h"
#include "../inc/helper_functions.h"
#include "../inc/funcs_with_process.h"
#include "../inc/manage_data/manage_mount.h"

#include "../inc/usernamespace.h"
#include "../inc/mntnamespace.h"


static char cmd_stack[STACKSIZE];

static int child_fn(void *arg) {
    // Kill the cmd process if the isolate process die.
    if (prctl(PR_SET_PDEATHSIG, SIGKILL))
        kill_process("cannot PR_SET_PDEATHSIG for child process: %m\n");

    struct process_params *params = (struct process_params*) arg;

    printf("before await_setup\n");

    // Wait for 'setup done' signal from the main process.
    await_setup(params->pipe_fd[PIPE_READ]);
    printf("after await_setup\n");

    setup_mntns(SRC_ROOTFS_PATH);
    printf("after setup_mntns\n");

    // Assuming, 0 in the current namespace maps to
    // a non-privileged UID in the parent namespace,
    // drop superuser privileges if any by enforcing
    // the exec'ed process runs with UID 0.
    set_userns_ids();
    printf("after set_userns_ids\n");

    // Move process in detach mode
    if (params->is_detached == true) {

//        if (close(params->log_pipe_fd[PIPE_READ])) {
//            kill_process("Failed to close read end of log pipe: %m");
//        }
        printf("before dup2\n");
        if (dup2(params->log_pipe_fd[PIPE_WRITE], STDOUT_FILENO) < 0) {
            printf("Unable to duplicate STDOUT_FILENO");
            exit(EXIT_FAILURE);
        }
        printf("after dup2 one\n");
        if (dup2(params->log_pipe_fd[PIPE_WRITE], STDERR_FILENO) < 0) {
            printf("Unable to duplicate STDERR_FILENO");
            exit(EXIT_FAILURE);
        }
        printf("after dup2 two\n");

        if (close(params->log_pipe_fd[PIPE_READ])) {
            kill_process("Failed to close read end of log pipe: %m");
        }
        if (close(params->log_pipe_fd[PIPE_WRITE]) == -1) {
            perror("child_fn(): failed to close target_file STDOUT_FILENO");
            exit(EXIT_FAILURE);
        }

//        if (close(STDERR_FILENO) == -1) {
//            perror("child_fn(): failed to close target_file STDERR_FILENO");
//            exit(EXIT_FAILURE);
//        }

        printf("closed stdout and stderr fd\n");
    }

    char **argv = params->argv;
    char *cmd = argv[0];
#ifdef DEBUG_MODE
    printf("child strlen(params->argv) -- %lu\n", sizeof(params->argv) / sizeof(char*));
    printf("child strlen(cmd) -- %lu\n", strlen(cmd));
#endif
    printf("\n\n=========== %s ============\n", cmd);

    if (execvp(cmd, argv) != 0)
        kill_process("Failed to exec %s: %m\n", cmd);

    printf("child process ended, errno %s\n", strerror(errno));
    close(params->log_pipe_fd[PIPE_WRITE]);
    kill_process("¯\\_(ツ)_/¯");
    return 1;
}


void run_main_logic(int argc, char **argv, char *exec_file_path) {
    // Set Process params such as: PIPE file descriptors and Command to execute.
    struct process_params params;
    memset(&params, 0, sizeof(struct process_params));

    // Set default limits for cgroup
    resource_limits res_limits;
    set_up_default_limits(&res_limits);

    set_up_default_params(&params);

    parse_args(argc, argv, &params, &res_limits);
    printf("after parsing\n");

    // Create pipe to communicate between main and command process.
    if (pipe(params.pipe_fd) < 0)
        kill_process("Failed to create pipe: %m");
//    if (pipe2(params.log_pipe_fd, O_DIRECT) < 0)
    if (pipe2(params.log_pipe_fd, O_NONBLOCK) < 0)
        kill_process("Failed to create log pipe: %m");

    // Clone command process.
    int clone_flags =
            // if the command process exits, it leaves an exit status
            // so that we can reap it.
            SIGCHLD | CLONE_NEWUTS | CLONE_NEWUSER | CLONE_NEWNS | CLONE_NEWPID;
    pid_t child_pid = clone(child_fn, cmd_stack + STACKSIZE, clone_flags, &params);

    // Kill process if failed to create.
    if (child_pid < 0)
        kill_process("Failed to clone: %m\n");
    printf("PID: %ld\n", (long)child_pid);

    // Create islenode file for the isle
//    if (params.name) {
//        create_islenode(params.name, child_pid);
//    } else {
//        create_islenode("islander", child_pid);
//    }

    // Get the writable end of the pipe.
    int pipe = params.pipe_fd[PIPE_WRITE];

    // Set proper namespace mappings to give the ROOT privileges to child process.
    set_userns_mappings(child_pid);

    // set up cgroup limits
//    config_cgroup_limits(child_pid, &res_limits);

    // create log process
    if (params.is_detached == true) log_process_output(params.log_pipe_fd);
    printf("send message to child\n");

    // Signal to the command process we're done with setup.
    if (write(pipe, PIPE_OK_MSG, PIPE_MSG_SIZE) != PIPE_MSG_SIZE) {
        kill_process("Failed to write to pipe: %m");
    }
    if (close(pipe)) {
        kill_process("Failed to close pipe: %m");
    }

    enable_features(child_pid, &params, exec_file_path);

    if (waitpid(child_pid, NULL, 0) == -1) {
        kill_process("Failed to wait pid %d: %m\n", child_pid);
    }

//    release_resources(child_pid, &params);
}


int main(int argc, char **argv) {
//int main() {
//    int argc = 2;
//    char *argv[] = {"./islander_engine", "/bin/bash"};
    printf("PID of islander_engine: %d\n", getpid());
    bool is_detached = false;
    for (int i = 1; i < argc; i++) {
         if (strcmp(argv[i], "--detach") == 0 || strcmp(argv[i], "-d") == 0) {
            is_detached = true;
            break;
         }
    }

    if (is_detached) {
        pid_t pid = fork();

        if (pid < 0) {
            fprintf(stderr, "fork Failed");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0) {
            // child process
            run_main_logic(argc, argv, argv[0]);
        }
    } else {
        run_main_logic(argc, argv, argv[0]);
    }
    return 0;
}