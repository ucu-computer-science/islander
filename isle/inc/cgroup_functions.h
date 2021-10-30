#ifndef NAMESPACES_CGROUP_FUNCTIONS_H
#define NAMESPACES_CGROUP_FUNCTIONS_H

#define CGROUP_ROOT_PATH "/sys/fs/cgroup/"
#define PROGRAM_NAME "islenet"


void config_cgroup_limits(int pid);

void config_cgroup_subsystem(char subsystem[], char group_name[], char subsystem_filename[],
                            char *limit_value, int pid);

int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);

int rmrf(char *path);

void rm_cgroup_dirs(int pid);

void rm_cgroup_dir(char subsystem[], char group_name[]);

#endif //NAMESPACES_CGROUP_FUNCTIONS_H
