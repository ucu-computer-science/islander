#ifndef NAMESPACES_CGROUP_FUNCTIONS_H
#define NAMESPACES_CGROUP_FUNCTIONS_H

#define CGROUP_ROOT_PATH "/sys/fs/cgroup/"

void config_cgroup_limits(int pid);

void config_cgroup_subsystem(char subsystem[], char group_name[], char subsystem_filename[],
                            char *limit_value, int pid);

#endif //NAMESPACES_CGROUP_FUNCTIONS_H
