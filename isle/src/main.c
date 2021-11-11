#include "../inc/base_header.h"
#include "../inc/cgroup_functions.h"
#include "../inc/helper_functions.h"

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

    setup_mntns("../rootfs-alpine-stress");
//    setup_mntns("../ubuntu-rootfs");

    // Assuming, 0 in the current namespace maps to
    // a non-privileged UID in the parent namespace,
    // drop superuser privileges if any by enforcing
    // the exec'ed process runs with UID 0.
    set_userns_ids();

    char **argv = params->argv;
    char *cmd = argv[0];
//    cmd[strlen(cmd)] = '\0';
    printf("child strlen(params->argv) -- %lu\n", sizeof(params->argv) / sizeof(char*));
    printf("child strlen(cmd) -- %lu\n", strlen(cmd));
    printf("===========%s============\n", cmd);

//    argv[1] = NULL;
    if (execvp(cmd, argv) == -1)
        kill_process("Failed to exec %s: %m\n", cmd);

    kill_process("¯\\_(ツ)_/¯");
    return 1;
}


int main(int argc, char **argv) {
//int main() {
//    char **argv = {"./namespace", "sh"};
//    char **argv = {"./namespace\0", "sh\0", "--memory-in-bytes\0", "1G\0",
//                   "--cpu-quota\0", "100000\0", "--device-write-bps\0", "8:0 10485760\0"}

//    char argv[8][256] = {"./namespace", "sh", "--memory-in-bytes", "1G",
//                   "--cpu-quota", "100000", "--device-write-bps", "8:0 10485760"};

//    int argc = 8;
//    int rows = argc;
//    char **argv = calloc (rows,sizeof(char*));
//    argv[0] = "./namespace";
//    argv[1] = "sh";
//    argv[2] = "--memory-in-bytes";
//    argv[3] = "1G";
//    argv[4] = "--cpu-quota";
//    argv[5] = "100000";
//    argv[6] = "--device-write-bps";
//    argv[7] = "8:0 10485760";

    // Set Process params such as: PIPE file descriptors and Command to execute.
    struct process_params params;
    resource_limits res_limits;
    memset(&params, 0, sizeof(struct process_params));

    argv++;
    params.argv = argv;
//    parse_args(argc, argv, &params, &res_limits);
    printf("params.argv[0] -- %s\n", params.argv[0]);

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

    // Set proper namespace mappings to give the ROOT privillages to child process.
    set_userns_mappings(child_pid);

    // set up cgroup
//    config_cgroup_limits(child_pid, &res_limits);

    // Signal to the command process we're done with setup.
    if (write(pipe, PIPE_OK_MSG, PIPE_MSG_SIZE) != PIPE_MSG_SIZE) {
        kill_process("Failed to write to pipe: %m");
    }
    if (close(pipe)) {
        kill_process("Failed to close pipe: %m");
    }

    if (waitpid(child_pid, NULL, 0) == -1) {
        kill_process("Failed to wait pid %d: %m\n", child_pid);
    }

    // TODO: check if rm_cgroup_dirs works correct when we end process
//    rm_cgroup_dirs(child_pid);
    free(params.argv);

    return 0;
}