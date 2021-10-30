//#include <sys/types.h>
#include <sys/stat.h>

#include "../inc/base_header.h"
#include "../inc/cgroup_functions.h"


void config_cgroup_limits(int pid) {
    char group_prefix[] = "group";
    char str_num[16];

    // convert 123 to string [buf]
    sprintf(str_num, "%d", pid);
    char *group_name = str_concat(group_prefix, str_num);

    // print our string
    printf("group_name -- %s\n", group_name);

    // set up memory limit
    config_cgroup_subsystem("memory", group_name, "memory.limit_in_bytes", "150M", pid);

    // TODO: rm dirs in cgroup subsystems
}


void config_cgroup_subsystem(char subsystem[], char group_name[], char subsystem_filename[],
                         char *limit_value, int pid) {
    struct stat st = {0};

    char *strings[] = {CGROUP_ROOT_PATH, subsystem, "/", group_name};
    char *subsystem_path = str_array_concat(strings, 4);
//    if (stat(subsystem_path, &st) == -1) {
//        mkdir("~/new-my-dir", 0777);
//        printf("Created a new directory -- %s\n", subsystem_path);
//    }

    // 0700 meaning -- http://www.filepermissions.com/file-permission/0700
    // permission codes -- https://man7.org/linux/man-pages/man7/inode.7.html
//    int check = mkdir(subsystem_path,0777);

    // check if directory is created or not
//    mode_t target_mode = 0777;
    mode_t target_mode = 0700;
    if (mkdir(subsystem_path, target_mode) == 0) {
        // reason of using chmod -- https://stackoverflow.com/questions/39737609/why-cant-my-program-set-0777-mode-with-the-mkdir-system-call
//        chmod(subsystem_path, target_mode);
        printf("Created a new directory -- %s\n", subsystem_path);
    } else {
        printf("Unable to create directory-- %s. Reason -- %s\n", subsystem_path, strerror(errno));
        exit(1);
    }

    char *string2 = str_concat("/", subsystem_filename);
    char *subsystem_file_path = str_concat(subsystem_path, string2);
    printf("subsystem_path -- %s\n", subsystem_file_path);
//    write_file(subsystem_file_path, limit_value);
}
