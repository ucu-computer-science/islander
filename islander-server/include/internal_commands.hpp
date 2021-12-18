#ifndef MYSHELL_INTERNAL_COMMANDS_HPP
#define MYSHELL_INTERNAL_COMMANDS_HPP

#include <iostream>
#include <unistd.h>
#include <vector>

#include "base_header.hpp"

#define BUFFER_SIZE 512

#define OK 0
#define GET_CWD_ERR -1
#define SMALL_BUFFER_SIZE_ERR -2
#define CHDIR_ERR -3
#define DIR_NOT_FOUND -4
#define TOO_MANY_ARGS_ERR -5
#define TOO_LITTLE_ARGS_ERR -6

void mpwd(command_line_input &cmd_input, int std_out, int std_err);

void mcd(command_line_input &cmd_input, int std_out, int std_err);

void merrno(command_line_input &cmd_input, int std_out, int std_err);

void mexit(command_line_input &input_struct, int std_out, int std_err);

void mexport(command_line_input &input_struct, int std_out, int std_err);

void mecho(command_line_input &input_struct, int std_out, int std_err);


#endif //MYSHELL_INTERNAL_COMMANDS_HPP
