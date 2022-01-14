// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <boost/algorithm/string.hpp>
#include <sys/wait.h>

#include <fcntl.h>    /* For O_RDWR */
#include <unistd.h>   /* For open(), creat() */

#include "../include/defined_vars.hpp"
#include "../include/execution_functs.hpp"
#include "../include/utils.hpp"

int merrno_status = OK;

/** Run external command via fork and exec syscalls */
void run_external_command(int* read_pfd, int* write_pfd, std::string& cmd, char *devname, int* ptm) {
    pid_t pid = fork();

    if (pid == -1) {
        encrypted_cerr_if_needed("Failed to fork()");
        exit_wrapper(EXIT_FAILURE);
    }
    else if (pid > 0) {
        // We are parent
        signal(SIGCHLD, SIG_IGN);

        // close write end of pipe to send EOF to read end of pipe
        close(read_pfd[ENCRYPTED_PIPE_WRITE]);
        close(write_pfd[ENCRYPTED_PIPE_READ]);
    }
    else {
        // close write end of pipe to send EOF to read end of pipe
        close(read_pfd[ENCRYPTED_PIPE_READ]);
        close(write_pfd[ENCRYPTED_PIPE_WRITE]);

        close(*ptm);  // Close pseudo terminal master
        setsid();  // Detach from the current TTY
        int pts = open(devname, O_RDWR);  // Open slave
        if (pts < 0) exit(FAIL);  // TODO: signal to handler that we are dead

        exec_command(read_pfd, write_pfd, cmd, &pts);
    }
}


void exec_command(int* read_pfd, int* write_pfd, std::string& cmd, int* pts) {
    // We are the child
    static char* engine_path = "./isle/build";
    chdir(engine_path);
    // Add dot to PATH
    std::string command =  "./islander_engine";  //
    std::string victim_name(command);

    std::vector<std::string> args;
    boost::split(args, cmd, boost::is_any_of(" "));

    args.insert(args.begin(), victim_name); // Zero argument should be program name

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

    /* For external commands we need to enable encryption also, but we have no access to their i/o functions.
     * Hence, we use dup2() to redirect their output from stdout and stderr into write end of the pipe from,
     * which the parent process can read and encrypt that output */
    int saved_stdout, saved_stderr, saved_stdin;

    // used to return to initial standard descriptors in case of error in exec() with input command
    saved_stdout = dup(STDOUT_FILENO);
    saved_stderr = dup(STDERR_FILENO);
    saved_stdin = dup(STDIN_FILENO);

    // Redirect stdout & stderr in write_pipe_fd for encryption mode
    if (dup2(*pts, STDIN_FILENO) == -1) {
        encrypted_cerr_if_needed("dup2: failed to duplicate stdin on socket fd");
        exit(EXIT_FAILURE);
    }
    if (dup2(*pts, STDOUT_FILENO) == -1) {
        encrypted_cerr_if_needed("dup2: failed to duplicate stdout on socket fd");
        exit(EXIT_FAILURE);
    }
    if (dup2(*pts, STDERR_FILENO) == -1) {
        encrypted_cerr_if_needed("dup2: failed to duplicate stderr on socket fd");
        exit(EXIT_FAILURE);
    }

//    std::cout << "ARG LIST:" << std::endl;
//    for (auto arg : args){
//        std::cout << "|" << arg << "|" << std::endl;
//    }
//    std::cout << "END" << std::endl;

    //! const_cast is used because I don't see another way
    //! with const char ** get the char * const * that function wants
    execvp(victim_name.c_str(), const_cast<char* const*>(arg_for_c.data()));

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
    if (dup2(saved_stderr, STDERR_FILENO) == -1) {
        encrypted_cerr_if_needed("dup2: failed to duplicate stderr on socket fd");
        exit(EXIT_FAILURE);
    }

    close(*pts);

    exit(EXIT_FAILURE);
}