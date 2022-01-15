// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "../inc/base_header.h"
#include "../inc/cgroup_functions.h"
#include "../inc/helper_functions.h"
#include "../inc/logger_interaction/server_interaction.h"
#include "../inc/manage_data/manage_mount.h"

#include "../inc/usernamespace.h"
#include "../inc/mntnamespace.h"
#include "../inc/netnamespace.h"


static char cmd_stack[STACKSIZE];


static int child_fn(void *arg) {
    // Kill the cmd process if the isolate process die.
    if (prctl(PR_SET_PDEATHSIG, SIGKILL))
        kill_process("cannot PR_SET_PDEATHSIG for child process: %m\n");

    struct process_params *params = (struct process_params*) arg;

    // Wait for 'setup done' signal from the main process.
    await_setup(params->pipe_fd[PIPE_READ]);

    setup_mntns(SRC_ROOTFS_PATH);

    // Assuming, 0 in the current namespace maps to
    // a non-privileged UID in the parent namespace,
    // drop superuser privileges if any by enforcing
    // the exec'ed process runs with UID 0.
    set_userns_ids();

    // Move process in detach mode
    if (params->is_detached == true) {
        // redirect stdout and stderr into UNIX domain socket
        if (dup2(params->sfd, STDOUT_FILENO) < 0) {
            printf("Unable to duplicate STDOUT_FILENO");
            exit(EXIT_FAILURE);
        }
        if (dup2(params->sfd, STDERR_FILENO) < 0) {
            printf("Unable to duplicate STDERR_FILENO");
            exit(EXIT_FAILURE);
        }

        if (close(params->sfd) == -1) {
            perror("child_fn(): failed to close socket fd");
            exit(EXIT_FAILURE);
        }

        // Some commands like printf can buffers, so in that case we can not get live stream.
        // Hence, we need to change size of that printf buffer.
        setvbuf(stdout, NULL, _IONBF, 0);
        setvbuf(stderr, NULL, _IONBF, 0);

        printf("Detached mode configured\n");
    }

    char **argv = params->argv;
    char *cmd = argv[0];

    printf("\n\n=========== %s ============\n", cmd);
    if (execvp(cmd, argv) == -1)
        kill_process("Failed to exec %s: %m\n", cmd);

    printf("Child process ended, errno %s\n", strerror(errno));
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

    // Create pipe to communicate between main and command process.
    if (pipe(params.pipe_fd) < 0)
        kill_process("Failed to create pipe: %m");

    // Get UNIX socket fd. Note that process_logger should be already running.
    if (params.is_detached) {
        params.sfd = connect_to_process_logger(params.name ? params.name : "islander");
    }

    // Clone command process.
    int clone_flags =
            // if the command process exits, it leaves an exit status
            // so that we can reap it.
            SIGCHLD | CLONE_NEWUTS | CLONE_NEWUSER | CLONE_NEWNS | CLONE_NEWPID | CLONE_NEWNET;
    pid_t child_pid = clone(child_fn, cmd_stack + STACKSIZE, clone_flags, &params);

    // Kill process if failed to create.
    if (child_pid < 0)
        kill_process("Failed to clone: %m\n");

    printf("Container process PID: %ld\n", (long)child_pid);

    // Create islenode file for the isle
    if (params.name) {
        create_islenode(params.name, child_pid, exec_file_path);
    } else {
        create_islenode("islander", child_pid, exec_file_path);
    }

    // Get the writable end of the pipe.
    int pipe = params.pipe_fd[PIPE_WRITE];

    // Set proper namespace mappings to give the ROOT privileges to child process.
    set_userns_mappings(child_pid);

    // Create a network namespace if it has to be created.
    if (params.has_netns) {
        set_netns(child_pid);
    }

    // Set up cgroup limits
//    config_cgroup_limits(child_pid, &res_limits);

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

    release_resources(child_pid, &params);
}


int main(int argc, char **argv) {
//int main() {
//    int argc = 7;
//    char *argv[] = {"./islander_engine", "./project_bin/hello_sample", "--mount-aws",
//                   "bucket", "os-project-test1", "dest", "../ubuntu-rootfs/s3_bucket/"};

    printf("PID of islander_engine: %d\n", getpid());
    bool is_detached = false;
    for (int i = 1; i < argc; i++) {
         if (strcmp(argv[i], "--detach") == 0 || strcmp(argv[i], "-d") == 0) {
            is_detached = true;
            break;
         }
    }

    if (is_detached) {
        // To run in detached mode we need to create a new process before starting run_main_logic,
        // since to create our container we use clone syscall, so parent process in any case
        // need make waitpid based on clone semantic
        // (look at https://codesteps.com/2014/05/19/c-programming-creating-a-child-process-using-clone/)
        pid_t pid = fork();

        if (pid < 0) {
            fprintf(stderr, "fork failed");
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