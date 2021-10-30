#define _XOPEN_SOURCE 500
#include <sys/stat.h>
#include <ftw.h>
#include <stdio.h>

#include "../inc/base_header.h"
#include "../inc/cgroup_functions.h"


void config_cgroup_limits(int pid) {
    char *group_name = get_cgroup_name(pid);

    // set up memory limit
    config_cgroup_subsystem("memory", group_name, "memory.limit_in_bytes", "1G", pid);
//    free(group_name);
}


void config_cgroup_subsystem(char subsystem[], char group_name[], char subsystem_filename[],
                         char *limit_value, int pid) {
    char *strings[] = {CGROUP_ROOT_PATH, subsystem, "/", PROGRAM_NAME};
    char *subsystem_path = str_array_concat(strings, 4);
    create_dir(subsystem_path);

    char *str_arr[] = {subsystem_path, "/", group_name};
    char *program_subsystem_path = str_array_concat(str_arr, 3);
    create_dir(program_subsystem_path);

//    char slash[] = "/";
    char *string2 = str_concat("/", subsystem_filename);

//    char *subsystem_file_path = NULL;
//    strcpy(subsystem_file_path, subsystem_path);
//    strcat(subsystem_file_path, string2);
    char* subsystem_file_path = str_concat(program_subsystem_path, string2);
    printf("subsystem_file_path -- %s\n", subsystem_file_path);

    // convert int to string
    char str_num[16];
    sprintf(str_num, "%d", pid);

//    char *string3 = "/";
//    strcat(string3, "tasks");
    char *string3 = str_concat("/", "tasks");
//    char *subsystem_tasks_path = NULL;
//    strcpy(subsystem_tasks_path, subsystem_path);
//    strcat(subsystem_path, string3);
    char *subsystem_tasks_path = str_concat(program_subsystem_path, string3);

    printf("program_subsystem_path2 -- %s\n", program_subsystem_path);
    printf("string3 -- %s\n", string3);
    printf("subsystem_file_path2 -- %s\n", subsystem_file_path);
    printf("subsystem_tasks_path -- %s\n", subsystem_tasks_path);
    write_file(subsystem_tasks_path, str_num);
    write_file(subsystem_file_path, limit_value);

    memset(string2, 0, strlen(string2));
    memset(string3, 0, strlen(string3));
    memset(program_subsystem_path, 0, strlen(program_subsystem_path));
    memset(subsystem_path, 0, strlen(subsystem_path));
    memset(subsystem_tasks_path, 0, strlen(subsystem_tasks_path));
    memset(subsystem_file_path, 0, strlen(subsystem_file_path));
}


//int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
//    int rv = remove(fpath);
//
//    if (rv)
//        perror(fpath);
//
//    return rv;
//}
//
//int rmrf(char *path) {
//    return nftw(path, unlink_cb, 64, FTW_DEPTH | FTW_PHYS);
//}


void rm_cgroup_dirs(int pid) {
    char *group_name = get_cgroup_name(pid);
    rm_cgroup_dir("memory", group_name);
    free(group_name);
}


void rm_cgroup_dir(char subsystem[], char group_name[]) {
    printf("\n\n rm_cgroup_dir\n");
    char *strings[] = {CGROUP_ROOT_PATH, subsystem, "/", PROGRAM_NAME, "/", group_name};
    char *subsystem_path = str_array_concat(strings, 6);

    if (rmdir(subsystem_path) == 0) {
        printf("Deleted a directory -- %s\n", subsystem_path);
    } else {
        printf("Unable to delete directory-- %s. Reason -- %s\n", subsystem_path, strerror(errno));
    }
    free(subsystem_path);
}
