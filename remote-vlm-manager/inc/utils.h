#ifndef REMOTE_VLM_MANAGER_UTILS_H
#define REMOTE_VLM_MANAGER_UTILS_H

#include "./base_header.h"

std::string exec_bash_command(const std::string &cmd, int& out_exitStatus);

#endif //REMOTE_VLM_MANAGER_UTILS_H
