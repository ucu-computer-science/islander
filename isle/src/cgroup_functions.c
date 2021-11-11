#define _XOPEN_SOURCE 500
#include <sys/stat.h>
#include <ftw.h>
#include <stdio.h>

#include "../inc/base_header.h"
#include "../inc/cgroup_functions.h"
#include "../inc/helper_functions.h"


void config_cgroup_limits(int pid) {
    char group_name[64];
    group_name[0] = '\0';
    get_cgroup_name(group_name, pid);

    // set up memory limit
    config_cgroup_subsystem("memory", group_name, "memory.limit_in_bytes", "1G", pid);
    config_cgroup_subsystem("cpu", group_name, "cpu.cfs_quota_us", "100000", pid);
    config_cgroup_subsystem("blkio", group_name, "blkio.throttle.write_bps_device", "8:0 10485760", pid);
//    config_cgroup_subsystem("cpuset", group_name, "cpuset.cpus", "1", pid);
}


void config_cgroup_subsystem(char subsystem[], char group_name[], char subsystem_filename[],
                         char *limit_value, int pid) {
    char *strings[] = {CGROUP_ROOT_PATH, subsystem, "/", PROGRAM_NAME};
    char subsystem_path[256];

    // very important to add an init value to string and '\0' is a good choice
    subsystem_path[0] = '\0';
    str_array_concat(subsystem_path, strings, 4);

    // create PROGRAM_NAME directory in CGROUP_ROOT_PATH
    create_dir(subsystem_path);

    // create group_<PID> directory in CGROUP_ROOT_PATH
    char program_subsystem_path[256];
    program_subsystem_path[0] = '\0';
    char *str_arr[] = {subsystem_path, "/", group_name};
    str_array_concat(program_subsystem_path, str_arr, 3);
    create_dir(program_subsystem_path);

    // write limit_value to limit special resource in cgroup char subsystem[]
    char subsystem_file_path[256];
    subsystem_file_path[0] = '\0';
    char *string2[] = {program_subsystem_path, "/", subsystem_filename};
    str_array_concat(subsystem_file_path, string2, 3);
    write_file(subsystem_file_path, limit_value);

    // convert int to string
    char str_num[16];
    sprintf(str_num, "%d", pid);

    // write pid to set limit of cgroup subsystem to the process
    char subsystem_tasks_path[256];
    subsystem_tasks_path[0] = '\0';
    char *string3[] = {program_subsystem_path, "/", "tasks"};
    str_array_concat(subsystem_tasks_path, string3, 3);
    write_file(subsystem_tasks_path, str_num);
#ifdef DEBUG_MODE
    printf("subsystem_file_path -- %s\n", subsystem_file_path);
    printf("subsystem_tasks_path -- %s\n", subsystem_tasks_path);
#endif
}


void rm_cgroup_dirs(int pid) {
    char group_name[128];
    group_name[0] = '\0';
    get_cgroup_name(group_name, pid);
    rm_cgroup_dir("memory", group_name);
    rm_cgroup_dir("cpu", group_name);
    rm_cgroup_dir("blkio", group_name);
//    rm_cgroup_dir("cpuset", group_name);
}


void rm_cgroup_dir(char subsystem[], char group_name[]) {
    char *str_arr[] = {CGROUP_ROOT_PATH, subsystem, "/", PROGRAM_NAME, "/", group_name};
    char subsystem_path[256];
    subsystem_path[0] = '\0';
    str_array_concat(subsystem_path, str_arr, 6);

    if (rmdir(subsystem_path) == 0) {
        printf("Deleted a directory -- %s\n", subsystem_path);
    } else {
        printf("Unable to delete directory-- %s. Reason -- %s\n", subsystem_path, strerror(errno));
    }
}
