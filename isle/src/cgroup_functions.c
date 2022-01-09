// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#define _XOPEN_SOURCE 500
#include <sys/stat.h>
#include <ftw.h>
#include <stdio.h>

#include "../inc/base_header.h"
#include "../inc/cgroup_functions.h"
#include "../inc/helper_functions.h"


void set_up_default_limits(resource_limits *res_limits) {
    // Default limits for cgroup
    res_limits->memory_in_bytes = "500M";
    res_limits->cpu_shares = "100";
    res_limits->cpu_quota = "100000";
    res_limits->cpu_period = "1000000";
    res_limits->device_read_bps = "524288000";
    res_limits->device_write_bps = "104857600";
}


void config_cgroup_limits(int pid, resource_limits *res_limits) {
    char group_name[64];
    group_name[0] = '\0';
    get_cgroup_name(group_name, pid);

    char *str_arr2[] = {"8:0 ", res_limits->device_read_bps};
    char read_bps_device_value[256];
    read_bps_device_value[0] = '\0';
    str_array_concat(read_bps_device_value, str_arr2, 2);

    char *str_arr[] = {"8:0 ", res_limits->device_write_bps};
    char write_bps_device_value[256];
    write_bps_device_value[0] = '\0';
    str_array_concat(write_bps_device_value, str_arr, 2);

    // set up memory limit
    config_cgroup_subsystem("memory", group_name, "memory.limit_in_bytes", res_limits->memory_in_bytes, pid);
    config_cgroup_subsystem("cpu", group_name, "cpu.shares", res_limits->cpu_shares, pid);
    config_cgroup_subsystem("cpu", group_name, "cpu.cfs_period_us", res_limits->cpu_period, pid);
    config_cgroup_subsystem("cpu", group_name, "cpu.cfs_quota_us", res_limits->cpu_quota, pid);
    config_cgroup_subsystem("blkio", group_name, "blkio.throttle.read_bps_device", read_bps_device_value, pid);
    config_cgroup_subsystem("blkio", group_name, "blkio.throttle.write_bps_device", write_bps_device_value, pid);
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
}


void rm_cgroup_dir(char subsystem[], char group_name[]) {
    char *str_arr[] = {CGROUP_ROOT_PATH, subsystem, "/", PROGRAM_NAME, "/", group_name};
    char subsystem_path[256];
    subsystem_path[0] = '\0';
    str_array_concat(subsystem_path, str_arr, 6);

    if (rmdir(subsystem_path) == 0) {
#ifdef DEBUG_MODE
        printf("Deleted a directory -- %s\n", subsystem_path);
#endif
    } else {
        printf("Unable to delete directory-- %s. Reason -- %s\n", subsystem_path, strerror(errno));
    }
}
