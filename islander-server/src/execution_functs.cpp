// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <boost/algorithm/string.hpp>
#include <sys/wait.h>
#include <fstream>
#include <sstream>
#include <cerrno>
#include <map>

#include "../include/defined_vars.hpp"
#include "../include/internal_commands.hpp"
#include "../include/execution_functs.hpp"
#include "../include/utils.hpp"
#include "../include/pipe.h"
#include "../include/redirect.h"
#include "../include/enclosed_cmd.h"

int merrno_status = OK;

/* here we use global variable, since encrypted mode is used for all clients */
extern bool need_encryption;


/** Run external command via fork and exec syscalls */
void run_external_command(int* read_pfd, int* write_pfd, std::string& cmd) {
    pid_t pid = fork();

    if (pid == -1) {
        encrypted_cerr_if_needed("Failed to fork()");
        exit_wrapper(EXIT_FAILURE);
    }
    else if (pid > 0) {
        // We are parent
        if (false) {
            signal(SIGCHLD, SIG_IGN);
        } else {
            // We are parent process
            signal(SIGCHLD, SIG_IGN);
//            waitpid(pid, &merrno_status, 0);

            // close write end of pipe to send EOF to read end of pipe
            close(read_pfd[ENCRYPTED_PIPE_WRITE]);
            close(write_pfd[ENCRYPTED_PIPE_READ]);
        }
    }
    else {
        // We are child
//        if (true) {
//            std::vector<int> std_fds = {STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO};
//            for (auto &std_fd : std_fds) {
//                if (close(std_fd) == -1)
//                    encrypted_cerr_if_needed("fork_redirect(): failed to close std fd for detached mode");
//            }
//        }
        // close write end of pipe to send EOF to read end of pipe
        close(read_pfd[ENCRYPTED_PIPE_READ]);
        close(write_pfd[ENCRYPTED_PIPE_WRITE]);
        exec_command(read_pfd, write_pfd, cmd);
    }
}


void exec_command(int* read_pfd, int* write_pfd, std::string& cmd) {
    // We are the child
    chdir("../../isle/build/");
    // Add dot to PATH
    std::string command =  "./islander_engine"; // "./midterm";  //
    std::string victim_name(command);

    std::vector<std::string> args;
    boost::split(args, cmd, boost::is_any_of(" "));

    args.insert(args.begin(), victim_name); // Zero argument should be program name

//    std::cout << "ARG LIST:" << std::endl;
//    for (auto arg : args){
//        std::cout << "|" << arg << "|" << std::endl;
//    }
//    std::cout << "END" << std::endl;

    //! Assume that the number of arguments at the time of compilation is unknown!
    //! This situation will be used by us often, corrections.
    //! Otherwise there is no need to do so difficult.
//    std::vector<std::string> args;
//    args.push_back(victim_name);
//    args.emplace_back("/bin/bash");

//    for (auto &flag: input_struct.flags) {
//        args.emplace_back(flag);
//    }
//
//    for (auto &arg: input_struct.args) {
//        args.emplace_back(arg);
//    }

    //! Environment is ready
    //! Prepare args array in form suitable for execvp
    std::vector<const char*> arg_for_c;

    //! "&" before the "auto" is critically important!
    //! Without it the "s" would be the temporary copy,
    //! destroyed at the end of the iteration -- and we
    //! would have dangling pointer.
    arg_for_c.reserve(args.size() + 1);
    for(const auto& s: args)
        arg_for_c.push_back(s.c_str());

    arg_for_c.push_back(nullptr);

#ifdef DEBUG_MODE
    cout << "before if (need_encryption) -- " << need_encryption << endl;
    cout << "before if cmd_is_pipe -- " << cmd_is_pipe(false, false) << endl;
    cout << "before if cmd_is_redirect -- " << cmd_is_redirect(false, false) << endl;
    cout << "before if cmd_is_enclosed -- " << cmd_is_enclosed(false, false) << endl;
#endif

    /* For external commands we need to enable encryption also, but we have no access to their i/o functions.
     * Hence, we use dup2() to redirect their output from stdout and stderr into write end of the pipe from,
     * which the parent process can read and encrypt that output */
    int saved_stdout, saved_stderr, saved_stdin;

    // used to return to initial standard descriptors in case of error in exec() with input command
    saved_stdout = dup(STDOUT_FILENO);
    saved_stderr = dup(STDERR_FILENO);
    saved_stdin = dup(STDIN_FILENO);

//    std::cout<< "aha" << std::endl;

    // Redirect stdout & stderr in write_pipe_fd for encryption mode
    if (dup2(write_pfd[ENCRYPTED_PIPE_READ], STDIN_FILENO) == -1) {
        encrypted_cerr_if_needed("dup2: failed to duplicate stdin on socket fd");
        exit(EXIT_FAILURE);
    }
    if (dup2(read_pfd[ENCRYPTED_PIPE_WRITE], STDOUT_FILENO) == -1) {
        encrypted_cerr_if_needed("dup2: failed to duplicate stdout on socket fd");
        exit(EXIT_FAILURE);
    }
    if (dup2(read_pfd[ENCRYPTED_PIPE_WRITE], STDERR_FILENO) == -1) {
        encrypted_cerr_if_needed("dup2: failed to duplicate stderr on socket fd");
        exit(EXIT_FAILURE);
    }

    //! const_cast is used because I don't see another way
    //! with const char ** get the char * const * that function wants
    execvp(victim_name.c_str(), const_cast<char* const*>(arg_for_c.data()));

    std::cout<< "back" << std::endl;

    /* in case, when the command failed */
    std::string msg = "Parent: Failed to execute " + victim_name + " \n\t";
    encrypted_cerr_if_needed(msg);
    encrypted_cerr_if_needed("Possible reasons: \n\t - file not found;\n\t - file is not executable;\n\t - typo in command line;\n\t - typo in flag names;");

    // Redirect stdout & stderr in write_pipe_fd for encryption mode
    if (dup2(saved_stdin, STDIN_FILENO) == -1) {
        encrypted_cerr_if_needed("dup2: failed to duplicate stdin on socket fd");
        exit(EXIT_FAILURE);
    }
    if (dup2(saved_stdout, STDOUT_FILENO) == -1) {
        encrypted_cerr_if_needed("dup2: failed to duplicate stdout on socket fd");
        exit(EXIT_FAILURE);
    }
//    if (dup2(saved_stderr, STDERR_FILENO) == -1) {
//        encrypted_cerr_if_needed("dup2: failed to duplicate stderr on socket fd");
//        exit(EXIT_FAILURE);
//    }

    close(get_write_pipe_fd(false, false));

    exit_if_needed(EXIT_FAILURE);
}