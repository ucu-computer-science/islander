#ifndef ISLANDER_ENGINE_FUNCS_WITH_PROCESS_H
#define ISLANDER_ENGINE_FUNCS_WITH_PROCESS_H

#include "./base_header.h"
#include "./helper_functions.h"
#include "./syscall_wrappers.h"

void log_process_output(int log_pipe_fd[PIPE_FD_NUM]);

#endif //ISLANDER_ENGINE_FUNCS_WITH_PROCESS_H
