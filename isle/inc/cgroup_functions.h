#ifndef NAMESPACES_CGROUP_FUNCTIONS_H
#define NAMESPACES_CGROUP_FUNCTIONS_H

#define CGROUP_ROOT_PATH "/sys/fs/cgroup/"
#define PROGRAM_NAME "islenet"


void set_up_default_limits(resource_limits *res_limits);

void config_cgroup_limits(int pid, resource_limits *res_limits);

void config_cgroup_subsystem(char subsystem[], char group_name[], char subsystem_filename[],
                            char *limit_value, int pid);

void rm_cgroup_dirs(int pid);

void rm_cgroup_dir(char subsystem[], char group_name[]);

#endif //NAMESPACES_CGROUP_FUNCTIONS_H
