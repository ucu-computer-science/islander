// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>

#include "../include/pipe.h"
#include "../include/execution_functs.hpp"
#include "../include/redirect.h"
#include "../include/utils.hpp"
#include "../include/defined_vars.hpp"


#define PIPE_READ_FD 0
#define PIPE_WRITE_FD 1

#define NO_READ_FD 0     // indicates that command does not read from anywhere (usually first command of pipe)
#define NO_WRITE_FD 1    // indicates that command does not write to anywhere (usually last command of pipe)


extern std::map<std::string, std::function<void (command_line_input&, int, int)>> command_to_function;
extern std::map<std::string, std::string> command_to_help;
extern bool need_encryption;


/* Close all unused ends of pipe and duplicate the pipe end for either read or write or both.
 * in: the READ end of pipe (fd)
 * out: the WRITE end of pipe (fd) */
//int prepare_pipe(int in, int out, command_line_input &cmd, int pfd[2], bool detached_mode,
//                 std::string redirect_type) {
//    int status = 0;
//    pid_t pid = fork();
//
//    // dict of file descriptors to close in case redirection command for detached mode
//    std::map<std::string, std::vector<int>> dict {
//            { "2>&1", { STDIN_FILENO } },
//            { "&>", { STDIN_FILENO } },
//            { "2>", { STDIN_FILENO, STDOUT_FILENO } },
//            { ">", { STDIN_FILENO, STDERR_FILENO } }
//    };
//
//    if (pid == 0) {  // If we are a child process.
//        if (detached_mode) {
//            if (redirect_type == NOT_REDIRECT) {
//                std::vector<int> std_fds = {STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO};
//                for (auto &std_fd: std_fds) {
//                    if (close(std_fd) == -1)
//                        encrypted_cerr_if_needed("prepare_pipe(): failed to close std fd for detached mode");
//                }
//            } else {
//                for (auto &std_fd: dict[redirect_type]) {
//                    if (close(std_fd) == -1)
//                        encrypted_cerr_if_needed("prepare_pipe(): failed to close std fd for detached mode");
//                }
//            }
//        }
//        // Close the read end of pipe as we don't use it here. Except for the last command in pipe.
//        if (out != NO_WRITE_FD) {
//            if (close(pfd[PIPE_READ_FD]) == -1)
//                encrypted_cerr_if_needed("pipe: failed to close the read end of pipe");
//        }
//
//        if (in != NO_READ_FD) {  // If we are reading.
//            // Duplicate stdin on read end of pipe. Close duplicate descriptor.
//            if (dup2(in, STDIN_FILENO) == -1)
//                encrypted_cerr_if_needed("dup2: failed to duplicate stdin on read end of pipe");
//            if (close(in) == -1)
//                encrypted_cerr_if_needed("pipe: failed to close the read end of pipe");
//        }
//
//        if (out != NO_WRITE_FD) {  // If we are writing.
//            // Duplicate stdout on write end of pipe. Close duplicate descriptor.
//            if (dup2(out, STDOUT_FILENO) == -1)
//                encrypted_cerr_if_needed("dup2: failed to duplicate stdout on write end of pipe");
//            if (close(out) == -1)
//                encrypted_cerr_if_needed("pipe: failed to close the write end of pipe");
//        }
//        if (redirect_type != NOT_REDIRECT) {
//            command_line_input input_struct;
//            std::string target_file, command = concat_command(cmd);
//            process_redirect_command_line(input_struct, target_file, command, redirect_type);
//            redirect_cmd(input_struct, target_file, redirect_type, command_to_function, command_to_help, true);
//        }
//        else exec_command(cmd);
//        return 0;
//    }
//    else {
//        if (detached_mode) {
//            signal(SIGCHLD, SIG_IGN);
//        } else {
//            waitpid(pid, &status, 0);
//        }
//    }
//    return pid;
//}
//

