#ifndef ISLANDER_ENGINE_MANAGE_TMPFS_H
#define ISLANDER_ENGINE_MANAGE_TMPFS_H

#include "../base_header.h"
#include "../helper_functions.h"

void mount_ns_tmpfs(int isle_pid, struct process_params *params);

#endif //ISLANDER_ENGINE_MANAGE_TMPFS_H
