// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "../inc/funcs_with_process.h"


void log_process_output(int log_pipe_fd[PIPE_FD_NUM]) {
    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "fork Failed");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0) {
        // parent process
//        int merrno_status;
//        waitpid(pid, &merrno_status, 0);
//        printf("Parent in log_process_output(): child stopped, exit code %d\n", merrno_status);
    }
    else {
        // child process
        printf("Created log process, PID: %d\n", getpid());
        if (close(log_pipe_fd[PIPE_WRITE])) {
            kill_process("Failed to close write end of log pipe: %m");
        }
        printf("after close(log_pipe_fd[PIPE_WRITE])\n");
        int buf_size = 1024 * 10; // 10 Kb
//        int buf_size = 20;
        char process_log[buf_size];
        int *status;
        int n_bytes;
        for (;;) {
//            printf("before read_in_buffer\n");
//            read_in_buffer(log_pipe_fd[PIPE_READ], process_log, buf_size, status);
            read(log_pipe_fd[PIPE_READ], process_log, buf_size);
//            printf("%d\n", n_bytes);
//            printf("%s\n", strerror(errno));
//            printf("after read_in_buffer\n");
            if (process_log[0] == EOF) {
                printf("Log process received EOF\n");
                break;
            }
//            printf("after EOF\n");

            if (strlen(process_log) > 1) {
                printf("Log process output: %s\n", process_log);
//                printf("LL%s", process_log);
            }
            process_log[0] = '\0'; // clear buffer in case read returns zero
        }
        close(log_pipe_fd[PIPE_READ]);
        exit(0);   // exec never returns
    }
}
