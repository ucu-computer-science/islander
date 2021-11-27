#ifndef MANAGE_VOLUMES_H
#define MANAGE_VOLUMES_H

#include <stdint.h>

void volume_feature(int isle_pid, struct process_params *params, const char *exec_file_path);

void unmount_volumes(int isle_pid, struct process_params *params);

void mount_ns_volume(int isle_pid, char* src_vlm_name, char* dest_vlm_path, const char *exec_file_path);

void fork_vlm_mount(int isle_pid, char* src_vlm_name, char* dest_dir_path);

#endif //MANAGE_VOLUMES_H
