// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <sys/stat.h> // Structures (types) for fstat(), lstat (), stat ().
#include <fcntl.h> // Types and constants for fcntl() and open().
#include <sys/wait.h>
#include <unistd.h>

#include "../include/redirect.h"
#include "../include/utils.hpp"
#include "../include/wrappers.h"
#include "../include/base_header.hpp"
#include "../include/execution_functs.hpp"

extern int merrno_status;
extern bool need_encryption;

//void redirect_cmd(
//        command_line_input &input_struct,
//        std::string &target_file,
//        std::string &redirect_type,
//        std::map<std::string, std::function<void (command_line_input&, int, int)>> &command_to_function,
//        std::map<std::string, std::string> &command_to_help,
//        bool piped) {
//    if (command_to_function.count(input_struct.command)) { // if command is internal
//        if (redirect_type == "<") {
//            encrypted_cerr_if_needed("redirect_cmd(): internal command can not use stdin redirection. Errno");
//            if (piped) exit_wrapper(0); // exit the forked process that was created because of pipe.
//            return;
//        }
//
//        std::string target_file_path = target_file;
//        std::size_t found = target_file.find('/');
//        if (found == std::string::npos) {
//            target_file_path = "./" + target_file;
//        }
//#ifdef DEBUG_MODE
//        cout << "redirect_cmd() target_file_path -- " << target_file_path << endl;
//#endif
//        int oflag = (redirect_type == ">" || redirect_type == "2>" ||
//                     redirect_type == "2>&1" || redirect_type == "&>") ? O_WRONLY: O_RDONLY;
//        int fd = open(target_file_path.c_str(), oflag | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
//
//        // set up correct stdout and stderr file descriptors to use them for redirection for internal commands
//        std::map<std::string, std::vector<int>> dict {
//                { "2>&1", { fd, fd } },
//                { "&>", { fd, fd } },
//                { "2>", { STDOUT_FILENO, fd } },
//                { ">", { fd, STDERR_FILENO } }
//        };
//
//        // skip input of internal command with incorrect flag
//        if (!input_struct.flags.empty() && input_struct.flags[0] != "-h" && input_struct.flags[0] != "--help") {
//            if ( ! (input_struct.detached_mode && dict[redirect_type][1] == STDERR_FILENO) ) {
//                std::string buf = "Error: you input incorrect option for the command\n";
//                encrypted_write_buffer_if_needed(dict[redirect_type][1], buf);
//            }
//        }
//        // print --help message
//        else if (input_struct.help_flag && command_to_help.find(input_struct.command) != command_to_help.end()) {
//            encrypted_write_buffer_if_needed(dict[redirect_type][0], command_to_help[input_struct.command]);
//        }
//        else {
//            std::function<void(command_line_input &, int, int)> command = command_to_function[input_struct.command];
//            command(input_struct, dict[redirect_type][0], dict[redirect_type][0]);
//        }
//        if (piped) exit_wrapper(0); // exit the forked process that was created because of pipe.
//    }
//    else { // if command is external
//        fork_redirect(input_struct, target_file, redirect_type, piped);
//    }
//}
//
//
//void fork_redirect(const command_line_input &input_struct, const std::string &target_file, const std::string &redirect_type, bool piped) {
//    if (target_file.length() < 3)
//        return;
//
//    /* note that redirect here is only with external command, as internal commands we processed previously */
//    if (need_encryption) {
//        cmd_is_external(true, true);
//    }
//
//    pid_t pid = fork();
//#ifdef DEBUG_MODE
//    cout << "redirect_type -- " << redirect_type << endl;
//#endif
//
//    if (pid == -1) {
//        encrypted_cerr_if_needed("Failed to fork()");
//        exit_wrapper(EXIT_FAILURE);
//    }
//    else if (pid > 0) {
//        // We are parent process
//        if (input_struct.detached_mode) {
//            signal(SIGCHLD, SIG_IGN);
//        } else {
//            waitpid(pid, &merrno_status, 0);
//
//
//            if (need_encryption) {
//                // close write end of pipe to send EOF to read end of pipe
//                close(get_write_pipe_fd(false, false));
//            }
//
//            if (piped) exit(SUCCESS); // exit the forked process that was created because of pipe.
//        }
//#ifdef DEBUG_MODE
//        cout << "Parent: child stopped, exit code: " << merrno_status << endl;
//#endif
//    }
//    else {
//        std::size_t found = target_file.find('/');
//        std::string target_file_path = target_file;
//        if (found == std::string::npos) {
//            target_file_path = "./" + target_file;
//        }
//#ifdef DEBUG_MODE
//        cout << "target_file_path -- " << target_file_path << endl;
//#endif
//        // close std file descriptors for detached mode which were not redirected
//        if (input_struct.detached_mode) {
//            std::map<std::string, std::vector<int>> dict {
//                    { "2>&1", { STDIN_FILENO } },
//                    { "&>", { STDIN_FILENO } },
//                    { "2>", { STDIN_FILENO, STDOUT_FILENO } },
//                    { ">", { STDIN_FILENO, STDERR_FILENO } },
//                    { "<", { STDOUT_FILENO, STDERR_FILENO } }
//            };
//            for (auto &std_fd : dict[redirect_type]) {
//                if (close(std_fd) == -1)
//                    encrypted_cerr_if_needed("fork_redirect(): failed to close std fd for detached mode");
//            }
//        }
//
//        int oflag = (redirect_type == ">" || redirect_type == "2>" ||
//                redirect_type == "2>&1" || redirect_type == "&>") ? O_WRONLY: O_RDONLY;
//        int fd = open(target_file_path.c_str(), oflag | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
//        int change_fd;
//        if (redirect_type == "2>&1" || redirect_type == "&>") {
//            // Duplicate file handler to redirect
//            if ((dup2(fd, STDERR_FILENO) == -1) || (dup2(fd, STDOUT_FILENO) == -1)) {
//                encrypted_cerr_if_needed("dup2: failed to duplicate stdin or stdout on read end of pipe");
//                return;
//            }
//
//            if (close(fd) == -1) {
//                encrypted_cerr_if_needed("fork_redirect(): failed to close target_file fd");
//                return;
//            }
//
//            exec_command(input_struct);
//            if (close(STDERR_FILENO) == -1)
//                encrypted_cerr_if_needed("fork_redirect(): failed to close stderr fd");
//            if (close(STDOUT_FILENO) == -1)
//                encrypted_cerr_if_needed("fork_redirect(): failed to close stdout fd");
//        }
//        else {
//            if (redirect_type == "2>") change_fd = STDERR_FILENO;
//            else if (redirect_type == ">") change_fd = STDOUT_FILENO;
//            else change_fd = STDIN_FILENO;
//
//            // Duplicate file handler to redirect
//            if (dup2(fd, change_fd) == -1) {
//                encrypted_cerr_if_needed("dup2: failed to duplicate stdin or stdout on read end of pipe");
//                return;
//            }
//
//            if (close(fd) == -1) {
//                encrypted_cerr_if_needed("fork_redirect(): failed to close target_file fd");
//                return;
//            }
//
//            exec_command(input_struct);
//            if (close(change_fd) == -1)
//                encrypted_cerr_if_needed("fork_redirect(): failed to close stdin or stdout fd");
//        }
//    }
//}
