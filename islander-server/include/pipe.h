#ifndef MYSHELL_PIPE_H
#define MYSHELL_PIPE_H

#include "base_header.hpp"

/* Close all unused ends of pipe and duplicate the pipe end for either read or write or both.
 * in: the READ end of pipe (fd)
 * out: the WRITE end of pipe (fd) */
int prepare_pipe(int in, int out, command_line_input &cmd, int pfd[2], bool detached_mode, std::string redirect_type);

/* Execute pipe command. */
int fork_pipes(std::vector<command_line_input> &pipe_commands, bool detached_mode);

#endif //MYSHELL_PIPE_H
