#ifndef MYSHELL_EXECUTION_FUNCTS_HPP
#define MYSHELL_EXECUTION_FUNCTS_HPP


#include "base_header.hpp"

void execute_file(std::string &filename);

void run_msh_script(std::string &filename);

void run_command(const std::string &buf);

void run_external_command(int* read_pfd, int* write_pfd, std::string& cmd, char *devname, int* ptm);

void exec_command(int* read_pfd, int* write_pfd, std::string& cmd, int* pts) ;

#endif //MYSHELL_EXECUTION_FUNCTS_HPP
