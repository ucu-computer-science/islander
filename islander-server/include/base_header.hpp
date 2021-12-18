#ifndef MYSHELL_BASE_HEADER_HPP
#define MYSHELL_BASE_HEADER_HPP

#include <iostream>
#include <boost/program_options.hpp>
#include <vector>
#include <stdlib.h>

using std::cin;
using std::cout;
using std::cerr;
using std::endl;


struct command_line_input {
    std::string command;
    std::vector<std::string> args;
    std::vector<std::string> flags;
    bool help_flag = false;
    bool detached_mode = false;
};

#endif //MYSHELL_BASE_HEADER_HPP
