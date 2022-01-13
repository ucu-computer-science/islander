#ifndef MYSHELL_REDIRECT_H
#define MYSHELL_REDIRECT_H

#include "./base_header.hpp"

void redirect_cmd(
        command_line_input &input_struct,
        std::string &target_file,
        std::string &redirect_type,
        std::map<std::string, std::function<void (command_line_input&, int, int)>> &command_to_function,
        std::map<std::string, std::string> &command_to_help,
        bool piped
);

void fork_redirect(const command_line_input &input_struct, const std::string &target_file, const std::string &redirect_type, bool piped);

#endif //MYSHELL_REDIRECT_H
