#include "../inc/base_header.h"
#include "../inc/cgroup_functions.h"
#include "../inc/helper_functions.h"
#include "../inc/manage_data.h"

#include "../inc/usernamespace.h"
#include "../inc/mntnamespace.h"


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

    char **argv = params->argv;
    char *cmd = argv[0];
#ifdef DEBUG_MODE
    printf("child strlen(params->argv) -- %lu\n", sizeof(params->argv) / sizeof(char*));
    printf("child strlen(cmd) -- %lu\n", strlen(cmd));
#endif
    printf("\n\n=========== %s ============\n", cmd);

    if (execvp(cmd, argv) == -1)
        kill_process("Failed to exec %s: %m\n", cmd);

    kill_process("¯\\_(ツ)_/¯");
    return 1;
}


int main(int argc, char **argv) {
    // Set Process params such as: PIPE file descriptors and Command to execute.
    struct process_params params;
    memset(&params, 0, sizeof(struct process_params));

    // Set default limits for cgroup
    resource_limits res_limits;
    set_up_default_limits(&res_limits);

    parse_args(argc, argv, &params, &res_limits);

    // Create pipe to communicate between main and command process.
    if (pipe(params.pipe_fd) < 0)
        kill_process("Failed to create pipe: %m");

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

    // Get the writable end of the pipe.
    int pipe = params.pipe_fd[PIPE_WRITE];

    // Set proper namespace mappings to give the ROOT privileges to child process.
    set_userns_mappings(child_pid);

    // set up cgroup limits
    config_cgroup_limits(child_pid, &res_limits);

//    if (params.is_mount)

    // Signal to the command process we're done with setup.
    if (write(pipe, PIPE_OK_MSG, PIPE_MSG_SIZE) != PIPE_MSG_SIZE) {
        kill_process("Failed to write to pipe: %m");
    }
    if (close(pipe)) {
        kill_process("Failed to close pipe: %m");
    }
//
//    sleep(2);
//    enable_features(child_pid);

    if (waitpid(child_pid, NULL, 0) == -1) {
        kill_process("Failed to wait pid %d: %m\n", child_pid);
    }

    rm_cgroup_dirs(child_pid);
    free(params.argv);
    return 0;
}