/* Execute pipe command. */
//int fork_pipes(std::vector<command_line_input> &pipe_commands, bool detached_mode) {
//    int i, in, pfd[2];
//
//    /* The first process should get its input from the original file descriptor 0.  */
//    in = PIPE_READ_FD;
//    /* Note the loop bound, we spawn here all, but the last stage of the pipeline.  */
//    for (i = 0; i < pipe_commands.size() - 1; ++i) {
//        /* Skip redirect command in the middle of the pipe. */
//        if (is_redirect(pipe_commands[i]) != NOT_REDIRECT) {
//            // for convenience do not use reference on the message in encrypted_cerr_if_needed()
//            encrypted_cerr_if_needed("pipe: redirect should be the last command of a pipe, current redirect will be skipped");
//            continue;
//        }
//        /* Create pipe for every command except for last one as it uses the previous fd. */
//        if (pipe(pfd) == -1)
//            encrypted_cerr_if_needed("pipe: failed to create pipe");
//
//        /* f[1] is the write end of the pipe, we carry `in` from the prev iteration.  */
//        if (command_to_function.count(pipe_commands[i].command)) { // if we are running internal command.
//            if (i != 0) {
//                encrypted_cerr_if_needed("pipe: internal command can be only first command in pipe");
//                return -1;
//            }
//            std::function<void (command_line_input&, int, int)> command = command_to_function[pipe_commands[i].command];
//            command(pipe_commands[i], pfd[PIPE_WRITE_FD], STDERR_FILENO);
//        } else {
//            prepare_pipe(in, pfd[PIPE_WRITE_FD], pipe_commands[i], pfd, detached_mode, NOT_REDIRECT);
//        }
//
//        /* No need for the write end of the pipe, the child will write here. */
//        if (close(pfd[PIPE_WRITE_FD]) == -1)
//            encrypted_cerr_if_needed("pipe: failed to close the write end of pipe");
//
//        /* Close previous READ end of file descriptor as info was already read from it. */
//        if (in != NO_READ_FD)
//            if (close(in) == -1)
//                encrypted_cerr_if_needed("pipe: failed to close the write end of pipe");
//
//        /* Keep the read end of the pipe, the next child will read from there.  */
//        in = pfd[PIPE_READ_FD];
//    }
//
//    /* Last stage of the pipeline - set stdin be the read end of the previous pipe
//       and output to the original file descriptor 1. */
//    /* Execute the last stage with the current process. */
//    std::string redirect_type = NOT_REDIRECT;
//    if ((redirect_type = is_redirect(pipe_commands[i])) != NOT_REDIRECT)   // If last command is redirect.
//        prepare_pipe(in, NO_WRITE_FD, pipe_commands[i], pfd, detached_mode, redirect_type);
//    else {
//        if (!need_encryption) {
//            prepare_pipe(in, NO_WRITE_FD, pipe_commands[i], pfd, detached_mode, redirect_type);
//            return 0;
//        }
//
//        /* redirect stdout of the last command in pipe into write end of pipe, from which parent process can get an output for encryption */
//        pid_t pid = fork();
//
//        if (pid == -1) {
//            encrypted_cerr_if_needed("Failed to fork()");
//            exit_wrapper(EXIT_FAILURE);
//        }
//        else if (pid > 0) {
//            int *status = nullptr;
//
//            // We are parent process
//            waitpid(pid, status, 0);
//            close(get_write_pipe_fd(false, false));
//        }
//        else {
//            // Redirect stdout & stderr in write_pipe_fd for encryption mode
//            if (dup2(get_write_pipe_fd(false, -1), STDOUT_FILENO) == -1) {
//                encrypted_cerr_if_needed("dup2: failed to duplicate stdout on socket fd");
//                exit(EXIT_FAILURE);
//            }
//            if (dup2(get_write_pipe_fd(false, -1), STDERR_FILENO) == -1) {
//                encrypted_cerr_if_needed("fork_pipes(), dup2: failed to duplicate stderr on socket fd");
//                exit(EXIT_FAILURE);
//            }
//            if (close(get_write_pipe_fd(false, -1)) == -1) {
//                encrypted_cerr_if_needed("pipe: failed to close the read end of pipe");
//                exit(EXIT_FAILURE);
//            }
//            prepare_pipe(in, NO_WRITE_FD, pipe_commands[i], pfd, detached_mode, redirect_type);
//            exit(SUCCESS);
//        }
//    }
//    return 0;
//}